#pragma once

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
//#include "luasocket.h"
#include "tolua++.h"
};

#include <string>
#include <vector>
#include <map>


namespace script
{
	struct LuaBuffer
	{
		void* buffer;
		int size;
		LuaBuffer(void* b,int s){
			buffer = b;
			size = s;
		}
	};
	class LuaModule
	{
	private:
		LuaModule( lua_State* state );

	public:
		
		~LuaModule( void );

		static LuaModule* create(lua_State* state = NULL);

		static void destroy(LuaModule* mod);

		lua_State* getLuaState(void) const {return d_state;}

		//----------------------------------------------------------------------------//
		void executeScriptFile(const std::string& filename);

		//----------------------------------------------------------------------------//
		template< typename ... Args >
		int executeScriptGlobal(const std::string& function_name, Args&&... args )
		{
			lua_getglobal(d_state, function_name.c_str());
			int argcnt = sizeof...(args);
			putStack(std::forward<Args>(args)...);
			// call it
			//int top = lua_gettop(d_state);
			int error = lua_pcall(d_state, argcnt, 0,0);
			if(error!=0){
				std::printf(lua_tostring(d_state,-1));
			}
			return error;
		}
		//----------------------------------------------------------------------------//
		template<typename T, typename... Tn>  
		void putStack(T&& t, Tn&&... args)
		{
			putStack(std::forward<T>(t));
			putStack(std::forward<Tn>(args)...);
		}
		
		template < typename T >
		void putStack(T &&t)
		{
			lua_pushlightuserdata(d_state,&t);
		}
		
		void putStack(LuaBuffer && buffer)
		{
			lua_pushlstring(d_state, (const char*)buffer.buffer, buffer.size);
		}
				
		void putStack(bool  value)
		{
			lua_pushboolean(d_state,value);
		}
		void putStack(char value)
		{
			lua_pushnumber(d_state,value);
		}
		void putStack(short value)
		{
			lua_pushnumber(d_state,value);
		}
		void putStack(int value)
		{
			lua_pushnumber(d_state,value);
		}
		
		void putStack(long value)
		{
			lua_pushnumber(d_state,value);
		}
		
		void putStack(float value)
		{
			lua_pushnumber(d_state,value);
		}
		void putStack(double value)
		{
			lua_pushnumber(d_state,value);
		}
		void putStack(const char* value)
		{
			lua_pushstring(d_state,value);
		}
		void putStack(char* value)
		{
			lua_pushstring(d_state,value);
		}
		void putStack(std::string value)
		{
			lua_pushstring(d_state,value.c_str());
		}

		template < typename T >
		void putStack(std::vector<T> value)
		{
			int size=value.size();
			lua_createtable(d_state,size,0);
			for(int i=0;i<size; i++)
			{
				putStack(value[i]);
				lua_rawseti(d_state,-2,i);
			}
		}

		
		template < typename K,typename T >
		void putStack(std::map<K,T> value)
		{
			lua_createtable(d_state,0,value.size());
			for(std::map<K,T>::iterator iter = value.begin();iter!=value.end();iter++)
			{
				putStack(iter->first);
				putStack(iter->second);
				lua_settable(d_state,-3);
			}
		}

		//template < typename T >
		//void putStack(T &&t, typename std::enable_if<
		//	std::is_pod<
		//		typename std::remove_all_extents<T>::type
		//	>::value>::type * = nullptr)
		//{
		//	const char* name = typeid(T).name();
		//	printf(name);
		//	printf(" pod \n\t");
		//}
		//template < typename T >
		//void putStack(T &&t,  typename std::enable_if<
		//	!std::is_pod<
		//		typename std::remove_all_extents<T>::type
		//	>::value>::type * = nullptr)
		//{
		//	const char* name = typeid(T).name();
		//	printf(name);
		//	printf(" class \n\t");
		//}
		/// 
	private:

		bool d_ownsState;

		lua_State* d_state;
	};
};

