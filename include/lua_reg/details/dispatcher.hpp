#ifndef __LUA_REG_DISPATCHER_DETAILS_HPP
#define __LUA_REG_DISPATCHER_DETAILS_HPP

#include "traits.hpp"
#include "def.hpp"


namespace luareg { namespace details {


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
		handler_impl_t(HandlerT && handler)
			: handler_(std::move(handler))
		{

		}

		virtual std::int32_t handle(state_t &state)
		{
			enum
			{
				TUPLE_SIZE = std::tuple_size<tuple_t>::value
			};

			return details::return_handle_t<TUPLE_SIZE, return_t, tuple_t>::handle(handler_, state);
		}
	};



	template < typename T >
	struct class_name_t
	{
		static std::string name_;
	};

	template < typename T >
	std::string class_name_t<T>::name_;


	template < std::uint32_t N >
	struct args_parser_t
	{
		template < typename ...Args >
		static void parse(state_t &state, std::int32_t index, std::tuple<Args...> &args)
		{
			typedef std::tuple<Args...> tuple_t;
			typedef typename std::tuple_element<N, tuple_t>::type type;

			std::get<N>(args) = std::move(convertion_t<type>::from(state, index));
			args_parser_t<N - 1>::parse(state, index - 1, args);
		}
	};

	template < >
	struct args_parser_t<0>
	{
		template < typename ...Args >
		static void parse(state_t &state, std::int32_t index, std::tuple<Args...> &args)
		{
			typedef std::tuple<Args...> tuple_t;
			typedef typename std::tuple_element<0, tuple_t>::type type;

			std::get<0>(args) = std::move(convertion_t<type>::from(state, index));
		}

		static void parse(state_t &state, std::int32_t index, std::tuple<> &)
		{
		}
	};

	template < std::uint32_t N >
	struct method_caller_t;

	template < >
	struct method_caller_t<0>
	{
		template < typename T, typename R >
		static R call(T *obj, R(T::*handler)(), const std::tuple<> &args)
		{
			return (obj->*handler)();
		}
	};

	template < >
	struct method_caller_t<1>
	{
		template < typename T, typename R, typename ...Args >
		static R call(T *obj, R(T::*handler)(Args...), const std::tuple<Args...> &args)
		{
			return (obj->*handler)(std::get<0>(args));
		}
	};

	template < >
	struct method_caller_t<2>
	{
		template < typename T, typename R, typename ...Args >
		static R call(T *obj, R(T::*handler)(Args...), const std::tuple<Args...> &args)
		{
			return (obj->*handler)(std::get<0>(args),
								   std::get<1>(args));
		}
	};

	template < >
	struct method_caller_t<3>
	{
		template < typename T, typename R, typename ...Args >
		static R call(T *obj, R(T::*handler)(Args...), const std::tuple<Args...> &args)
		{
			return (obj->*handler)(std::get<0>(args),
								   std::get<1>(args),
								   std::get<2>(args));
		}
	};

	template < >
	struct method_caller_t<4>
	{
		template < typename T, typename R, typename ...Args >
		static R call(T *obj, R(T::*handler)(Args...), const std::tuple<Args...> &args)
		{
			return (obj->*handler)(std::get<0>(args),
								   std::get<1>(args),
								   std::get<2>(args),
								   std::get<3>(args));
		}
	};

	template < >
	struct method_caller_t<5>
	{
		template < typename T, typename R, typename ...Args >
		static R call(T *obj, R(T::*handler)(Args...), const std::tuple<Args...> &args)
		{
			return (obj->*handler)(std::get<0>(args),
								   std::get<1>(args),
								   std::get<2>(args),
								   std::get<3>(args),
								   std::get<4>(args));
		}
	};

	template < >
	struct method_caller_t<6>
	{
		template < typename T, typename R, typename ...Args >
		static R call(T *obj, R(T::*handler)(Args...), const std::tuple<Args...> &args)
		{
			return (obj->*handler)(std::get<0>(args),
								   std::get<1>(args),
								   std::get<2>(args),
								   std::get<3>(args),
								   std::get<4>(args),
								   std::get<5>(args));
		}
	};


	template < std::uint32_t N >
	struct constructor_caller_t;

	template < >
	struct constructor_caller_t<0>
	{
		template < typename T >
		static void call(T *obj, const std::tuple<> &args)
		{
			::new (obj) T();
		}
	};

	template < >
	struct constructor_caller_t<1>
	{
		template < typename T, typename ...Args >
		static void call(T *obj, const std::tuple<Args...> &args)
		{
			::new (obj) T(std::get<0>(args));
		}
	};

	template < >
	struct constructor_caller_t<2>
	{
		template < typename T, typename ...Args >
		static void call(T *obj, const std::tuple<Args...> &args)
		{
			::new (obj) T(std::get<0>(args),
						 std::get<1>(args));
		}
	};

	template < >
	struct constructor_caller_t<3>
	{
		template < typename T, typename ...Args >
		static void call(T *obj, const std::tuple<Args...> &args)
		{
			::new (obj) T(std::get<0>(args), 
						  std::get<1>(args),
						  std::get<2>(args));
		}
	};

	template < >
	struct constructor_caller_t<4>
	{
		template < typename T, typename ...Args >
		static void call(T *obj, const std::tuple<Args...> &args)
		{
			::new (obj) T(std::get<0>(args),
						  std::get<1>(args),
						  std::get<2>(args),
						  std::get<3>(args));
		}
	};


	template < typename ...Args >
	void parse_args(state_t &state, std::int32_t index, std::tuple<Args...> &args)
	{
		typedef std::tuple<Args...> tuple_t;
		enum
		{
			TUPLE_SIZE = std::tuple_size<tuple_t>::value == 0 ? 0 : std::tuple_size<tuple_t>::value - 1
		};

		args_parser_t<TUPLE_SIZE>::parse(state, index, args);
	}

	template < typename T, typename R, typename ...Args >
	struct call_method_t
	{
		static std::int32_t call(state_t &state, T *obj, R(T::*handler)(Args...), const std::tuple<Args...> &args)
		{
			typedef std::tuple<Args...> tuple_t;
			enum
			{
				TUPLE_SIZE = std::tuple_size<tuple_t>::value == 0 ? 0 : std::tuple_size<tuple_t>::value - 1
			};

			R ret = method_caller_t<TUPLE_SIZE>::call(obj, handler, args);

			return convertion_t<R>::to(state, ret);
		}
	};

	template < typename T, typename ...Args >
	struct call_method_t<T, void, Args...>
	{
		static std::int32_t call(state_t &state, T *obj, void (T::*handler)(Args...), const std::tuple<Args...> &args)
		{
			typedef std::tuple<Args...> tuple_t;
			method_caller_t<std::tuple_size<tuple_t>::value>::call(obj, handler, args);

			return 0;
		}
	};


	template < std::uint32_t N, typename T, typename R, typename ...Args >
	struct class_method_t
	{
		typedef R(T::*handler_t)(Args...);

		static const std::uint32_t value = N;
		static std::string handler_name_;
		static handler_t handler_;

		class_method_t(const std::string &handler_name, handler_t handler)
		{
			handler_name_ = handler_name;
			handler_ = handler;
		}

		static int on_handler(lua_State *lua_state)
		{
			state_t state = lua_state;
			T *val = static_cast<T *>(::luaL_checkudata(state, 1, class_name_t<T>::name_.c_str()));
			assert(val != nullptr);
			if( !val )
				throw parameter_error_t(state, "class method on handler error");

			typedef std::tuple<Args...> tuple_t;
			tuple_t args;

			check_stack<std::tuple_size<tuple_t>::value + 1>(state, has_special_type_t<state_t, tuple_t>::type());
			parse_args(state, std::tuple_size<tuple_t>::value + 1, args);

			return call_method_t<T, R, Args...>::call(state, val, handler_, args);
		}
	};

	template < std::uint32_t N, typename T, typename R, typename ...Args >
	std::string class_method_t<N, T, R, Args...>::handler_name_;

	template < std::uint32_t N, typename T, typename R, typename ...Args >
	typename class_method_t<N, T, R, Args...>::handler_t class_method_t<N, T, R, Args...>::handler_;


	template < typename T, typename ...Args >
	struct constructor_t
	{
		typedef std::tuple<Args...> tuple_t;


		static std::int32_t on_handler(lua_State *lua_state)
		{
			state_t state = lua_state;
			T * val = static_cast<T *>(::lua_newuserdata(state, sizeof(T)));
			assert(val != nullptr);
			if( !val )
				throw parameter_error_t(state, "lua_checkudata error:");

			luaL_getmetatable(state, details::class_name_t<T>::name_.c_str());
			::lua_setmetatable(state, -2);

			tuple_t args;
			check_stack<std::tuple_size<tuple_t>::value + 1>(state, std::false_type());
			parse_args(state, std::tuple_size<tuple_t>::value + 1, args);

			constructor_caller_t<std::tuple_size<tuple_t>::value>::call(val, args);

			return 1;
		}
	};


	template < typename T >
	struct destructor_t
	{
		static std::int32_t on_handler(lua_State *lua_state)
		{
			state_t state = lua_state;

			T *val = static_cast<T *>(::luaL_checkudata(state, 1, details::class_name_t<T>::name_.c_str()));
			assert(val != nullptr);
			if( !val )
				throw parameter_error_t(state, "lua_checkudata error:");

			val->~T();

			return 0;
		}
	};
}
}

#endif