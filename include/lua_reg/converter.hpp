#ifndef __LUA_REG_CONVERSION_HPP
#define __LUA_REG_CONVERSION_HPP


#include <map>
#include <vector>
#include <tuple>
#include <string>
#include <type_traits>
#include <cstdint>
#include <algorithm>

#include "config.hpp"
#include "state.hpp"
#include "error.hpp"
#include "reference.hpp"
#include "details/converter.hpp"

namespace luareg {
	
	struct return_number_t
	{
		std::uint32_t num_;
		return_number_t(std::uint32_t num)
			: num_(num)
		{}
	};

	template < typename T >
	struct is_userdata_traits_t
		: std::false_type
	{};

	template <>
	struct is_userdata_traits_t<std::string>
		: std::false_type
	{};



	template < >
	struct convertion_t<void *>
	{
		static void * from(state_t &state, int index)
		{
			LUAREG_ERROR(lua_islightuserdata(state, index) != 0, LUA_TLIGHTUSERDATA, index);

			return ::lua_touserdata(state, index);
		}

		static std::uint32_t to(state_t &state, const void *val)
		{
			if( val != nullptr )
				::lua_pushlightuserdata(state, (void *)val);
			else
				::lua_pushnil(state);

			return 1;
		}
	};

	template < typename T >
	struct convertion_t<T *>
	{
		static T * from(state_t &state, int index)
		{
			LUAREG_ERROR(::lua_isuserdata(state, index) != 0, LUA_TUSERDATA, index);

			return static_cast<T *>(::lua_touserdata(state, index));
		}
		
		static std::uint32_t to(state_t &state, const T * val)
		{
			if( val != nullptr )
				::lua_pushlightuserdata(state, (void *)(val));
			else
				::lua_pushnil(state);

			return 1;
		}
	};


	template <>
	struct convertion_t<lua_CFunction>
	{
		static lua_CFunction from(state_t &state, int index)
		{
			LUAREG_ERROR(lua_iscfunction(state, index) != 0, LUA_TFUNCTION, index);

			auto func = ::lua_tocfunction(state, index);
			return func;
		}

		static std::uint32_t to(state_t &state, lua_CFunction val)
		{
			::lua_pushcfunction(state, val);
			return 1;
		}
	};

	template < typename T >
	struct convertion_t<T, 
		typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value>::type >
	{
		static T from(state_t &state, int index)
		{
			LUAREG_ERROR(::lua_isnumber(state, index) != 0, LUA_TNUMBER, index);

			return static_cast<T>(::lua_tonumber(state, index));
		}

		static std::uint32_t to(state_t &state, T val)
		{
			::lua_pushnumber(state, static_cast<lua_Number>(val));
			return 1;
		}
	};

	template <>
	struct convertion_t<bool>
	{
		static bool from(state_t &state, int index)
		{
			LUAREG_ERROR(lua_isboolean(state, index) != 0, LUA_TBOOLEAN, index);
			int n = ::lua_toboolean(state, index);
			return n != 0;
		}

		static std::uint32_t to(state_t &state, bool val)
		{
			::lua_pushboolean(state, val);
			return 1;
		}
	};


	template < >
	struct convertion_t< const char * >
	{
		static const char * from(state_t &state, int index)
		{
			LUAREG_ERROR(::lua_isstring(state, index) != 0, LUA_TSTRING, index);
			return ::lua_tostring(state, index);
		}

		static std::uint32_t to(state_t &state, const char *val)
		{
			::lua_pushstring(state, val);
			return 1;
		}
	};

	template < std::uint32_t N >
	struct convertion_t< char [N] >
	{
		static std::string from(state_t &state, int index)
		{
			LUAREG_ERROR(::lua_isstring(state, index) != 0, LUA_TSTRING, index);
			return ::lua_tostring(state, index);
		}

		static std::uint32_t to(state_t &state, const char *val)
		{
			::lua_pushstring(state, val);
			return 1;
		}
	};

	template < typename CharT, typename CharTraitsT, typename AllocatorT >
	struct convertion_t< std::basic_string<CharT, CharTraitsT, AllocatorT> >
	{
		static std::basic_string<CharT, CharTraitsT, AllocatorT> from(state_t &state, int index)
		{
			LUAREG_ERROR(::lua_isstring(state, index) != 0, LUA_TSTRING, index);
			std::size_t len = 0;
			auto p = ::lua_tolstring(state, index, &len);
			return std::basic_string<CharT, CharTraitsT, AllocatorT>(p, len);
		}

		static std::uint32_t to(state_t &state, const std::basic_string<CharT, CharTraitsT, AllocatorT> &val)
		{
			::lua_pushlstring(state, val.c_str(), val.size());
			return 1;
		}
	};

	// struct nil

	template < typename FirstT, typename SecondT >
	struct convertion_t< std::pair<FirstT, SecondT> >
	{
		typedef std::pair<FirstT, SecondT> pair_t;

		static pair_t from(state_t &state, int index)
		{
			LUAREG_ERROR(lua_istable(state, index) != 0, LUA_TTABLE, index);

			const std::uint32_t len = (std::uint32_t)::lua_rawlen(state, index);
			assert(len == 2);

			::lua_rawgeti(state, index, 1);
			FirstT first_val = convertion_t<FirstT>::from(state, -1);
			lua_pop(state, 1);

			::lua_rawgeti(state, index, 2);
			SecondT second_val = convertion_t<SecondT>::from(state, -1);
			lua_pop(state, 1);

			return { std::move(first_val), std::move(second_val) };
		}

		static std::uint32_t to(state_t &state, const pair_t &val)
		{
			::lua_createtable(state, 2, 0);

			convertion_t<FirstT>::to(state, val.first);
			::lua_rawseti(state, -2, 1);

			convertion_t<SecondT>::to(state, val.second);
			::lua_rawseti(state, -2, 2);

			return 1;
		}
	};

	template < >
	struct convertion_t< std::pair<const char *, std::uint32_t> >
	{
		static std::pair<const char *, std::uint32_t> from(state_t &state, int index)
		{
			LUAREG_ERROR(::lua_isstring(state, index) != 0, LUA_TSTRING, index);

			std::size_t len = 0;
			const char *p = ::lua_tolstring(state, index, &len);

			return { p, (std::uint32_t)len };
		}

		static std::uint32_t to(state_t &state, const std::pair<const char *, std::uint32_t> &val)
		{
			::lua_pushlstring(state, val.first, val.second);
			return 1;
		}
	};

	template < typename ...Args >
	struct convertion_t< std::tuple<Args...> >
	{
		typedef std::tuple<Args...> tuple_t;

		static tuple_t from(state_t &state, int index)
		{
			tuple_t val;
			details::tuple_helper_t<tuple_t, std::tuple_size<tuple_t>::value - 1>::from(state, -1, val);

			return val;
		}

		static std::uint32_t to(state_t &state, const tuple_t &val)
		{
			details::tuple_helper_t<tuple_t, std::tuple_size<tuple_t>::value - 1>::to(state, val);

			return std::tuple_size<tuple_t>::value;
		}
	};

	template < typename T, typename AllocatorT >
	struct convertion_t< std::vector<T, AllocatorT> >
	{
		typedef std::vector<T, AllocatorT> vector_t;

		static vector_t from(state_t &state, int index)
		{
			LUAREG_ERROR(lua_istable(state, index) != 0, LUA_TTABLE, index);

			vector_t vec;
			const int len = ::lua_rawlen(state, index);
			vec.reserve(len);

			for(auto i = 1; i <= len; ++i) 
			{
				::lua_rawgeti(state, index, i);
				vec.emplace_back(std::move(static_cast<T>(convertion_t<T>::from(state, -1))));
				lua_pop(state, 1);
			}

			return vec;
		}

		static std::uint32_t to(state_t &state, const vector_t &val)
		{
			::lua_createtable(state, (int)val.size(), (int)val.size());

			std::uint32_t i = 1;
			std::for_each(val.cbegin(), val.cend(), 
				[&state, &i](const T &t)
			{
				convertion_t<T>::to(state, t);
				::lua_rawseti(state, -2, i++);
			});

			return 1;
		}
	};

	template < typename KeyT, typename ValueT, typename CompareT, typename AllocatorT >
	struct convertion_t< std::map<KeyT, ValueT, CompareT, AllocatorT> >
	{
		typedef std::map<KeyT, ValueT, CompareT, AllocatorT> map_t;

		static map_t from(state_t &state, int index)
		{
			LUAREG_ERROR(lua_istable(state, index) != 0, LUA_TTABLE, index);

			map_t map_val;
			::lua_pushvalue(state, index);
			::lua_pushnil(state);

			while( ::lua_next(state, -2) )
			{
				::lua_pushvalue(state, -2);

				auto key = convertion_t<KeyT>::from(state, -1);
				auto value = convertion_t<ValueT>::from(state, -2);
				//map_val.emplace(std::move(key), std::move(value));
				map_val.insert(std::make_pair(key, value));

				lua_pop(state, 2);
			}

			lua_pop(state, 1);

			return map_val;
		}

		static std::uint32_t to(state_t &state, const map_t &map_val)
		{
			::lua_createtable(state, (int)map_val.size(), (int)map_val.size());

			std::for_each(map_val.cbegin(), map_val.cend(), 
				[&state](const typename map_t::value_type &val)
			{
				convertion_t<typename map_t::key_type>::to(state, val.first);
				convertion_t<typename map_t::mapped_type>::to(state, val.second);

				::lua_settable(state, -3);
			});

			return map_val.size() == 0 ? 0 : 1;
		}
	};


	template < >
	struct convertion_t< return_number_t >
	{
		static std::uint32_t to(state_t &state, const return_number_t &val)
		{
			return val.num_;
		}
	};

	template < >
	struct convertion_t< state_t >
	{
		static state_t from(state_t &state, int index)
		{
			return state;
		}
	};

	template < >
	struct convertion_t< index_t >
	{
		static index_t from(state_t &state, int index)
		{
			return {index};
		}
	};

	template <>
	struct convertion_t<function_ref_t>
	{
		static function_ref_t from(state_t &state, int index)
		{
			LUAREG_ERROR(lua_isfunction(state, index) != 0, LUA_TFUNCTION, index);
			::lua_pushvalue(state, index);
			return function_ref_t(state);
		}

		static std::uint32_t to(state_t &state, const function_ref_t &ref)
		{
			ref.get();
			return 1;
		}
	};

	template <>
	struct convertion_t<table_ref_t>
	{
		static table_ref_t from(state_t &state, int index)
		{
			LUAREG_ERROR(lua_istable(state, index) != 0, LUA_TTABLE, index);
			::lua_pushvalue(state, index);
			return table_ref_t(state);
		}

		static std::uint32_t to(state_t &state, const table_ref_t &ref)
		{
			ref.get();
			return 1;
		}
	};

	template <>
	struct convertion_t<string_ref_t>
	{
		static string_ref_t from(state_t &state, int index)
		{
			LUAREG_ERROR(::lua_isstring(state, index) != 0, LUA_TSTRING, index);
			::lua_pushvalue(state, index);
			return string_ref_t(state);
		}

		static std::uint32_t to(state_t &state, const string_ref_t &ref)
		{
			ref.get();
			return 1;
		}
	};
}

#endif