#ifndef __LUA_REG_ERROR_HPP
#define __LUA_REG_ERROR_HPP

#include <stdexcept>
#include <cassert>
#include <sstream>

#include "config.hpp"
#include "state.hpp"
#include "error.hpp"

namespace luareg {


#define LUAREG_ERROR(expr) assert(expr); if( !(expr) ) throw fatal_error_t(state, "error: "#expr);


	template < typename StreamT >
	inline void stack_trace(const state_t &state, StreamT &os)
	{
		::lua_Debug entry = {0};
		int depth = 0; 

		while(::lua_getstack(state, depth, &entry))
		{
			int status = ::lua_getinfo(state, "Sln", &entry);
			assert(status);

			os << entry.short_src 
				<< "(" << entry.currentline << "): " 
				<< (entry.name ? entry.name : "?") 
				<< std::endl;

			depth++;
		}
	}



	template < typename StreamT >
	void parse_parameter(state_t &state, int idx, StreamT &os)
	{
		auto lua_type = ::lua_type(state, idx);
		os << "index[" << idx << "] type[" << ::lua_typename(state, lua_type) << "] : ";
		switch (lua_type)
		{
		case LUA_TNIL:
			os << "nil";
			break;
		case LUA_TBOOLEAN:
			os << std::boolalpha << (::lua_toboolean(state, idx) != 0) << std::noboolalpha;
			break;
		case LUA_TLIGHTUSERDATA:
			os << ::lua_touserdata(state, idx);
			break;
		case LUA_TNUMBER:
			os << ::lua_tonumber(state, idx);
			break;
		case LUA_TSTRING:
			os << lua_tostring(state, idx);
			break;
		case LUA_TTABLE:
			os << "";
			break;
		case LUA_TFUNCTION:
			os << ::lua_tocfunction(state, idx);
			break;
		case LUA_TUSERDATA:
			os << ::lua_touserdata(state, idx);
			break;
		case LUA_TTHREAD:
			os << "";
			break;
		default:
			os << "unknown type & value";
		}
	}

	class fatal_error_t
		: public std::exception
	{
		state_t state_;
		std::string msg_;

	public:
		fatal_error_t(::lua_State *state, std::string &&msg)
			: state_(state)
			, msg_(std::move(msg))
		{}

		fatal_error_t(const fatal_error_t &rhs)
			: state_(rhs.state_)
			, msg_(std::move(rhs.msg_))
		{}

		fatal_error_t &operator=(const fatal_error_t &rhs)
		{
			if( &rhs != this )
			{
				state_ = rhs.state_;
				msg_ = std::move(rhs.msg_);
			}

			return *this;
		}
	
	public:
		template < typename StreamT >
		void dump(StreamT &os) const
		{
			stack_trace(state_, os);
		}

		virtual const char* what() const
		{
			return msg_.c_str();
		}
	};

	class parameter_error_t
		: public std::exception
	{
		state_t state_;
		std::string msg_;

	public:
		parameter_error_t(::lua_State *state, std::string && msg)
			: state_(state)
			, msg_(std::move(msg))
		{
			std::ostringstream os;
			os << std::endl << "lua parameter:" << std::endl;

			int param_num = ::lua_gettop(state_);
			for (auto i = 1; i <= param_num; ++i)
			{
				parse_parameter(state_, i, os);
				os << std::endl;
			}

			msg_ += os.str();
		}

		parameter_error_t(::lua_State *state, std::string && msg, int index)
			: state_(state)
			, msg_(std::move(msg))
		{
			std::ostringstream os;
			os << " lua parameter--";

			parse_parameter(state_, index, os);

			msg_ += os.str();
		}

		parameter_error_t(const parameter_error_t &rhs)
			: state_(rhs.state_)
			, msg_(std::move(rhs.msg_))
		{}

		parameter_error_t &operator=(const parameter_error_t &rhs)
		{
			if (&rhs != this)
			{
				state_ = rhs.state_;
				msg_ = std::move(rhs.msg_);
			}

			return *this;
		}

	public:
		template < typename StreamT >
		void dump(StreamT &os) const
		{
			stack_trace(state_, os);
		}

		virtual const char* what() const
		{
			return msg_.c_str();
		}
	};
	

	struct error_t
	{
		static int handler(lua_State *state)
		{
			throw fatal_error_t(state, lua_tostring(state, -1));

			return 0;
		}
	};
}

#endif