#ifndef __LUA_REG_DEF_DETAILS_HPP
#define __LUA_REG_DEF_DETAILS_HPP

#include <functional>

#include "../config.hpp"
#include "../converter.hpp"
#include "../error.hpp"

namespace luareg { namespace details {


	void check_stack(state_t &state, std::uint32_t N)
	{
		int n = ::lua_gettop(state);
		if ( n != N )
			throw parameter_error_t(state, "expect parameter count error ");
	}

	template < typename T >
	struct is_normal_t
	{
		typedef std::true_type type;
	};

	template <>
	struct is_normal_t<state_t>
	{
		typedef std::false_type type;
	};

	template <>
	struct is_normal_t<index_t>
	{
		typedef std::false_type type;
	};


	template < std::uint32_t N >
	struct param_count_t
	{
		template < typename TupleT >
		static void count(std::uint32_t &cnt)
		{
			typedef typename std::tuple_element<N, TupleT>::type element_type;
			typedef typename std::remove_cv<typename std::remove_reference<element_type>::type>::type origi_type;
			typedef typename is_normal_t<origi_type>::type type;

			cnt += std::is_same<type, std::true_type>::value ? 1 : 0;
			param_count_t<N - 1>::count<TupleT>(cnt);
		}
	};

	template < >
	struct param_count_t<0>
	{
		template < typename TupleT >
		static void count(std::uint32_t &cnt)
		{
			typedef typename std::tuple_element<0, TupleT>::type element_type;
			typedef typename std::remove_cv<typename std::remove_reference<element_type>::type>::type origi_type;
			typedef typename is_normal_t<origi_type>::type type;

			cnt += std::is_same<type, std::true_type>::value ? 1 : 0;
		}
	};

	template < typename TupleT >
	std::uint32_t param_count()
	{
		std::uint32_t cnt = 0;
		param_count_t<std::tuple_size<TupleT>::value - 1>::count<TupleT>(cnt);
		return cnt;
	}


	template < std::uint32_t N, typename TupleT >
	struct auto_check_value_t
	{
		typedef typename std::tuple_element<N, TupleT>::type element_param_t;
		typedef typename std::remove_cv<typename std::remove_reference<element_param_t>::type>::type param_t;

		static auto check(state_t &state)->decltype(convertion_t<param_t>::from(state, N + 1))
		{
			// lua stack index begin from 1
			return convertion_t<param_t>::from(state, N + 1);
		}
	};

	template < std::uint32_t N, typename R, typename TupleT >
	struct param_handler_t;

	template < typename R, typename TupleT >
	struct param_handler_t<0, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			return handler();
		}
	};

	template < typename TupleT >
	struct param_handler_t<0, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			handler();
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<1, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			static_assert(std::is_same<R, decltype(handler(auto_check_value_t<0, TupleT>::check(state)))>::value, "R type must same as decltype(handler())");
			return handler(auto_check_value_t<0, TupleT>::check(state));
		}
	};

	template < typename TupleT >
	struct param_handler_t<1, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			handler(auto_check_value_t<0, TupleT>::check(state));
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<2, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			return handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state));
		}
	};

	template < typename TupleT >
	struct param_handler_t<2, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state));
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<3, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			return handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state),
				auto_check_value_t<2, TupleT>::check(state));
		}
	};

	template < typename TupleT >
	struct param_handler_t<3, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state),
				auto_check_value_t<2, TupleT>::check(state));
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<4, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			return handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state),
				auto_check_value_t<2, TupleT>::check(state),
				auto_check_value_t<3, TupleT>::check(state));
		}
	};

	template < typename TupleT >
	struct param_handler_t<4, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state),
				auto_check_value_t<2, TupleT>::check(state),
				auto_check_value_t<3, TupleT>::check(state));
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<5, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			return handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state),
				auto_check_value_t<2, TupleT>::check(state),
				auto_check_value_t<3, TupleT>::check(state),
				auto_check_value_t<4, TupleT>::check(state));
		}
	};

	template < typename TupleT >
	struct param_handler_t<5, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state),
				auto_check_value_t<2, TupleT>::check(state),
				auto_check_value_t<3, TupleT>::check(state),
				auto_check_value_t<4, TupleT>::check(state));
		}
	};


	template < typename R, typename TupleT >
	struct param_handler_t<6, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			return handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state),
				auto_check_value_t<2, TupleT>::check(state),
				auto_check_value_t<3, TupleT>::check(state),
				auto_check_value_t<4, TupleT>::check(state),
				auto_check_value_t<5, TupleT>::check(state));
		}
	};

	template < typename TupleT >
	struct param_handler_t<6, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack(state, param_count<TupleT>());
			handler(
				auto_check_value_t<0, TupleT>::check(state),
				auto_check_value_t<1, TupleT>::check(state),
				auto_check_value_t<2, TupleT>::check(state),
				auto_check_value_t<3, TupleT>::check(state),
				auto_check_value_t<4, TupleT>::check(state),
				auto_check_value_t<5, TupleT>::check(state));
		}
	};

	// -----------

	template < std::uint32_t N, typename R, typename TupleT >
	struct return_handle_t
	{
		template < typename HandlerT >
		static std::int32_t handle(HandlerT &&handler, state_t &state)
		{
			R ret = param_handler_t<N, R, TupleT>::handle(std::forward<HandlerT>(handler), state);

			return convertion_t<R>::to(state, ret);
		}
	};

	template < std::uint32_t N, typename TupleT >
	struct return_handle_t<N, void, TupleT>
	{
		template < typename HandlerT >
		static std::int32_t handle(HandlerT &&handler, state_t &state)
		{
			param_handler_t<N, void, TupleT>::handle(std::forward<HandlerT>(handler), state);
			return 0;
		}
	};


	using namespace std::placeholders;

	
	template < typename T >
	struct bind_helper_t;

	template < typename R >
	struct bind_helper_t<R()>
	{
		template < typename FuncT >
		static auto bind(FuncT &&func)->decltype(std::bind(func))
		{
			return std::bind(func);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT &&func, T obj)->decltype(std::bind(func, obj))
		{
			return std::bind(func, obj);
		}
	};


	template < typename R, typename T1 >
	struct bind_helper_t<R(T1)>
	{
		template < typename FuncT >
		static auto bind(FuncT &&func)->decltype(std::bind(func, _1))
		{
			return std::bind(func, _1);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT &&func, T obj)->decltype(std::bind(func, obj, _1))
		{
			return std::bind(func, obj, _1);
		}
	};

	template < typename R, typename T1, typename T2 >
	struct bind_helper_t<R(T1, T2)>
	{
		template < typename FuncT >
		static auto bind(FuncT &&func)->decltype(std::bind(func, _1, _2))
		{
			return std::bind(func, _1, _2);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT &&func, T obj)->decltype(std::bind(func, obj, _1, _2))
		{
			return std::bind(func, obj, _1, _2);
		}
	};

	template < typename R, typename T1, typename T2, typename T3 >
	struct bind_helper_t<R(T1, T2, T3)>
	{
		template < typename FuncT >
		static auto bind(FuncT &&func)->decltype(std::bind(func, _1, _2, _3))
		{
			return std::bind(func, _1, _2, _3);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT &&func, T obj)->decltype(std::bind(func, obj, _1, _2, _3))
		{
			return std::bind(func, obj, _1, _2, _3);
		}
	};

	template < typename R, typename T1, typename T2, typename T3, typename T4 >
	struct bind_helper_t<R(T1, T2, T3, T4)>
	{
		template < typename FuncT >
		static auto bind(FuncT &&func)->decltype(std::bind(func, _1, _2, _3, _4))
		{
			return std::bind(func, _1, _2, _3, _4);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT &&func, T obj)->decltype(std::bind(func, obj, _1, _2, _3, _4))
		{
			return std::bind(func, obj, _1, _2, _3, _4);
		}
	};

	template < typename R, typename T1, typename T2, typename T3, typename T4, typename T5 >
	struct bind_helper_t<R(T1, T2, T3, T4, T5)>
	{
		template < typename FuncT >
		static auto bind(FuncT &&func)->decltype(std::bind(func, _1, _2, _3, _4, _5))
		{
			return std::bind(func, _1, _2, _3, _4, _5);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT &&func, T obj)->decltype(std::bind(func, obj, _1, _2, _3, _4, _5))
		{
			return std::bind(func, obj, _1, _2, _3, _4, _5);
		}
	};

	template < typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
	struct bind_helper_t<R(T1, T2, T3, T4, T5, T6)>
	{
		template < typename FuncT >
		static auto bind(FuncT &&func)->decltype(std::bind(func, _1, _2, _3, _4, _5, _6))
		{
			return std::bind(func, _1, _2, _3, _4, _5, _6);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT &&func, T obj)->decltype(std::bind(func, obj, _1, _2, _3, _4, _5, _6))
		{
			return std::bind(func, obj, _1, _2, _3, _4, _5, _6);
		}
	};
}

}

#endif