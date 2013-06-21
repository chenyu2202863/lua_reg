#ifndef __LUA_REG_STATE_HPP
#define __LUA_REG_STATE_HPP

#include "config.hpp"

namespace luareg {

	struct state_t
	{
		bool is_owner_;
		lua_State *state_;

		state_t()
			: is_owner_(true)
			, state_(nullptr)
		{
			open();
		}

		state_t(lua_State *state)
			: is_owner_(false)
			, state_(state)
		{

		}

		~state_t()
		{
			close();
		}

	public:
		lua_State *get()
		{
			return state_;
		}

		operator lua_State*()
		{
			return state_;
		}

		operator lua_State*() const
		{
			return state_;
		}

		void open()
		{
			assert(state_ == nullptr);

			static const ::luaL_Reg lualibs[] = 
			{
				{"",				::luaopen_base},
				{LUA_LOADLIBNAME,	::luaopen_package},
				{LUA_TABLIBNAME,	::luaopen_table},
				{LUA_IOLIBNAME,		::luaopen_io},
				{LUA_OSLIBNAME,		::luaopen_os},
				{LUA_STRLIBNAME,	::luaopen_string},
				{LUA_MATHLIBNAME,	::luaopen_math},
#if defined(DEBUG) || defined (_DEBUG)
				{LUA_DBLIBNAME,		::luaopen_debug}
#endif
			};

			state_ = ::lua_open();
			for(auto i = 0; i != _countof(lualibs); ++i)
			{
				::lua_pushcfunction(state_, lualibs[i].func);
				::lua_pushstring(state_, lualibs[i].name);
				::lua_call(state_, 1, 0);
			}

		}

		void close()
		{
			if( is_owner_ )
				::lua_close(state_);
		}
	};
}

#endif