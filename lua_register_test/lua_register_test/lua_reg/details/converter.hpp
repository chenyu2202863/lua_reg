#ifndef __LUA_REG_CONVERTER_DETAILS_HPP
#define __LUA_REG_CONVERTER_DETAILS_HPP

namespace luareg { namespace details {

	template < typename TupleT, std::uint32_t N >
	struct tuple_helper_t
	{
		typedef TupleT tuple_t;
		typedef typename std::tuple_element<N - 1, tuple_t>::type value_t;

		static void from(state_t &state, int index, tuple_t &tuple_val)
		{
			::lua_rawgeti(state, index, N);

			auto val = convertion_t<value_t>::from(state, -1);
			::lua_pop(state, 1);

			std::get<N - 1>(tuple_val) = std::move(val);
			tuple_helper_t<tuple_t, N - 1>::from(state, index, tuple_val);
		}

		static void to(state_t &state, const tuple_t &val)
		{
			convertion_t<value_t>::to(state, std::get<N - 1>(val));
			::lua_rawseti(state, -2, N - 1);

			tuple_helper_t<tuple_t, N - 1>::to(state, val);
		}

	};

	template < typename TupleT >
	struct tuple_helper_t<TupleT, 1>
	{
		typedef TupleT tuple_t;
		typedef typename std::tuple_element<0, tuple_t>::type value_t;


		static void from(state_t &state, int index, tuple_t &tuple_val)
		{
			::lua_rawgeti(state, index, 1);

			auto val = std::move(convertion_t<value_t>::from(state, -1));
			std::get<0>(tuple_val) = std::move(val);

			::lua_pop(state, 1);
		}

		static void to(state_t &state, const tuple_t &val)
		{
			convertion_t<value_t>::to(state, std::get<0>(val));
			::lua_rawseti(state, -2, 1);
		}
	};
}}

#endif