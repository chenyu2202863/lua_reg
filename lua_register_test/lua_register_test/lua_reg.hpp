#ifndef __LUA_REGISTER_HPP
#define __LUA_REGISTER_HPP

#include <map>
#include <set>
#include <cstdint>
#include <string>
#include <functional>
#include <tuple>
#include <memory>
#include <atomic>

#include <lua.hpp>


namespace lua {


	struct register_t;
	register_t &register_instance();


	template < typename T, typename EnableT = void >
	struct lua_value_t
	{
		static T *get(lua_State *state, int index)
		{
			return reinterpret_cast<T*>(lua_touserdata(state, index));
		}

		static void set(lua_State *state, const T &val)
		{
			lua_pushlightuserdata(state, &const_cast<T &>(val));
		}
	};

	template < typename T >
	struct lua_value_t<T, 
		typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value>::type >
	{
		static T get(lua_State *state, int index)
		{
			return static_cast<T>(lua_tonumber(state, index));
		}

		static void set(lua_State *state, T val)
		{
			lua_pushnumber(state, static_cast<lua_Number>(val));
		}
	};

	template <>
	struct lua_value_t<bool>
	{
		static bool get(lua_State *state, int index)
		{
			int n = lua_toboolean(state, index);
			return n != 0;
		}

		static void set(lua_State *state, bool val)
		{
			lua_pushboolean(state, val);
		}
	};

	template < typename CharT, typename CharTraitsT, typename AllocatorT >
	struct lua_value_t<std::basic_string<CharT, CharTraitsT, AllocatorT>>
	{
		static std::basic_string<CharT, CharTraitsT, AllocatorT> get(lua_State *state, int index)
		{
			return lua_tostring(state, index);
		}

		static void set(lua_State *state, const std::basic_string<CharT, CharTraitsT, AllocatorT> &val)
		{
			lua_pushstring(state, val.c_str());
		}
	};


	namespace details {


		template < std::uint32_t N, typename TupleT >
		struct auto_check_value_t
		{
			typedef typename std::tuple_element<N, TupleT>::type element_param_t;
			typedef typename std::remove_const<typename std::remove_reference<element_param_t>::type>::type param_t;

			static param_t check(lua_State *state)
			{
				// lua stack index begin from 1
				param_t val = lua_value_t<param_t>::get(state, N + 1);
				return val;
			}
		};

		template < std::uint32_t N, typename R, typename TupleT >
		struct param_handler_t;

		template < typename R, typename TupleT >
		struct param_handler_t<0, R, TupleT>
		{
			template < typename HandlerT >
			static R handle(HandlerT &&handler, lua_State *state)
			{
				return handler();
			}
		};

		template < typename TupleT >
		struct param_handler_t<0, void, TupleT>
		{
			template < typename HandlerT >
			static void handle(HandlerT &&handler, lua_State *state)
			{
				handler();
			}
		};

		template < typename R, typename TupleT >
		struct param_handler_t<1, R, TupleT>
		{
			template < typename HandlerT >
			static R handle(HandlerT &&handler, lua_State *state)
			{
				return handler(std::cref(auto_check_value_t<0, TupleT>::check(state)));
			}
		};

		template < typename TupleT >
		struct param_handler_t<1, void, TupleT>
		{
			template < typename HandlerT >
			static void handle(HandlerT &&handler, lua_State *state)
			{
				handler(std::cref(auto_check_value_t<0, TupleT>::check(state)));
			}
		};

		template < typename R, typename TupleT >
		struct param_handler_t<2, R, TupleT>
		{
			template < typename HandlerT >
			static R handle(HandlerT &&handler, lua_State *state)
			{
				return handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)));
			}
		};

		template < typename TupleT >
		struct param_handler_t<2, void, TupleT>
		{
			template < typename HandlerT >
			static void handle(HandlerT &&handler, lua_State *state)
			{
				handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)));
			}
		};

		template < typename R, typename TupleT >
		struct param_handler_t<3, R, TupleT>
		{
			template < typename HandlerT >
			static R handle(HandlerT &&handler, lua_State *state)
			{
				return handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)),
					std::cref(auto_check_value_t<2, TupleT>::check(state)));
			}
		};

		template < typename TupleT >
		struct param_handler_t<3, void, TupleT>
		{
			template < typename HandlerT >
			static void handle(HandlerT &&handler, lua_State *state)
			{
				handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)),
					std::cref(auto_check_value_t<2, TupleT>::check(state)));
			}
		};

		template < typename R, typename TupleT >
		struct param_handler_t<4, R, TupleT>
		{
			template < typename HandlerT >
			static R handle(HandlerT &&handler, lua_State *state)
			{
				return handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)),
					std::cref(auto_check_value_t<2, TupleT>::check(state)),
					std::cref(auto_check_value_t<3, TupleT>::check(state)));
			}
		};

		template < typename TupleT >
		struct param_handler_t<4, void, TupleT>
		{
			template < typename HandlerT >
			static void handle(HandlerT &&handler, lua_State *state)
			{
				handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)),
					std::cref(auto_check_value_t<2, TupleT>::check(state)),
					std::cref(auto_check_value_t<3, TupleT>::check(state)));
			}
		};

		template < typename R, typename TupleT >
		struct param_handler_t<5, R, TupleT>
		{
			template < typename HandlerT >
			static R handle(HandlerT &&handler, lua_State *state)
			{
				return handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)),
					std::cref(auto_check_value_t<2, TupleT>::check(state)),
					std::cref(auto_check_value_t<3, TupleT>::check(state)),
					std::cref(auto_check_value_t<4, TupleT>::check(state)));
			}
		};

		template < typename TupleT >
		struct param_handler_t<5, void, TupleT>
		{
			template < typename HandlerT >
			static void handle(HandlerT &&handler, lua_State *state)
			{
				handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)),
					std::cref(auto_check_value_t<2, TupleT>::check(state)),
					std::cref(auto_check_value_t<3, TupleT>::check(state)),
					std::cref(auto_check_value_t<4, TupleT>::check(state)));
			}
		};


		// -----------

		template < std::uint32_t N, typename R, typename TupleT >
		struct return_handle_t
		{
			template < typename HandlerT >
			static std::int32_t handle(HandlerT &&handler, lua_State *state)
			{
				R ret = param_handler_t<N, R, TupleT>::handle(handler, state);

				lua_value_t<R>::set(state, ret);
				return 1;
			}
		};

		template < std::uint32_t N, typename TupleT >
		struct return_handle_t<N, void, TupleT>
		{
			template < typename HandlerT >
			static std::int32_t handle(HandlerT &&handler, lua_State *state)
			{
				param_handler_t<N, void, TupleT>::handle(std::forward<HandlerT>(handler), state);
				return 0;
			}
		};


	}
	

	struct register_t
	{
		struct handler_base_t
		{
			virtual ~handler_base_t(){}
			virtual std::int32_t handle(lua_State *state) = 0;
		};
		typedef std::shared_ptr<handler_base_t> handler_base_ptr;

		template < typename HandlerT, typename R, typename TupleT >
		struct handler_impl_t
			: handler_base_t
		{
			typedef R return_t;
			typedef TupleT tuple_t;

			HandlerT handler_;
			//static_assert(std::is_bind_expression<HandlerT>::value, "handler must be a std::bind type");
			
			handler_impl_t(HandlerT &&handler)
				: handler_(std::move(handler))
			{

			}

			virtual std::int32_t handle(lua_State *state)
			{
				enum { TUPLE_SIZE = std::tuple_size<tuple_t>::value };

				return details::return_handle_t<TUPLE_SIZE, return_t, tuple_t>::handle(handler_, state);
			}
		};

		std::map<std::uint32_t, handler_base_ptr> handler_map_;

		typedef std::map<std::uint32_t, lua_CFunction> callback_map_t;
		callback_map_t callback_map_;

		typedef std::set<std::string> reg_function_t;
		reg_function_t reg_functions_;

		std::atomic<std::uint32_t> callback_index_;

		enum { MAX_CALLBACK_NUM = 50 };

		

		register_t();

		template < typename HandlerT, typename R, typename ...Args >
		void reg(lua_State *state, const std::string &name, HandlerT &&handler, R (*func)(Args...) )
		{
			typedef std::tuple<Args...> tuple_t;
			typedef handler_impl_t<HandlerT, R, tuple_t> handler_t;

			reg_impl<handler_t>(state, name, std::forward<HandlerT>(handler));
		}

		template < typename HandlerT, typename R, typename T, typename ...Args >
		void reg(lua_State *state, const std::string &name, HandlerT &&handler, R (T::*func)(Args...) )
		{
			typedef std::tuple<Args...> tuple_t;
			typedef handler_impl_t<HandlerT, R, tuple_t> handler_t;

			reg_impl<handler_t>(state, name, std::forward<HandlerT>(handler));
		}

		template < typename HandlerImplT, typename HandlerT >
		void reg_impl(lua_State *state, const std::string &name, HandlerT &&handler)
		{
			auto iter = reg_functions_.find(name);
			assert(iter == reg_functions_.end());
			reg_functions_.insert(name);

			std::uint32_t index = callback_index_++;
			lua_register(state, name.c_str(), callback_map_[index]);

			handler_map_[index] = std::make_shared<HandlerImplT>(std::forward<HandlerT>(handler));
		}
	};



}

#endif