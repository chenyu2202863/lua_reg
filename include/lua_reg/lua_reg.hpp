#ifndef __LUA_REG_LUA_REG_HPP
#define __LUA_REG_LUA_REG_HPP

#include "error.hpp"
#include "state.hpp"
#include "call.hpp"
#include "execute.hpp"
#include "reference.hpp"
#include "iterator.hpp"
#include "class.hpp"
#include "module.hpp"


namespace luareg {

	template < typename R, typename ...Args >
	inline details::free_function_t<R, Args...> def(const char *name, R(*func)(Args...))
	{
		return details::free_function_t<R, Args...>(name, func);
	}

	template < typename HandlerT >
	inline auto def(const char *name, HandlerT &&handler)->
		typename details::lambda_function_t<HandlerT, typename details::function_traits_t<HandlerT>::result_type, typename details::function_traits_t<HandlerT>::args_type>::class_function_impl_t
	{
		typedef typename details::function_traits_t<HandlerT>::args_type	tuple_t;
		typedef typename details::function_traits_t<HandlerT>::result_type	R;

		typedef details::lambda_function_t<HandlerT, R, tuple_t> lambda_t;
		return details::lambda_t(name, std::forward<HandlerT>(handler));
	}

	template < typename R, typename T, typename ...Args >
	inline details::class_function_t<R, T, Args...> def(const char *name, T *obj, R(T::*func)(Args...))
	{
		return details::class_function_t<R, T, Args...>(name, obj, func);
	}

	template < typename R, typename T, typename ...Args >
	inline details::class_function_t<R, T, Args...> def(const char *name, T *obj, R(T::*func)(Args...) const)
	{
		return details::class_function_t<R, T, Args...>(name, obj, (R(T::*)(Args...))func);
	}

	template < typename R, typename T, typename ...Args >
	inline details::class_function_t<R, T, Args...> def(const char *name, T *obj, R(T::*func)(Args...) volatile)
	{
		return details::class_function_t<R, T, Args...>(name, obj, (R(T::*)(Args...))func);
	}

	template < typename R, typename T, typename ...Args >
	inline details::class_function_t<R, T, Args...> def(const char *name, R(T::*func)(Args...))
	{
		return details::class_function_t<R, T, Args...>(name, nullptr, func);
	}

	template < typename R, typename T, typename ...Args >
	inline details::class_function_t<R, T, Args...> def(const char *name, R(T::*func)(Args...) const)
	{
		return details::class_function_t<R, T, Args...>(name, nullptr, (R(T::*)(Args...))func);
	}

	template < typename R, typename T, typename ...Args >
	inline details::class_function_t<R, T, Args...> def(const char *name, R(T::*func)(Args...) volatile)
	{
		return details::class_function_t<R, T, Args...>(name, nullptr, (R(T::*)(Args...))func);
	}
}
#endif