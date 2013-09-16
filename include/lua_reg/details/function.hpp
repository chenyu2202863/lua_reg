#ifndef __LUA_REG_FUNCTION_DETAILS_HPP
#define __LUA_REG_FUNCTION_DETAILS_HPP


namespace luareg { namespace details {

	

	template < typename R, typename ...Args >
	struct free_function_t
	{
		const char *name_;

		typedef R(*function_t)(Args...);
		function_t function_;

		free_function_t(const char *name, function_t func)
			: name_(name)
			, function_(func)
		{}
	};

	
	template < typename R, typename T, typename ...Args >
	struct class_function_t
	{
		const char *name_;

		typedef R(T::*function_t)(Args...);
		T *obj_;
		function_t function_;

		class_function_t(const char *name, T *obj, function_t func)
			: name_(name)
			, obj_(obj)
			, function_(func)
		{}
	};

	template < typename ...Args >
	struct lambda_function_t;

	template < typename HandlerT, typename R, typename ...Args >
	struct lambda_function_t<HandlerT, R, std::tuple<Args...>>
	{
		using function_t = R(*)(Args...);
		using this_t = lambda_function_t<HandlerT, R, std::tuple<Args...>>;
	
		const char *name_;
		HandlerT handler_;

		lambda_function_t(const char *name, HandlerT &&handler)
			: name_(name)
			, handler_(std::forward<HandlerT>(handler))
		{}

		R on_handler(Args...args)
		{
			return handler_(args...);
		}
	};


	template < typename R, typename ...Args >
	inline free_function_t<R, Args...> def(const char *name, R(*func)(Args...))
	{
		return free_function_t<R, Args...>(name, func);
	}

	template < typename HandlerT >
	inline auto def(const char *name, HandlerT &&handler)->
		typename lambda_function_t<HandlerT, typename details::function_traits_t<HandlerT>::result_type, typename details::function_traits_t<HandlerT>::args_type>::class_function_impl_t
	{
		typedef typename details::function_traits_t<HandlerT>::args_type	tuple_t;
		typedef typename details::function_traits_t<HandlerT>::result_type	R;

		typedef lambda_function_t<HandlerT, R, tuple_t> lambda_t;
		return lambda_t(name, std::forward<HandlerT>(handler));
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, T *obj, R(T::*func)(Args...))
	{
		return class_function_t<R, T, Args...>(name, obj, func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, T *obj, R(T::*func)(Args...) const)
	{
		return class_function_t<R, T, Args...>(name, obj, (R(T::*)(Args...))func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, T *obj, R(T::*func)(Args...) volatile)
	{
		return class_function_t<R, T, Args...>(name, obj, (R(T::*)(Args...))func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, R(T::*func)(Args...))
	{
		return class_function_t<R, T, Args...>(name, nullptr, func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, R(T::*func)(Args...) const)
	{
		return class_function_t<R, T, Args...>(name, nullptr, (R(T::*)(Args...))func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, R(T::*func)(Args...) volatile)
	{
		return class_function_t<R, T, Args...>(name, nullptr, (R(T::*)(Args...))func);
	}
}}


#endif