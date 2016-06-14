#ifndef __LUA_REG_CLASS_HPP
#define __LUA_REG_CLASS_HPP

#include "config.hpp"
#include "state.hpp"
#include "error.hpp"
#include "converter.hpp"

#include "details/dispatcher.hpp"
#include "details/function.hpp"

namespace luareg {

	
	template < typename ...Args >
	struct constructor_t
	{};

	struct destructor_t
	{};

	
	template < typename ...Args >
	inline constructor_t<Args...> constructor()
	{
		return constructor_t<Args...>();
	}

	inline destructor_t destructor()
	{
		return destructor_t();
	}
	


	template < typename T >
	struct class_name_t
	{
		static std::string name_;
	};

	template < typename T >
	std::string class_name_t<T>::name_;


	template < typename T >
	struct class_t
	{
		state_t &state_;
		
		explicit class_t(state_t &state, const char *name)
			: state_(state)
		{
			assert(class_name_t<T>::name_.empty());
			class_name_t<T>::name_ = name;

			if( ::luaL_newmetatable(state_, name) == 0 )
				throw fatal_error_t(state_, "register class has fatal error");

			//add_constructor<T>();
			add_destructor();

			::lua_pushvalue(state_, -1);
			::lua_setfield(state_, -1, "__index");
		}

		template < typename R, typename U, typename ...Args >
		class_t &operator<<(const details::class_function_t<R, U, Args...> &func)
		{
			typedef typename details::class_function_t<R, U, Args...>::function_t function_t;

			auto lambda = [](lua_State *l)->int
			{
				state_t state(l);

				U *obj = static_cast<U *>(::luaL_checkudata(state, 1, class_name_t<T>::name_.c_str()));
				assert(obj != nullptr);
				if( !obj )
					throw parameter_error_t(state, "class method on handler error");

				auto function = static_cast<function_t *>(::lua_touserdata(state, lua_upvalueindex(1)));

				return details::call(state, obj, *function,1);
			};

			::lua_pushlightuserdata(state_, (void *)&func.function_);
			::lua_pushcclosure(state_, lambda, 1);
			::lua_setfield(state_, -2, func.name_);

			return *this;
		}

		template < typename ...Args >
		class_t &operator<<(const constructor_t<Args...> &method)
		{
			typedef std::tuple<Args...> tuple_t;

			lua_pushcfunction(state_, [](lua_State *l)->int
			{
				state_t state(l);
				T * val = static_cast<T *>(::lua_newuserdata(state, sizeof(T)));
				assert(val != nullptr);
				if( !val )
					throw parameter_error_t(state, "lua_checkudata error:");

				luaL_getmetatable(state, class_name_t<T>::name_.c_str());
				::lua_setmetatable(state, -2);

				details::call(state, val, tuple_t());

				return 1;
			});
			::lua_setfield(state_, -2, "new");

			return *this;
		}

		class_t &operator<<(const destructor_t &method)
		{
			lua_pushcfunction(state_, [](lua_State *l)->int
			{
				state_t state(l);

				T *val = static_cast<T *>(::luaL_checkudata(state, 1, class_name_t<T>::name_.c_str()));
				assert(val != nullptr);
				if( !val )
					throw parameter_error_t(state, "lua_checkudata error:");

				val->~T();
				return 0;
			});
			::lua_setfield(state_, -2, "__gc");

			return *this;
		}

	private:
		/*template < typename U >
		void add_constructor(typename std::enable_if<std::has_default_constructor<U>::value>::type * = nullptr)
		{
			*this << constructor<>();
		}

		template < typename U >
		void add_constructor(typename std::enable_if<!std::has_default_constructor<U>::value>::type * = nullptr)
		{
		}*/

		void add_destructor()
		{
			*this << destructor();
		}
	};

}

#endif