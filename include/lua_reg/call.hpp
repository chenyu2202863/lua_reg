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
		const std::uint32_t stack_size_;

		call_ret_t(state_t &state)
			: state_(state)
			, stack_size_(0)
		{}

		call_ret_t(state_t &state, std::uint32_t stack_size)
			: state_(state)
			, stack_size_(stack_size)
		{}

		~call_ret_t()
		{
			lua_pop(state_, static_cast<std::int32_t>(stack_size_));
		}

		template < typename T >
		operator T() const
		{
			return convertion_t<T>::from(state_, -1);
		}

		template < typename ...Args >
		operator std::tuple<Args...>() const
		{
			assert(stack_size_ != 0);
			assert(stack_size_ == std::tuple_size<std::tuple<Args...>>::value);

			state_t state = state_;
			return convertion_t<std::tuple<Args...>>::from(state_, -1);
		}
	};


	template< typename ... Args >
	call_ret_t call(state_t &state, const char *function_name, Args&&... args )
	{
		assert(function_name && "function_name not be empty");
		
		int top_beg = ::lua_gettop(state);

		::lua_getglobal(state, function_name);
		auto arg_cnt = sizeof...(args);

		push_value(state, std::forward<Args>(args)...);

		int base = ::lua_gettop(state) - arg_cnt;
		::lua_pushcfunction(state, &error_t::handler);
		::lua_insert(state, base);
		int error_index = base;

		int error = ::lua_pcall(state, arg_cnt, LUA_MULTRET, error_index);
		assert(error == 0);

		if( error_index != 0 )
			::lua_remove(state, error_index);

		int top_last = ::lua_gettop(state);

		return call_ret_t(state, top_last - top_beg);
	}

	template< typename ... Args >
	call_ret_t call(state_t &state, const function_ref_t &func, Args &&... args)
	{
		assert(func.is_valid());

		int top_beg = ::lua_gettop(state);

		func.get();

		auto arg_cnt = sizeof...( args );
		push_value(state, std::forward<Args>( args )...);

		int error_index = 0;
		int base = ::lua_gettop(state) - arg_cnt;
		lua_pushcfunction(state, &error_t::handler);
		lua_insert(state, base);
		error_index = base;

		int error = ::lua_pcall(state, arg_cnt, LUA_MULTRET, error_index);
		assert(error == 0);

		if(error_index != 0)
			::lua_remove(state, error_index);

		int top_last = ::lua_gettop(state);

		return call_ret_t(state, top_last - top_beg);
	}
}

#endif