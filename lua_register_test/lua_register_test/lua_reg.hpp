#ifndef __LUA_REGISTER_HPP
#define __LUA_REGISTER_HPP

#include <map>
#include <set>
#include <vector>

#include <cstdint>
#include <string>
#include <functional>
#include <tuple>
#include <memory>
#include <atomic>
#include <algorithm>

#include <lua.hpp>


namespace lua {

	struct return_number_t
	{
		std::uint32_t num_;
		return_number_t(std::uint32_t num)
			: num_(num)
		{}
	};

	template < std::uint32_t N >
	void check_stack(lua_State *state)
	{
		int n = lua_gettop(state);
		assert(n == N);
	}


	template < typename T, typename EnableT = void >
	struct lua_value_t;

	template < typename T >
	struct lua_value_t<T, 
		typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value>::type >
	{
		static T get(lua_State *state, int index)
		{
			assert(::lua_isnumber(state, index) != 0);

			return static_cast<T>(lua_tonumber(state, index));
		}

		static std::uint32_t set(lua_State *state, T val)
		{
			lua_pushnumber(state, static_cast<lua_Number>(val));
			return 1;
		}
	};

	template <>
	struct lua_value_t<bool>
	{
		static bool get(lua_State *state, int index)
		{
			assert(lua_isboolean(state, index) != 0);
			int n = lua_toboolean(state, index);
			return n != 0;
		}

		static std::uint32_t set(lua_State *state, bool val)
		{
			lua_pushboolean(state, val);
			return 1;
		}
	};

	template < typename CharT, typename CharTraitsT, typename AllocatorT >
	struct lua_value_t<std::basic_string<CharT, CharTraitsT, AllocatorT>>
	{
		static std::basic_string<CharT, CharTraitsT, AllocatorT> get(lua_State *state, int index)
		{
			assert(::lua_isstring(state, index) != 0);
			return lua_tostring(state, index);
		}

		static std::uint32_t set(lua_State *state, const std::basic_string<CharT, CharTraitsT, AllocatorT> &val)
		{
			lua_pushstring(state, val.c_str());
			return 1;
		}
	};

	template < typename FirstT, typename SecondT >
	struct lua_value_t< std::pair<FirstT, SecondT> >
	{
		typedef std::pair<FirstT, SecondT> pair_t;

		static pair_t get(lua_State *state, int index)
		{
			const int len = lua_objlen(state, index);
			assert(len == 2);

			lua_rawgeti(state, index, 1);
			FirstT first_val = lua_value_t<FirstT>::get(state, -1);
			lua_pop(state, 1);
			
			lua_rawgeti(state, index, 2);
			SecondT second_val = lua_value_t<SecondT>::get(state, -1);
			lua_pop(state, 1);

			return std::make_pair(first_val, second_val);
		}

		static std::uint32_t set(lua_State *state, const pair_t &val)
		{
			lua_createtable(state, 2, 0);
			auto index = lua_gettop(state);

			lua_value_t<FirstT>::set(state, val.first);
			lua_rawseti(state, -2, 1);

			lua_value_t<SecondT>::set(state, val.second);
			lua_rawseti(state, -2, 2);

			return 1;
		}
	};

	template < typename T >
	struct lua_value_t< std::pair<const T *, std::uint32_t> >
	{
		static_assert(std::is_pod<T>::value, "T must be a pod type");

		static std::pair<const T *, std::uint32_t> get(lua_State *state, int index)
		{
			std::uint32_t len = 0;
			const char *p = lua_tolstring(state, index, &len);

			return std::make_pair(reinterpret_cast<const T *>(p), len / sizeof(T));
		}

		static std::uint32_t set(lua_State *state, const std::pair<const T *, std::uint32_t> &val)
		{
			lua_pushlstring(state, reinterpret_cast<const char *>(val.first), val.second * sizeof(T));
			return 1;
		}
	};

	template < typename T, typename AllocatorT >
	struct lua_value_t< std::vector<T, AllocatorT> >
	{
		typedef std::vector<T, AllocatorT> vector_t;

		static vector_t get(lua_State *state, int index)
		{
			vector_t vec;
			const int len = lua_objlen(state, index);
			vec.reserve(len);
			for(auto i = 1; i <= len; ++i) 
			{
				lua_rawgeti(state, index, i);
				vec.push_back(static_cast<T>(lua_value_t<T>::get(state, -1)));
				lua_pop(state, 1);
			}

			return vec;
		}

		static std::uint32_t set(lua_State *state, const vector_t &val)
		{
			lua_createtable(state, val.size(), 0);

			std::uint32_t i = 1;
			std::for_each(val.begin(), val.end(), 
				[state, &i](const T &t)
			{
				lua_value_t<T>::set(state, t);
				lua_rawseti(state, -2, i++);
			});

			return val.size() == 0 ? 0 : 1;
		}
	};


	template < >
	struct lua_value_t< return_number_t >
	{
		static return_number_t get(lua_State *state, int index)
		{
			assert(0 && "not implement");
			return return_number_t(0);
		}

		static std::uint32_t set(lua_State *state, const return_number_t &val)
		{
			return val.num_;
		}
	};
	

	namespace details {


		template < std::uint32_t N, typename TupleT >
		struct auto_check_value_t
		{
			typedef typename std::tuple_element<N, TupleT>::type element_param_t;
			typedef typename std::remove_const<typename std::remove_reference<element_param_t>::type>::type param_t;

			static auto check(lua_State *state)->decltype(lua_value_t<param_t>::get(state, N + 1))
			{
				// lua stack index begin from 1
				return lua_value_t<param_t>::get(state, N + 1);
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
				check_stack<0>(state);
				handler();
			}
		};

		template < typename R, typename TupleT >
		struct param_handler_t<1, R, TupleT>
		{
			template < typename HandlerT >
			static R handle(HandlerT &&handler, lua_State *state)
			{
				check_stack<1>(state);
				return handler(std::cref(auto_check_value_t<0, TupleT>::check(state)));
			}
		};

		template < typename TupleT >
		struct param_handler_t<1, void, TupleT>
		{
			template < typename HandlerT >
			static void handle(HandlerT &&handler, lua_State *state)
			{
				check_stack<1>(state);
				handler(std::cref(auto_check_value_t<0, TupleT>::check(state)));
			}
		};

		template < typename R, typename TupleT >
		struct param_handler_t<2, R, TupleT>
		{
			template < typename HandlerT >
			static R handle(HandlerT &&handler, lua_State *state)
			{
				check_stack<2>(state);
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
				check_stack<2>(state);
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
				check_stack<3>(state);
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
				check_stack<3>(state);
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
				check_stack<4>(state);
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
				check_stack<4>(state);
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
				check_stack<5>(state);
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
				check_stack<5>(state);
				handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)),
					std::cref(auto_check_value_t<2, TupleT>::check(state)),
					std::cref(auto_check_value_t<3, TupleT>::check(state)),
					std::cref(auto_check_value_t<4, TupleT>::check(state)));
			}
		};


		template < typename R, typename TupleT >
		struct param_handler_t<6, R, TupleT>
		{
			template < typename HandlerT >
			static R handle(HandlerT &&handler, lua_State *state)
			{
				check_stack<6>(state);
				return handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)),
					std::cref(auto_check_value_t<2, TupleT>::check(state)),
					std::cref(auto_check_value_t<3, TupleT>::check(state)),
					std::cref(auto_check_value_t<4, TupleT>::check(state)),
					std::cref(auto_check_value_t<5, TupleT>::check(state)));
			}
		};

		template < typename TupleT >
		struct param_handler_t<6, void, TupleT>
		{
			template < typename HandlerT >
			static void handle(HandlerT &&handler, lua_State *state)
			{
				check_stack<6>(state);
				handler(
					std::cref(auto_check_value_t<0, TupleT>::check(state)),
					std::cref(auto_check_value_t<1, TupleT>::check(state)),
					std::cref(auto_check_value_t<2, TupleT>::check(state)),
					std::cref(auto_check_value_t<3, TupleT>::check(state)),
					std::cref(auto_check_value_t<4, TupleT>::check(state)),
					std::cref(auto_check_value_t<5, TupleT>::check(state)));
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

				return lua_value_t<R>::set(state, ret);
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


		using namespace std::placeholders;
		template < typename T >
		struct bind_helper_t;

		template < typename R >
		struct bind_helper_t<R()>
		{
			template < typename FuncT >
			static auto bind(FuncT func)->decltype(std::bind(func))
			{
				return std::bind(func);
			}

			template < typename FuncT, typename T >
			static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj))
			{
				return std::bind(func, obj);
			}
		};


		template < typename R, typename T1 >
		struct bind_helper_t<R(T1)>
		{
			template < typename FuncT >
			static auto bind(FuncT func)->decltype(std::bind(func, _1))
			{
				return std::bind(func, _1);
			}

			template < typename FuncT, typename T >
			static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1))
			{
				return std::bind(func, obj, _1);
			}
		};

		template < typename R, typename T1, typename T2 >
		struct bind_helper_t<R(T1, T2)>
		{
			template < typename FuncT >
			static auto bind(FuncT func)->decltype(std::bind(func, _1, _2))
			{
				return std::bind(func, _1, _2);
			}

			template < typename FuncT, typename T >
			static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2))
			{
				return std::bind(func, obj, _1, _2);
			}
		};

		template < typename R, typename T1, typename T2, typename T3 >
		struct bind_helper_t<R(T1, T2, T3)>
		{
			template < typename FuncT >
			static auto bind(FuncT func)->decltype(std::bind(func, _1, _2, _3))
			{
				return std::bind(func, _1, _2, _3);
			}

			template < typename FuncT, typename T >
			static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2, _3))
			{
				return std::bind(func, obj, _1, _2, _3);
			}
		};

		template < typename R, typename T1, typename T2, typename T3, typename T4 >
		struct bind_helper_t<R(T1, T2, T3, T4)>
		{
			template < typename FuncT >
			static auto bind(FuncT func)->decltype(std::bind(func, _1, _2, _3, _4))
			{
				return std::bind(func, _1, _2, _3, _4);
			}

			template < typename FuncT, typename T >
			static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2, _3, _4))
			{
				return std::bind(func, obj, _1, _2, _3, _4);
			}
		};

		template < typename R, typename T1, typename T2, typename T3, typename T4, typename T5 >
		struct bind_helper_t<R(T1, T2, T3, T4, T5)>
		{
			template < typename FuncT >
			static auto bind(FuncT func)->decltype(std::bind(func, _1, _2, _3, _4, _5))
			{
				return std::bind(func, _1, _2, _3, _4, _5);
			}

			template < typename FuncT, typename T >
			static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2, _3, _4, _5))
			{
				return std::bind(func, obj, _1, _2, _3, _4, _5);
			}
		};

		template < typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
		struct bind_helper_t<R(T1, T2, T3, T4, T5, T6)>
		{
			template < typename FuncT >
			static auto bind(FuncT func)->decltype(std::bind(func, _1, _2, _3, _4, _5, _6))
			{
				return std::bind(func, _1, _2, _3, _4, _5, _6);
			}

			template < typename FuncT, typename T >
			static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2, _3, _4, _5, _6))
			{
				return std::bind(func, obj, _1, _2, _3, _4, _5, _6);
			}
		};
	}
	
	
	template < typename R, typename ...Args >
	void reg(lua_State *state, const std::string &name, R (*func)(Args...) )
	{
		register_instance().reg(state, name, details::bind_helper_t<R(Args...)>::bind(func), func);
	}

	template < typename R, typename U, typename T, typename ...Args >
	void reg(lua_State *state, const std::string &name, U obj, R (T::*func)(Args...))
	{
		register_instance().reg(state, name, details::bind_helper_t<R (Args...)>::bind(func, obj), func);
	}

	// fix ms bug
	template < typename R >
	void reg(lua_State *state, const std::string &name, R (*func)() )
	{
		register_instance().reg(state, name, details::bind_helper_t<R()>::bind(func), func);
	}

	template < typename R, typename U, typename T >
	void reg(lua_State *state, const std::string &name, U obj, R (T::*func)())
	{
		register_instance().reg(state, name, details::bind_helper_t<R()>::bind(func, obj), func);
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

	register_t &register_instance();


}

#endif