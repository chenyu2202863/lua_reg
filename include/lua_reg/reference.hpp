#ifndef __LUA_REFERENCE_HPP
#define __LUA_REFERENCE_HPP

#include "config.hpp"


namespace luareg {


	namespace detail  
	{
		struct function_check_t
		{
			static void check(lua_State *state, int ref)
			{
				lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
				assert(lua_isfunction(state, -1));
				lua_pop(state, 1);
			}
		};

		struct table_check_t
		{
			static void check(lua_State *state, int ref)
			{
				lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
				assert(lua_istable(state, -1));
				lua_pop(state, 1);
			}
		};

		struct string_check_t
		{
			static void check(lua_State *state, int ref)
			{
				lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
				assert(lua_isstring(state, -1));
				lua_pop(state, 1);
			}
		};
	}
	

	template < typename CheckT >
	class reference_t
		: CheckT
	{
		lua_State *state_;
		int ref_;

	public:
		reference_t(lua_State *state)
			: state_(state)
			, ref_(LUA_NOREF)
		{
			ref_ = luaL_ref(state_, LUA_REGISTRYINDEX);
		}
		
		~reference_t() 
		{
			if( ref_ != LUA_NOREF ) 
			{
				luaL_unref(state_, LUA_REGISTRYINDEX, ref_);
				ref_ = LUA_NOREF;
			}
		}

		reference_t(reference_t && rhs)
			: ref_(rhs.ref_)
			, state_(rhs.state_)
		{
			rhs.ref_ = LUA_NOREF;
			rhs.state_ = nullptr;
		}

		reference_t &operator=(reference_t &&rhs)
		{
			if(this != &rhs)
			{
				ref_ = rhs.ref_;
				state_ = rhs.state_;

				rhs.ref_ = LUA_NOREF;
				state_ = nullptr;
			}

			return *this;
		}

	private:
		reference_t(const reference_t &);
		reference_t &operator=(const reference_t &);


	public:
		bool is_valid() const
		{
			CheckT::check(state_, ref_);
			return ref_ != LUA_NOREF;
		}

		void get() const
		{
			assert(ref_ != LUA_NOREF);
			lua_rawgeti(state_, LUA_REGISTRYINDEX, ref_);
		}
	};

	typedef reference_t<detail::function_check_t>	function_ref_t;
	typedef reference_t<detail::table_check_t>		table_ref_t;
	typedef reference_t<detail::string_check_t>		string_ref_t;
}

#endif