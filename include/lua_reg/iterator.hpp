#ifndef __LUA_REG_ITERATOR_HPP
#define __LUA_REG_ITERATOR_HPP

#include <vector>
#include <iterator>

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
		template < typename T, typename AllocatorT >
		void get_all(std::vector<std::tuple<T, value_type_t, std::uint32_t>, AllocatorT> &values)
		{
			int param_num = ::lua_gettop(state_);
			values.reserve(param_num);
			
			for(auto i = 1; i <= param_num; ++i)
			{
				values.emplace_back(_parse_parameter<T>(state_, i));
			}
		}

	private:
		template < typename T >
		std::tuple<T, value_type_t, std::uint32_t> _parse_parameter(const state_t &state, int idx)
		{
			auto lua_type = ::lua_type(state, idx);

			switch(lua_type)
			{
			case LUA_TBOOLEAN:
				{
					return std::make_tuple(::lua_toboolean(state_, idx) == 1 ? true : false, value_type_t::BOOL_T, 1);
				}
				break;
			case LUA_TNUMBER:
				{
					return std::make_tuple(::lua_tonumber(state, idx), value_type_t::NUMBER_T, sizeof(double));
				}
				break;
			case LUA_TSTRING:
				{
					const char *value = lua_tostring(state, idx);
					return std::make_tuple(value, value_type_t::STRING_T, std::strlen(value));
				}
				break;
			default:
				throw parameter_error_t(state, "parameter type is not bool/number/string type");
			}
		}
	};
}


#endif