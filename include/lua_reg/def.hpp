#ifndef __LUA_REG_DEF_HPP
#define __LUA_REG_DEF_HPP

#include <memory>
#include <set>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <unordered_map>

#include "config.hpp"
#include "state.hpp"

#include "details/dispatcher.hpp"
#include "details/traits.hpp"



namespace luareg {

	template < typename R, typename ...Args >
	void reg(state_t &state, const std::string &name, R (*func)(Args...) )
	{
		register_instance().reg(state, name, details::bind_helper_t<R(Args...)>::bind(func), func);
	}

	template < typename R, typename U, typename T, typename ...Args >
	void reg(state_t &state, const std::string &name, U obj, R (T::*func)(Args...))
	{
		register_instance().reg(state, name, details::bind_helper_t<R(Args...)>::bind(func, std::forward<U>(obj)), func);
	}

	template < typename R, typename U, typename T, typename ...Args >
	void reg(state_t &state, const std::string &name, U obj, R(T::*func)(Args...) const)
	{
		R(T::*tmp)(Args...) = (R(T::*)(Args...))func;
		register_instance().reg(state, name, details::bind_helper_t<R(Args...)>::bind(tmp, std::forward<U>(obj)), tmp);
	}

	template < typename T >
	void reg(state_t &state, const std::string &name, T &&func)
	{
		register_instance().reg(state, name, std::forward<T>(func));
	}

	struct register_t
	{
		typedef std::uint32_t callback_function_id_t;

		std::map<callback_function_id_t, details::handler_base_ptr> handler_map_;

		
		typedef std::map<callback_function_id_t, ::lua_CFunction> callback_map_t;
		callback_map_t callback_map_;

		typedef std::set<std::string> reg_function_t;
		reg_function_t reg_functions_;

		typedef std::unordered_map<std::string, callback_function_id_t> function_name_map_id_t;
		function_name_map_id_t name_2_id_;

		std::mutex reg_mutex_;
		std::atomic<callback_function_id_t> callback_index_;

		enum { MAX_CALLBACK_NUM = 200 };


		register_t();
		~register_t();

		template < typename HandlerT, typename R, typename ...Args >
		void reg(state_t &state, const std::string &name, HandlerT &&handler, R (*func)(Args...) )
		{
			typedef std::tuple<Args...> tuple_t;
			typedef details::handler_impl_t<HandlerT, R, tuple_t> handler_t;

			reg_impl<handler_t>(state, name, std::forward<HandlerT>(handler));
		}

		template < typename HandlerT, typename R, typename T, typename ...Args >
		void reg(state_t &state, const std::string &name, HandlerT &&handler, R (T::*func)(Args...) )
		{
			typedef std::tuple<Args...> tuple_t;
			typedef details::handler_impl_t<HandlerT, R, tuple_t> handler_t;

			reg_impl<handler_t>(state, name, std::forward<HandlerT>(handler));
		}

		template < typename HandlerT >
		void reg(state_t &state, const std::string &name, HandlerT && handler)
		{
			typedef details::function_traits_t<HandlerT>::args_type			tuple_t;
			typedef details::function_traits_t<HandlerT>::result_type		result_t;
			typedef details::handler_impl_t<HandlerT, result_t, tuple_t>	handler_t;

			reg_impl<handler_t>(state, name, std::forward<HandlerT>(handler));
		}

		template < typename HandlerImplT, typename HandlerT >
		void reg_impl(state_t &state, const std::string &name, HandlerT &&handler)
		{
			std::unique_lock<std::mutex> lock(reg_mutex_);

			callback_function_id_t index = 0;
			auto iter = reg_functions_.find(name);
			if( iter != reg_functions_.end() )
			{
				index = name_2_id_[name];
			}
			else
			{
				index = callback_index_++;
				reg_functions_.insert(name);

				name_2_id_[name] = index;
				handler_map_[index] = std::make_shared<HandlerImplT>(std::forward<HandlerT>(handler));
			}

			lua_register(state, name.c_str(), callback_map_[index]);
		}
	};


	template < std::uint32_t N >
	struct callback_t
	{
		static std::int32_t handler(state_t &state)
		{
			auto iter = register_instance().handler_map_.find(N);
			assert(iter != register_instance().handler_map_.end());

			return iter->second->handle(state);
		}

		static std::int32_t callback(lua_State *state)
		{
			return handler(state_t(state));
		}
	};


	template < std::uint32_t N >
	struct auto_register_callback_t
	{
		template < typename MapT >
		static void reg(MapT &callback_map)
		{
			callback_map[N] = &callback_t<N>::callback;
			auto_register_callback_t<N - 1>::reg(callback_map);
		}
	};

	template <>
	struct auto_register_callback_t<0>
	{
		template < typename MapT >
		static void reg(MapT &callback_map)
		{
			callback_map[0] = &callback_t<0>::callback;
		}
	};


	inline register_t::register_t()
	{
		auto_register_callback_t<MAX_CALLBACK_NUM>::reg(callback_map_);
		callback_index_ = 0;
	}

	inline register_t::~register_t()
	{

	}

	inline register_t &register_instance()
	{
		static std::once_flag flag;
		static std::unique_ptr<register_t> register_val;

		std::call_once(flag, []()
		{
			register_val = std::make_unique<register_t>();
		});
		return *register_val;
	}
}

#endif