#ifndef __LUA_REG_EXECUTE_HPP
#define __LUA_REG_EXECUTE_HPP

#include "config.hpp"
#include "state.hpp"

namespace luareg {

	inline void execute(state_t &state, const std::string &file)
	{
		int top = ::lua_gettop(state);
		if( luaL_dofile(state, file.c_str()) )
		{
			std::string errMsg = lua_tostring(state,-1);
			::lua_settop(state,top);
			printf(errMsg.c_str());
		}
	}
}

#endif