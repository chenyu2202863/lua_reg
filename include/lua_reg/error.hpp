#ifndef __LUA_REG_ERROR_HPP
#define __LUA_REG_ERROR_HPP

#include <stdexcept>
#include <cassert>
#include <sstream>

#include "config.hpp"
#include "state.hpp"
#include "error.hpp"

namespace luareg {


	template < typename StreamT >
	inline void stack_trace(const state_t &state, StreamT &os)
	{
		::lua_Debug entry = {0};
		int depth = 0;

		while(::lua_getstack(state, depth, &entry))
		{
			if( depth >= 20 )
				break;

			int status = ::lua_getinfo(state, "Sln", &entry);
			assert(status);

			os << entry.short_src
			<< "(" << entry.currentline << "): "
			<< (entry.name ? entry.name : "?")
			<< std::endl;

			++depth;
		}
	}

	template < typename OStreamT >
	void traverse_table(state_t &state, int index, OStreamT &os)
	{
		::lua_pushnil(state);

		while( ::lua_next(state, index) )
		{
			::lua_pushvalue(state, -2);
	
			const char* key = ::lua_tostring(state, -1);
			const char* value = ::lua_tostring(state, -2);
	
			const char *key_msg = key != nullptr ? key : ::lua_typename(state, ::lua_type(state, -1));
			const char *value_msg = value != nullptr ? value : ::lua_typename(state, ::lua_type(state, -2));
			
			os << "key[" << key_msg << "] " 
				<< "value[" << value_msg << "]" << std::endl;

			::lua_pop(state, 2);
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
			//os << "lua table";
			traverse_table(state, idx, os);
			break;
		case LUA_TFUNCTION:
			os << "lua function[" << ::lua_tocfunction(state, idx) << "]";
			break;
		case LUA_TUSERDATA:
			os << "lua userdata[" << ::lua_touserdata(state, idx) << "]";
			break;
		case LUA_TTHREAD:
			os << "lua thread[" << ::lua_tothread(state, idx) << "]";
			break;
		default:
			os << "unknown type & value";
		}
	}


	template < typename OStreamT >
	void dump_parameter(state_t &state, OStreamT &os)
	{
		int param_num = ::lua_gettop(state);
		for( auto i = 1; i <= param_num; ++i )
		{
			parse_parameter(state, i, os);
			os << std::endl;
		}
	}

	class fatal_error_t
		: public std::exception
	{
		state_t state_;
		std::string msg_;
		int error_code_ = 0;

	public:
		fatal_error_t(state_t &state, std::string &&msg)
			: state_(state)
			, msg_(std::move(msg))
		{
			std::ostringstream os;

			os << std::endl << "lua stack: " << std::endl;
			dump(os);

			msg_ += os.str();
		}

		fatal_error_t(state_t &state, const std::string &msg, int code)
			: state_(state)
			, error_code_(code)
		{
			std::ostringstream os;
			os << std::endl << msg << " code: " << code;

			msg_ += os.str();
		}

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
	
		virtual ~fatal_error_t() throw()
		{}

	public:
		template < typename StreamT >
		void dump(StreamT &os) const
		{
			stack_trace(state_, os);
		}

		virtual const char* what() const throw()
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
		parameter_error_t(const state_t &state, std::string && msg)
			: state_(state)
			, msg_(std::move(msg))
		{
			std::ostringstream os;
			os << std::endl << "lua parameter:" << std::endl;

			dump_parameter(state_, os);

			os << std::endl << "lua stack: " << std::endl;
			dump(os);

			msg_ += os.str();
		}

		parameter_error_t(state_t &state, std::string && msg, int index)
			: state_(state)
			, msg_(std::move(msg))
		{
			std::ostringstream os;
			os << " lua parameter--";

			parse_parameter(state_, index, os);

			os << std::endl << "lua stack: " << std::endl;
			dump(os);

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

		virtual ~parameter_error_t() throw()
		{}

	public:
		template < typename StreamT >
		void dump(StreamT &os) const
		{
			stack_trace(state_, os);
		}

		virtual const char* what() const throw()
		{
			return msg_.c_str();
		}
	};
	

	template < typename T >
	void error_handler(state_t &state, T &error_msg)
	{
		auto lambda = [](lua_State *l)
		{
			state_t state(l);

			auto error_msg = static_cast<T *>(::lua_touserdata(state, lua_upvalueindex(1)));

			std::size_t len = 0;
			const char *msg = ::lua_tolstring(state, -1, &len);

			if( len == 0 )
				*error_msg = "unknown error";
			else
				error_msg->append(msg, len);

			std::ostringstream os;
			os << std::endl << "lua stack: " << std::endl;
			stack_trace(state, os);

			*error_msg += os.str();

			return 0;
		};

		::lua_pushlightuserdata(state, (void *) &error_msg);
		::lua_pushcclosure(state, lambda, 1);
	}



#define LUAREG_ERROR(expr, type, idx) error_report(state, (expr), type, idx, #expr);

	inline void error_report(lua_State *state, bool suc, int type, int idx, const char *msg)
	{
		if( suc )
			return;

		std::ostringstream os;
		os << "lua parameter error: " << std::endl
			<< "index [" << idx << "]" << std::endl
			<< "real type [" << ::lua_typename(state, ::lua_type(state, idx)) << "]" << std::endl
			<< "expected type [" << ::lua_typename(state, type) << "]" << std::endl
			<< "information [" << msg << "]" << std::endl;

		throw parameter_error_t(state_t(state), std::move(os.str()));
	}

}

#endif