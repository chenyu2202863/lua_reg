#ifndef __LUA_REG_DISPATCHER_DETAILS_HPP
#define __LUA_REG_DISPATCHER_DETAILS_HPP

#include "../config.hpp"
#include "traits.hpp"


namespace luareg { namespace details {


	template < typename R, typename T, typename ...Args >
	struct object_wrapper_t
	{
		using has_special_t = typename has_special_type_t<state_t, std::tuple<Args...>>::type;
		using has_check_t = std::integral_constant<bool, !has_special_t::value>;

		using return_t = R;
		using handler_t = R(T::*)(Args...);

		typedef std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...> params_t;
		typedef std::tuple<Args...> tuple_t;

		T *obj_;
		handler_t handler_;

		object_wrapper_t(T *obj, handler_t handler)
			: obj_(obj)
			, handler_(handler)
		{}

		R operator()(const Args &...param) const
		{
			return (*obj_.*handler_)(param...);
		}
	};

	template < typename R, typename T, typename ...Args >
	struct lambda_wrapper_t
	{
		using has_special_t = typename has_special_type_t<state_t, std::tuple<Args...>>::type;
		using has_check_t = std::integral_constant<bool, !has_special_t::value>;
		using return_t = R;
		using handler_t = R(T::*)(Args...);

		typedef std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...> params_t;
		typedef std::tuple<Args...> tuple_t;

		T *obj_;
		
		lambda_wrapper_t(T *obj)
			: obj_(obj)
		{}

		return_t operator()(const Args &...args) const
		{
			return (*obj_)(args...);
		}
	};


	template < typename R, typename ...Args >
	struct function_wrapper_t
	{
		using has_special_t = typename has_special_type_t<state_t, std::tuple<Args...>>::type;
		using has_check_t = std::integral_constant<bool, !has_special_t::value>;
		using return_t = R;
		using handler_t = R(*)(Args...);

		typedef std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...> params_t;
		typedef std::tuple<Args...> tuple_t;

		handler_t handler_;

		function_wrapper_t(handler_t handler)
			: handler_(handler)
		{}

		R operator()(const Args &...param) const
		{
			return (*handler_)(param...);
		}
	};

	template < typename T, typename ...Args >
	struct constructor_wrapper_t
	{
		using has_check_t = std::false_type;
		typedef void return_t;
		T *obj_;

		typedef std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...> params_t;
		typedef std::tuple<Args...> tuple_t;

		constructor_wrapper_t(T *obj)
			: obj_(obj)
		{}

		void operator()(const Args &...param) const
		{
			::new (obj_) T(param...);
		}
	};

	template < typename R, typename T, typename ...Args >
	object_wrapper_t<R, T, Args...> make_obj(T *obj, R(T::*handler)(Args...))
	{
		return object_wrapper_t<R, T, Args...>(obj, handler);
	}

	template < typename R, typename T, typename ...Args >
	lambda_wrapper_t<R, T, Args...> make_obj(T *obj)
	{
		return lambda_wrapper_t<R, T, Args...>(obj);
	}

	template < typename R, typename ...Args >
	function_wrapper_t<R, Args...> make_obj(R(*handler)(Args...))
	{
		return function_wrapper_t<R, Args...>(handler);
	}


	template < typename T, typename ...Args >
	constructor_wrapper_t<T, Args...> make_obj(T *obj, const std::tuple<Args...> &)
	{
		return constructor_wrapper_t<T, Args...>(obj);
	}


	template < std::uint32_t N >
	struct caller_t;

	template < >
	struct caller_t<0>
	{
		template < typename T >
		static typename T::return_t call(const T &val, std::tuple<> &args)
		{
			return val();
		}
	};

	template < >
	struct caller_t<1>
	{
		template < typename T, typename ...Args >
		static typename T::return_t call(const T &val, std::tuple<Args...> &args)
		{
			return val(std::get<0>(args));
		}
	};

	template < >
	struct caller_t<2>
	{
		template < typename T, typename ...Args >
		static typename T::return_t call(const T &val, std::tuple<Args...> &args)
		{
			return val(std::get<0>(args), std::get<1>(args));
		}
	};

	template < >
	struct caller_t<3>
	{
		template < typename T, typename ...Args >
		static typename T::return_t call(const T &val, std::tuple<Args...> &args)
		{
			return val(std::get<0>(args), std::get<1>(args), std::get<2>(args));
		}
	};

	template < >
	struct caller_t<4>
	{
		template < typename T, typename ...Args >
		static typename T::return_t call(const T &val, std::tuple<Args...> &args)
		{
			return val(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args));
		}
	};

	template < >
	struct caller_t<5>
	{
		template < typename T, typename ...Args >
		static typename T::return_t call(const T &val, std::tuple<Args...> &args)
		{
			return val(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args));
		}
	};

	template < >
	struct caller_t<6>
	{
		template < typename T, typename ...Args >
		static typename T::return_t call(const T &val, std::tuple<Args...> &args)
		{
			return val(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args), std::get<5>(args));
		}
	};


	template < std::uint32_t N >
	inline bool check_stack(state_t &state, std::int32_t offset, std::false_type)
	{
		return true;
	}

	template < std::uint32_t N >
	inline bool check_stack(state_t &state, std::int32_t offset, std::true_type)
	{
		int n = ::lua_gettop(state) - offset;
		if( n != N )
		{
			dump_parameter(state, std::cerr);
			
			parameter_error_t error(state, "expect parameter count error ");
			error.dump(std::cerr);
			assert(!"lua stack parameter error");

			throw error;

			return false;
		}

		return true;
	}


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



	template < typename T >
	typename T::return_t call_impl(state_t &state, const T &obj, std::int32_t offset)
	{
		typedef typename T::params_t args_t;
		typedef typename T::tuple_t tuple_t;
		typedef typename T::has_check_t has_check_t;

		assert(check_stack<details::parameter_count_t<tuple_t>::value>(state, offset, has_check_t()));
		
		enum
		{
			TUPLE_SIZE = std::tuple_size<args_t>::value == 0 ? 0 : std::tuple_size<args_t>::value - 1
		};
		args_t args;
		args_parser_t<TUPLE_SIZE>::parse(state, std::tuple_size<args_t>::value + offset, args);

		return caller_t<std::tuple_size<args_t>::value>::call(obj, args);
	}


	template < typename T, typename ...Args >
	inline void call(state_t &state, T *obj, const std::tuple<Args...> &args)
	{
		call_impl(state, make_obj(obj, args), 1);
	}


	template < typename R, typename T, typename ...Args >
	std::int32_t call(state_t &state, T *obj, R(T::*handler)(Args...), std::uint32_t offset = 0,
					  typename std::enable_if<!std::is_same<R, void>::value>::type * = nullptr)
	{
		using result_t = typename std::remove_cv<R>::type;
		return convertion_t<result_t>::to(state, call_impl(state, make_obj(obj, handler), offset));
	}

	template < typename R, typename T, typename ...Args >
	std::int32_t call(state_t &state, T *obj, R(T::*handler)(Args...), std::uint32_t offset = 0,
					  typename std::enable_if<std::is_same<R, void>::value>::type * = nullptr)
	{
		call_impl(state, make_obj(obj, handler), offset);

		return 0;
	}

	template < typename R, typename ...Args >
	std::int32_t call(state_t &state, R(*handler)(Args...),
					  typename std::enable_if<!std::is_same<R, void>::value>::type * = nullptr)
	{
		using result_t = typename std::remove_cv<R>::type;
		return convertion_t<result_t>::to(state, call_impl(state, make_obj(handler), 0));
	}

	template < typename R, typename ...Args >
	std::int32_t call(state_t &state, R(*handler)(Args...),
					  typename std::enable_if<std::is_same<R, void>::value>::type * = nullptr)
	{
		call_impl(state, make_obj(handler), 0);

		return 0;
	}


	template < typename HandlerT, typename R, typename ...Args >
	std::uint32_t call(state_t &state, HandlerT *handler,
		typename std::enable_if<!std::is_same<R, void>::value>::type * = nullptr)
	{
		using result_t = typename std::remove_cv<R>::type;
		return convertion_t<result_t>::to(state, call_impl(state, make_obj<R, HandlerT, Args...>(handler), 0));
	}

	template < typename HandlerT, typename R, typename ...Args >
	std::uint32_t call(state_t &state, HandlerT *handler,
		typename std::enable_if<std::is_same<R, void>::value>::type * = nullptr)
	{
		call_impl(state, make_obj<R, HandlerT, Args...>(handler), 0);
		return 0;
	}
}
}

#endif