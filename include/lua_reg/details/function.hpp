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


	
}}


#endif