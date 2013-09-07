#ifndef __LUA_REG_CONVERTER_DETAILS_HPP
#define __LUA_REG_CONVERTER_DETAILS_HPP

#include "../config.hpp"


namespace luareg {

	struct state_t;
}

namespace luareg { namespace details {

	template < typename T >
	struct check_type_t;

	template <>
	struct check_type_t<bool>
	{
		static void check(state_t &state, int idx)
		{
			LUAREG_ERROR(::lua_type(state, idx) == LUA_TBOOLEAN, LUA_TBOOLEAN, idx);
		}
	};


	template <>
	struct check_type_t<std::int32_t>
	{
		static void check(state_t &state, int idx)
		{
			LUAREG_ERROR(::lua_type(state, idx) == LUA_TNUMBER, LUA_TNUMBER, idx);
		}
	};

	template <>
	struct check_type_t<std::uint32_t>
		: check_type_t<std::int32_t>
	{};

	template <>
	struct check_type_t<std::int16_t>
		: check_type_t<std::int32_t>
	{};

	template <>
	struct check_type_t<std::uint16_t>
		: check_type_t<std::int32_t>
	{};

	template <>
	struct check_type_t<unsigned long>
		: check_type_t<std::int32_t>
	{};

	template <>
	struct check_type_t<long>
		: check_type_t<std::int32_t>
	{};

	template <>
	struct check_type_t<float>
		: check_type_t<std::int32_t>
	{};

	template <>
	struct check_type_t<double>
		: check_type_t<std::int32_t>
	{};

	template < typename AllocatorT >
	struct check_type_t<std::basic_string<char, std::char_traits<char>, AllocatorT>>
	{
		static void check(state_t &state, int idx)
		{
			LUAREG_ERROR(::lua_type(state, idx) == LUA_TSTRING, LUA_TSTRING, idx);
		}
	};

	template <>
	struct check_type_t<std::pair<const char *, std::uint32_t>>
	{
		static void check(state_t &state, int idx)
		{
			LUAREG_ERROR(::lua_type(state, idx) == LUA_TSTRING, LUA_TSTRING, idx);
		}
	};


	template < typename TupleT, std::uint32_t N >
	struct tuple_helper_t
	{
		typedef TupleT tuple_t;
		typedef typename std::tuple_element<N, tuple_t>::type value_t;

		static void from(state_t &state, int index, tuple_t &tuple_val)
		{
			check_type_t<value_t>::check(state, index);

			auto val = convertion_t<value_t>::from(state, index);
			std::get<N>(tuple_val) = std::move(val);

			tuple_helper_t<tuple_t, N - 1>::from(state, index - 1, tuple_val);
		}

		static void to(state_t &state, const tuple_t &val)
		{
			convertion_t<value_t>::to(state, std::get<N>(val));

			tuple_helper_t<tuple_t, N - 1>::to(state, val);
		}

	};

	template < typename TupleT >
	struct tuple_helper_t<TupleT, 0>
	{
		typedef TupleT tuple_t;
		typedef typename std::tuple_element<0, tuple_t>::type value_t;


		static void from(state_t &state, int index, tuple_t &tuple_val)
		{
			check_type_t<value_t>::check(state, ::lua_type(state, index));

			auto val = convertion_t<value_t>::from(state, index);
			std::get<0>(tuple_val) = std::move(val);
		}

		static void to(state_t &state, const tuple_t &val)
		{
			convertion_t<value_t>::to(state, std::get<0>(val));
		}
	};
}}

#endif