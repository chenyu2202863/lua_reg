#ifndef __LUA_REG_ERROR_HPP
#define __LUA_REG_ERROR_HPP

#include <stdexcept>
#include <cassert>
#include <ostream>

#include "config.hpp"
#include "state.hpp"

namespace luareg {

	inline void stack_trace(const state_t &state, std::ostream &os)
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
		void dump(std::ostream &os) const
		{
			stack_trace(state_, os);
		}

		virtual const char* what() const
		{
			return msg_.c_str();
		}
	};
}

#endif