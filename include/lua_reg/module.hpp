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
			if( name != nullptr )
			{
				::lua_getglobal(state_, name_);
				
				if( lua_isnil(state_, -1) ) // has same name table
				{
					::lua_pop(state_, 1);
					::lua_newtable(state_);
					::lua_setglobal(state_, name_);
					::lua_getglobal(state_, name_);
				}
			}
			else
			{
				::lua_pushglobaltable(state_);
			}
		}

		~module_t()
		{
			::lua_pop(state_, 1);
		}

		module_t(module_t &&rhs)
			: state_(rhs.state_)
			, name_(rhs.name_)
		{}

		module_t(const module_t &) = delete;
		module_t &operator=(const module_t &) = delete;

	public:
		template < typename R, typename ...Args >
		module_t &operator<<(const details::free_function_t<R, Args...> &func)
		{
			auto lambda = [](lua_State *l)->int
			{
				state_t state(l);
				typedef typename details::free_function_t<R, Args...>::function_t function_t;
				auto function = (function_t)(::lua_touserdata(state, lua_upvalueindex(1)));
				
				return details::call(state, function);
			};

			::lua_pushlightuserdata(state_, (void *)func.function_);
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
		
				function_t *func = reinterpret_cast<function_t *>(::lua_touserdata(state, lua_upvalueindex(1)));
				T *obj = static_cast<T *>(::lua_touserdata(state, lua_upvalueindex(2)));
				
				return details::call(state, obj, *func, 0);
			};

			void *p = ::lua_newuserdata(state_, sizeof(function_t));
			std::memcpy(p, &func.function_, sizeof(function_t));
			::lua_pushlightuserdata(state_, func.obj_);
			::lua_pushcclosure(state_, lambda, 2);
			::lua_setfield(state_, -2, func.name_);

			return *this;
		}

		template < typename HandlerT, typename R, typename ...Args >
		module_t &operator<<(const details::lambda_function_t<HandlerT, R, std::tuple<Args...>> &func)
		{
			using function_t = typename details::lambda_function_t<HandlerT, R, std::tuple<Args...>>::function_t;

			auto lambda = [](lua_State *l)->int
			{
				state_t state(l);

				HandlerT *obj = static_cast<HandlerT *>(::lua_touserdata(state, lua_upvalueindex(1)));
		
				return details::call<HandlerT, R, Args...>(state, obj);
			};

			::lua_pushlightuserdata(state_, (void *)(&func.obj_));
			::lua_pushcclosure(state_, lambda, 1);
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

		module_t &operator()(const char *name)
		{
			::lua_getfield(state_, -1, name);

			if( !lua_istable(state_, -1) )
			{
				::lua_pop(state_, 1);
				::lua_newtable(state_);
				::lua_setfield(state_, -2, name);

				::lua_getfield(state_, -1, name);
			}

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