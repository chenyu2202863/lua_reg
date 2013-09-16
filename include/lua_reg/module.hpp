#ifndef __LUA_REG_MODULE_HPP
#define __LUA_REG_MODULE_HPP

#include "config.hpp"

#include "details/function.hpp"
#include "class.hpp"

namespace luareg {

	struct state_t;


	class module_t
	{
		state_t &state_;
		const char *name_;
		
	public:
		module_t(state_t &state, const char *name)
			: state_(state)
			, name_(name)
		{
			if( name )
			{
				::lua_pushstring(state_, name_);
				::lua_gettable(state_, LUA_GLOBALSINDEX);

				if( lua_isnil(state_, -1) ) // has same name table
				{
					::lua_pop(state_, 1);
					::lua_newtable(state);
					::lua_pushstring(state, name_);
					::lua_pushvalue(state, -2);

					::lua_settable(state, LUA_GLOBALSINDEX);
				}
			}
			else
			{
				::lua_pushvalue(state_, LUA_GLOBALSINDEX);
			}
		}
		~module_t()
		{
			lua_pop(state_, 1);
		}

	public:
		template < typename R, typename ...Args >
		module_t &operator<<(const details::free_function_t<R, Args...> &func)
		{
			auto lambda = [](lua_State *l)->int
			{
				state_t state(l);
				typedef typename details::free_function_t<R, Args...>::function_t function_t;
				auto func = static_cast<function_t>(::lua_touserdata(state, lua_upvalueindex(1)));
				
				return details::call(state, func);
			};

			::lua_pushlightuserdata(state_, func.function_);
			::lua_pushcclosure(state_, lambda, 1);
			::lua_setfield(state_, -2, func.name_);

			return *this;
		}

		template < typename R, typename T, typename ...Args >
		module_t &operator<<(const details::class_function_t<R, T, Args...> &func)
		{
			typedef typename details::class_function_t<R, T, Args...>::function_t function_t;

			auto lambda = [](lua_State *l)->int
			{
				state_t state(l);
		
				function_t *func = static_cast<function_t *>(::lua_touserdata(state, lua_upvalueindex(1)));
				T *obj = static_cast<T *>(::lua_touserdata(state, lua_upvalueindex(2)));
				
				return details::call(state, obj, *func);
			};

			::lua_pushlightuserdata(state_, (void *)(&func.function_));
			::lua_pushlightuserdata(state_, func.obj_);
			::lua_pushcclosure(state_, lambda, 2);
			::lua_setfield(state_, -2, func.name_);

			return *this;
		}


		template < typename T >
		module_t &operator[](const class_t<T> &class_val)
		{
			::lua_pushvalue(state_, 1);
			::lua_setfield(state_, -2, class_name_t<T>::name_.c_str());
			::lua_pop(state_, 1);

			return *this;
		}
	};

	inline module_t module(state_t &state, const char *name = nullptr)
	{
		if( name )
			assert(std::strlen(name) != 0);
		return module_t(state, name);
	}


	
}


#endif