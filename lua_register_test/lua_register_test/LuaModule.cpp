#include <stdio.h>
#include <stdarg.h>
#include "LuaModule.h"

extern void   OpenLuaModule(lua_State *pLua);

namespace script
{
	//----------------------------------------------------------------------------//
	LuaModule* LuaModule::create(lua_State* state)
	{
		LuaModule* lua = new LuaModule(state);
		//OpenLuaModule(lua->getLuaState());
		return lua;
	}

	//----------------------------------------------------------------------------//
	void LuaModule::destroy(LuaModule* mod)
	{
		if(mod!=NULL)
			delete mod;
	}

	LuaModule::LuaModule( lua_State* state ):
		d_ownsState(state == 0),
		d_state(state)
	{
		// initialise and create a lua_State if one was not provided
		if (!d_state)
		{
			static const luaL_Reg lualibs[] = {
				{"", luaopen_base},
				{LUA_LOADLIBNAME, luaopen_package},
				{LUA_TABLIBNAME, luaopen_table},
				{LUA_IOLIBNAME, luaopen_io},
				{LUA_OSLIBNAME, luaopen_os},
				{LUA_STRLIBNAME, luaopen_string},
				{LUA_MATHLIBNAME, luaopen_math},
#if defined(DEBUG) || defined (_DEBUG)
				{LUA_DBLIBNAME, luaopen_debug},
#endif
				{0, 0}
			};

			// create a lua state
			d_ownsState = true;
			d_state = lua_open();

			//luaopen_socket_core(d_state);

			// init all standard libraries
			const luaL_Reg *lib = lualibs;
			for (; lib->func; lib++)
			{
				lua_pushcfunction(d_state, lib->func);
				lua_pushstring(d_state, lib->name);
				lua_call(d_state, 1, 0);
			}
		}
	}


	LuaModule::~LuaModule(void)
	{
		if (d_state)
		{
			if (d_ownsState)
				lua_close( d_state );
		}
	}
	
	//----------------------------------------------------------------------------//
	void LuaModule::executeScriptFile(const std::string& filename)
	{
		int top = lua_gettop(d_state);
		if(luaL_dofile(d_state, filename.c_str()))
		{
			std::string errMsg = lua_tostring(d_state,-1);
			lua_settop(d_state,top);
			printf(errMsg.c_str());
#ifdef LUADEBUG
			LOG("Lua script error :%s:%s",filename.c_str(),errMsg.c_str());
#endif
		}
	}
}