#ifndef __LUA_REG_DEF_HPP
#define __LUA_REG_DEF_HPP

#include <memory>
#include <set>
#include <atomic>
#include <cstdint>

#include "config.hpp"
#include "state.hpp"
#include "details/def.hpp"

namespace luareg {

	

	template < typename R, typename ...Args >
	void reg(state_t &state, const std::string &name, R (*func)(Args...) )
	{
		register_instance().reg(state, name, details::bind_helper_t<R(Args...)>::bind(func), func);
	}

	template < typename R, typename U, typename T, typename ...Args >
	void reg(state_t &state, const std::string &name, U obj, R (T::*func)(Args...))
	{
		register_instance().reg(state, name, details::bind_helper_t<R (Args...)>::bind(func, std::forward<U>(obj)), func);
	}

	template < typename R, typename U, typename T, typename ...Args >
	void reg(state_t &state, const std::string &name, U obj, R (T::*func)(Args...) const)
	{
		static_assert(false, "not implement, not support const member function");
	}

	// fix ms bug
	template < typename R >
	void reg(state_t &state, const std::string &name, R (*func)() )
	{
		register_instance().reg(state, name, details::bind_helper_t<R()>::bind(func), func);
	}

	template < typename R, typename U, typename T >
	void reg(state_t &state, const std::string &name, U obj, R (T::*func)())
	{
		register_instance().reg(state, name, details::bind_helper_t<R()>::bind(func, obj), func);
	}

	struct register_t
	{
		struct handler_base_t
		{
			virtual ~handler_base_t(){}
			virtual std::int32_t handle(state_t &state) = 0;
		};
		typedef std::shared_ptr<handler_base_t> handler_base_ptr;

		template < typename HandlerT, typename R, typename TupleT >
		struct handler_impl_t
			: handler_base_t
		{
			typedef R return_t;
			typedef TupleT tuple_t;

			HandlerT handler_;
			handler_impl_t(HandlerT &&handler)
				: handler_(std::move(handler))
			{

			}

			virtual std::int32_t handle(state_t &state)
			{
				enum { TUPLE_SIZE = std::tuple_size<tuple_t>::value };

				return details::return_handle_t<TUPLE_SIZE, return_t, tuple_t>::handle(handler_, state);
			}
		};

		std::map<std::uint32_t, handler_base_ptr> handler_map_;

		typedef std::map<std::uint32_t, ::lua_CFunction> callback_map_t;
		callback_map_t callback_map_;

		typedef std::set<std::string> reg_function_t;
		reg_function_t reg_functions_;

		std::atomic<std::uint32_t> callback_index_;

		enum { MAX_CALLBACK_NUM = 50 };


		register_t();

		template < typename HandlerT, typename R, typename ...Args >
		void reg(state_t &state, const std::string &name, HandlerT &&handler, R (*func)(Args...) )
		{
			typedef std::tuple<Args...> tuple_t;
			typedef handler_impl_t<HandlerT, R, tuple_t> handler_t;

			reg_impl<handler_t>(state, name, std::forward<HandlerT>(handler));
		}

		template < typename HandlerT, typename R, typename T, typename ...Args >
		void reg(state_t &state, const std::string &name, HandlerT &&handler, R (T::*func)(Args...) )
		{
			typedef std::tuple<Args...> tuple_t;
			typedef handler_impl_t<HandlerT, R, tuple_t> handler_t;

			reg_impl<handler_t>(state, name, std::forward<HandlerT>(handler));
		}

		template < typename HandlerImplT, typename HandlerT >
		void reg_impl(state_t &state, const std::string &name, HandlerT &&handler)
		{
			auto iter = reg_functions_.find(name);
			assert(iter == reg_functions_.end());
			reg_functions_.insert(name);

			std::uint32_t index = callback_index_++;
			lua_register(state, name.c_str(), callback_map_[index]);

			handler_map_[index] = std::make_shared<HandlerImplT>(std::forward<HandlerT>(handler));
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
		static void reg(register_t::callback_map_t &callback_map)
		{
			callback_map[N] = &callback_t<N>::callback;
			auto_register_callback_t<N - 1>::reg(callback_map);
		}
	};

	template <>
	struct auto_register_callback_t<0>
	{
		static void reg(register_t::callback_map_t &callback_map)
		{
			callback_map[0] = &callback_t<0>::callback;
		}
	};


	register_t::register_t()
	{
		auto_register_callback_t<MAX_CALLBACK_NUM>::reg(callback_map_);
		callback_index_ = 0;
	}

	inline register_t &register_instance()
	{
		static register_t reg;

		return reg;
	}
}

#endif