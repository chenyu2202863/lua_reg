#ifndef __LUA_REG_CLASS_HPP
#define __LUA_REG_CLASS_HPP

#include "config.hpp"
#include "state.hpp"
#include "error.hpp"
#include "converter.hpp"

#include "details/def.hpp"
#include "details/dispatcher.hpp"

namespace luareg {


	template < std::uint32_t N, typename T, typename R, typename ...Args >
	details::class_method_t<N, T, R, Args...> def(const std::string &handler_name, R(T::*handler)(Args...))
	{
		return details::class_method_t<N, T, R, Args...>(handler_name, handler);
	}


	template < typename ...Args >
	struct constructor_t
	{
		constructor_t()
		{}
	};

	struct destructor_t
	{
		destructor_t()
		{}
	};

	
	template < typename ...Args >
	constructor_t<Args...> constructor()
	{
		return constructor_t<Args...>();
	}

	inline destructor_t destructor()
	{
		return destructor_t();
	}
	

	template < typename T >
	struct convertion_t<T *, typename std::enable_if<T::is_userdata>::type>
	{
		static std::int32_t to(state_t &state, T *val)
		{
			assert(val != nullptr);
			::lua_pushlightuserdata(state, val);

			luaL_getmetatable(state, details::class_name_t<T>::name_.c_str());
			::lua_setmetatable(state, -2);

			return 1;
		}
	};

	template < typename T >
	struct class_t
	{
		state_t &state_;
		std::map<const char *, lua_CFunction> lua_regs_;

		explicit class_t(state_t &state, const char *name)
			: state_(state)
		{
			details::class_name_t<T>::name_ = name;

			if( ::luaL_newmetatable(state_, name) == 0 )
				throw fatal_error_t(state_, "register class has fatal error");

			add_constructor<T>();
			add_destructor();
		}
		~class_t()
		{
			assert(lua_regs_.find("new") != lua_regs_.cend());

			std::vector<luaL_Reg> regs;
			std::for_each(lua_regs_.cbegin(), lua_regs_.cend(), 
						  [&regs](const std::pair<const char *, lua_CFunction> &val)
			{
				regs.push_back(luaL_Reg{val.first, val.second});
			});
			regs.push_back(luaL_Reg{ nullptr, nullptr });

			::luaL_register(state_, nullptr, regs.data());
			::lua_pushvalue(state_, -1);
			::lua_setfield(state_, -1, "__index");
			::lua_setglobal(state_, details::class_name_t<T>::name_.c_str());
		}

		template < std::uint32_t N, typename R, typename ...Args >
		class_t<T> &operator<<(const details::class_method_t<N, T, R, Args...> &method)
		{
			typedef details::class_method_t<N, T, R, Args...> method_t;
			lua_regs_[method.handler_name_.c_str()] = &method_t::on_handler;

			return *this;
		}

		template < typename ...Args >
		class_t<T> &operator<<(const constructor_t<Args...> &method)
		{
			typedef details::constructor_t<T, Args...> constructor_t;
			lua_regs_["new"] = &constructor_t::on_handler;

			return *this;
		}

		class_t<T> &operator<<(const destructor_t &method)
		{
			typedef details::destructor_t<T> destructor_t;
			lua_regs_["__gc"] = &destructor_t::on_handler;

			return *this;
		}

	private:
		template < typename U >
		void add_constructor(typename std::enable_if<std::has_default_constructor<U>::value>::type * = nullptr)
		{
			lua_regs_.insert({"new", &class_t<U>::construct<0>});
		}

		template < typename U >
		void add_constructor(typename std::enable_if<!std::has_default_constructor<U>::value>::type * = nullptr)
		{
		}

		void add_destructor()
		{
			lua_regs_.insert({"__gc", &class_t<T>::destroy});
		}

		template < std::uint32_t N >
		static int construct(lua_State *state)
		{
			static_assert(std::has_default_constructor<T>::value, "T must be a default constructor");

			T *udata = static_cast<T *>(::lua_newuserdata(state, sizeof(T)));
			assert(udata);
			new (udata) T();

			luaL_getmetatable(state, details::class_name_t<T>::name_.c_str());
			::lua_setmetatable(state, -2);

			return 1;
		}


		static int destroy(lua_State *state)
		{
			T *foo = static_cast<T *>(::luaL_checkudata(state, 1, details::class_name_t<T>::name_.c_str()));
			assert(foo != nullptr);
			if( !foo )
				throw parameter_error_t(state, "lua_checkudata error:");
			
			foo->~T();
			return 0;
		}
	};


	struct class_functor_t
	{
		std::uint32_t index_;
		std::map<std::uint32_t, details::handler_base_ptr> handlers_;

		template < typename T, typename R, typename ...Args >
		void add_handler(R(T::*func)(Args && ...))
		{
			typedef R(T::*handler_t)(Args && ...);

			typedef std::tuple<Args...> tuple_t;
			typedef handler_impl_t<handler_t, R, tuple_t>
		}
	};
}

#endif