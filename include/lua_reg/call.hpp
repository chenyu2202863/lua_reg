#ifndef __LUA_REG_CALL_HPP
#define __LUA_REG_CALL_HPP

#include "config.hpp"
#include "converter.hpp"
#include "error.hpp"
#include "reference.hpp"

namespace luareg {

	inline void push_value(state_t &state)
	{}

	template < typename T, typename... Args >
	void push_value(state_t &state, T && t, Args && ...args)
	{
		push_value(state, std::forward<T>( t ));
		push_value(state, std::forward<Args>( args )...);
	}

	template < typename T, typename... Args >
	void push_value(state_t &state, T && t)
	{
		typedef typename std::remove_cv <
			typename std::remove_reference<T>::type
		> ::type value_t;

		convertion_t<value_t>::to(state, t);
	}


	struct call_ret_t
	{
		state_t &state_;
		call_ret_t(state_t &state)
			: state_(state)
		{}

		template < typename T >
		operator T() const
		{
			return convertion_t<T>::from(state_, -1);
		}
	};


	template< typename ... Args >
	call_ret_t call(state_t &state, const char *function_name, Args&&... args )
	{
		assert(function_name && "function_name not be empty");

		::lua_getglobal(state, function_name);
		auto arg_cnt = sizeof...(args);

		int top = ::lua_gettop(state);

		push_value(state, std::forward<Args>(args)...);

		int error_index = 0;
		int base = ::lua_gettop(state) - arg_cnt;
		lua_pushcfunction(state, &error_t::handler);
		lua_insert(state, base);
		error_index = base;

		int error = ::lua_pcall(state, arg_cnt, 1, error_index);
		assert(error == 0);

		if( error_index != 0 )
			::lua_remove(state, error_index);

		int top_last = ::lua_gettop(state);
		
		return call_ret_t(state);
	}

	template< typename ... Args >
	call_ret_t call(state_t &state, const function_ref_t &func, Args &&... args)
	{
		assert(func.is_valid());
		func.get();

		auto arg_cnt = sizeof...( args );
		int top = ::lua_gettop(state);
		push_value(state, std::forward<Args>( args )...);

		int error_index = 0;
		int base = ::lua_gettop(state) - arg_cnt;
		lua_pushcfunction(state, &error_t::handler);
		lua_insert(state, base);
		error_index = base;

		int error = ::lua_pcall(state, arg_cnt, 1, error_index);
		assert(error == 0);

		if(error_index != 0)
			::lua_remove(state, error_index);

		int top_last = ::lua_gettop(state);

		return call_ret_t(state);
	}
}

#endif