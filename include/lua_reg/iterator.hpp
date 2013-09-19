#ifndef __LUA_REG_ITERATOR_HPP
#define __LUA_REG_ITERATOR_HPP

#include <iterator>
#include <functional>

namespace luareg {

	/*class parameter_iterator_t
	: public std::iterator<
	*/

	enum class value_type_t
	{
		BOOL_T,
		NUMBER_T,
		STRING_T
	};

	struct state_t;

	class parameter_t
	{
		const state_t &state_;

	public:
		parameter_t(const state_t &state)
			: state_(state)
		{}

	private:
		parameter_t(const parameter_t &);
		parameter_t &operator=(const parameter_t &);

	public:
		template < typename T >
		void get_all(const std::function<void(const T &)> &handler, int start_idx, int cnt)
		{
			for(auto i = start_idx + 1; i <= cnt + start_idx; ++i)
			{
				_parse_parameter<T>(i, handler);
			}
		}

	private:
		template < typename T >
		void _parse_parameter(int idx, const std::function<void(const T &)> &handler)
		{
			auto lua_type = ::lua_type(state_, idx);

			switch(lua_type)
			{
			case LUA_TBOOLEAN:
				{
					handler(::lua_toboolean(state_, idx) == 1);
				}
				break;
			case LUA_TNUMBER:
				{
					handler(::lua_tonumber(state_, idx));
				}
				break;
			case LUA_TSTRING:
				{
					std::uint32_t len = 0;
					const char *value = ::lua_tolstring(state_, idx, &len);
					handler(std::make_pair(value, len));
				}
				break;
			default:
				throw parameter_error_t(state_, "parameter type is not bool/number/string type");
			}
		}
	};
}


#endif