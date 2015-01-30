#ifndef __LUA_REG_EXECUTE_HPP
#define __LUA_REG_EXECUTE_HPP

#include "config.hpp"
#include "state.hpp"
#include "error.hpp"

namespace luareg {

	inline void execute(state_t &state, const char *file_name)
	{
		assert(file_name);
		int top = ::lua_gettop(state);

		if( ::luaL_loadfile(state, file_name) )
			throw fatal_error_t(state, ::lua_tostring(state, -1));

		int error_index = 0;
		int base = ::lua_gettop(state);
		
		std::string error_msg;
		error_handler(state, error_msg);

		::lua_insert(state, base);
		error_index = base;

		int error = ::lua_pcall(state, 0, LUA_MULTRET, error_index);
		if( error != 0 )
			throw fatal_error_t(state, error_msg, error);
	}
}

#endif