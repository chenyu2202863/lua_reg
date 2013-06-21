#ifndef __LUA_REG_DEF_DETAILS_HPP
#define __LUA_REG_DEF_DETAILS_HPP

#include <functional>

#include "../config.hpp"
#include "../converter.hpp"

namespace luareg { namespace details {


	template < std::uint32_t N >
	void check_stack(state_t &state)
	{
		int n = ::lua_gettop(state);
		assert(n == N);
	}



	template < std::uint32_t N, typename TupleT >
	struct auto_check_value_t
	{
		typedef typename std::tuple_element<N, TupleT>::type element_param_t;
		typedef typename std::remove_const<typename std::remove_reference<element_param_t>::type>::type param_t;

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
			check_stack<0>(state);
			handler();
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<1, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack<1>(state);
			return handler(std::cref(auto_check_value_t<0, TupleT>::check(state)));
		}
	};

	template < typename TupleT >
	struct param_handler_t<1, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack<1>(state);
			handler(std::cref(auto_check_value_t<0, TupleT>::check(state)));
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<2, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack<2>(state);
			return handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)));
		}
	};

	template < typename TupleT >
	struct param_handler_t<2, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack<2>(state);
			handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)));
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<3, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack<3>(state);
			return handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)),
				std::cref(auto_check_value_t<2, TupleT>::check(state)));
		}
	};

	template < typename TupleT >
	struct param_handler_t<3, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack<3>(state);
			handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)),
				std::cref(auto_check_value_t<2, TupleT>::check(state)));
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<4, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack<4>(state);
			return handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)),
				std::cref(auto_check_value_t<2, TupleT>::check(state)),
				std::cref(auto_check_value_t<3, TupleT>::check(state)));
		}
	};

	template < typename TupleT >
	struct param_handler_t<4, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack<4>(state);
			handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)),
				std::cref(auto_check_value_t<2, TupleT>::check(state)),
				std::cref(auto_check_value_t<3, TupleT>::check(state)));
		}
	};

	template < typename R, typename TupleT >
	struct param_handler_t<5, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack<5>(state);
			return handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)),
				std::cref(auto_check_value_t<2, TupleT>::check(state)),
				std::cref(auto_check_value_t<3, TupleT>::check(state)),
				std::cref(auto_check_value_t<4, TupleT>::check(state)));
		}
	};

	template < typename TupleT >
	struct param_handler_t<5, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack<5>(state);
			handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)),
				std::cref(auto_check_value_t<2, TupleT>::check(state)),
				std::cref(auto_check_value_t<3, TupleT>::check(state)),
				std::cref(auto_check_value_t<4, TupleT>::check(state)));
		}
	};


	template < typename R, typename TupleT >
	struct param_handler_t<6, R, TupleT>
	{
		template < typename HandlerT >
		static R handle(HandlerT &&handler, state_t &state)
		{
			check_stack<6>(state);
			return handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)),
				std::cref(auto_check_value_t<2, TupleT>::check(state)),
				std::cref(auto_check_value_t<3, TupleT>::check(state)),
				std::cref(auto_check_value_t<4, TupleT>::check(state)),
				std::cref(auto_check_value_t<5, TupleT>::check(state)));
		}
	};

	template < typename TupleT >
	struct param_handler_t<6, void, TupleT>
	{
		template < typename HandlerT >
		static void handle(HandlerT &&handler, state_t &state)
		{
			check_stack<6>(state);
			handler(
				std::cref(auto_check_value_t<0, TupleT>::check(state)),
				std::cref(auto_check_value_t<1, TupleT>::check(state)),
				std::cref(auto_check_value_t<2, TupleT>::check(state)),
				std::cref(auto_check_value_t<3, TupleT>::check(state)),
				std::cref(auto_check_value_t<4, TupleT>::check(state)),
				std::cref(auto_check_value_t<5, TupleT>::check(state)));
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
		static auto bind(FuncT func)->decltype(std::bind(func))
		{
			return std::bind(func);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj))
		{
			return std::bind(func, obj);
		}
	};


	template < typename R, typename T1 >
	struct bind_helper_t<R(T1)>
	{
		template < typename FuncT >
		static auto bind(FuncT func)->decltype(std::bind(func, _1))
		{
			return std::bind(func, _1);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1))
		{
			return std::bind(func, obj, _1);
		}
	};

	template < typename R, typename T1, typename T2 >
	struct bind_helper_t<R(T1, T2)>
	{
		template < typename FuncT >
		static auto bind(FuncT func)->decltype(std::bind(func, _1, _2))
		{
			return std::bind(func, _1, _2);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2))
		{
			return std::bind(func, obj, _1, _2);
		}
	};

	template < typename R, typename T1, typename T2, typename T3 >
	struct bind_helper_t<R(T1, T2, T3)>
	{
		template < typename FuncT >
		static auto bind(FuncT func)->decltype(std::bind(func, _1, _2, _3))
		{
			return std::bind(func, _1, _2, _3);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2, _3))
		{
			return std::bind(func, obj, _1, _2, _3);
		}
	};

	template < typename R, typename T1, typename T2, typename T3, typename T4 >
	struct bind_helper_t<R(T1, T2, T3, T4)>
	{
		template < typename FuncT >
		static auto bind(FuncT func)->decltype(std::bind(func, _1, _2, _3, _4))
		{
			return std::bind(func, _1, _2, _3, _4);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2, _3, _4))
		{
			return std::bind(func, obj, _1, _2, _3, _4);
		}
	};

	template < typename R, typename T1, typename T2, typename T3, typename T4, typename T5 >
	struct bind_helper_t<R(T1, T2, T3, T4, T5)>
	{
		template < typename FuncT >
		static auto bind(FuncT func)->decltype(std::bind(func, _1, _2, _3, _4, _5))
		{
			return std::bind(func, _1, _2, _3, _4, _5);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2, _3, _4, _5))
		{
			return std::bind(func, obj, _1, _2, _3, _4, _5);
		}
	};

	template < typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
	struct bind_helper_t<R(T1, T2, T3, T4, T5, T6)>
	{
		template < typename FuncT >
		static auto bind(FuncT func)->decltype(std::bind(func, _1, _2, _3, _4, _5, _6))
		{
			return std::bind(func, _1, _2, _3, _4, _5, _6);
		}

		template < typename FuncT, typename T >
		static auto bind(FuncT func, T obj)->decltype(std::bind(func, obj, _1, _2, _3, _4, _5, _6))
		{
			return std::bind(func, obj, _1, _2, _3, _4, _5, _6);
		}
	};
}

}

#endif