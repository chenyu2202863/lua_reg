#ifndef __LUAREG_TRAITS_HPP
#define __LUAREG_TRAITS_HPP

namespace luareg { 
	
	struct state_t;
	struct index_t;


	namespace details {

		template <typename T>
		struct function_traits_t
			: public function_traits_t<decltype( &T::operator( ) )>
		{};


		namespace xx_impl {
			template <typename C, typename R, typename... A>
			struct memfn_type
			{
				typedef typename std::conditional <
					std::is_const<C>::value, typename std::conditional <std::is_volatile<C>::value, R(C::*)( A... ) const volatile, R(C::*)( A... ) const> ::type,
					typename std::conditional <std::is_volatile<C>::value, R(C::*)( A... ) volatile, R(C::*)( A... )> ::type
				> ::type type;
			};
		}


		template <typename R, typename... Args>
		struct function_traits_t<R(Args...)>
		{
			typedef R result_type;
			typedef std::tuple<Args...> args_type;
			typedef R function_type(Args...);

			template <typename OwnerType>
			struct member_function_type
				: xx_impl::memfn_type <
				typename std::remove_pointer<typename std::remove_reference<OwnerType>::type>::type,
				R, Args...
				> ::type
			{};

			enum
			{
				arity = sizeof...( Args )
			};

			template <size_t i>
			struct arg
			{
				typedef typename std::tuple_element<i, args_type>::type type;
			};
		};


		template <typename R, typename... Args>
		struct function_traits_t<R(*)( Args... )>
			: public function_traits_t<R(Args...)>
		{};


		template <typename C, typename R, typename... Args>
		struct function_traits_t<R(C::*)( Args... )>
			: public function_traits_t<R(Args...)>
		{
			typedef C& owner_type;
		};


		template <typename C, typename R, typename... Args>
		struct function_traits_t<R(C::*)( Args... ) const>
			: public function_traits_t<R(Args...)>
		{
			typedef const C& owner_type;
		};


		template <typename C, typename R, typename... Args>
		struct function_traits_t<R(C::*)( Args... ) volatile>
			: public function_traits_t<R(Args...)>
		{
			typedef volatile C& owner_type;
		};


		template <typename C, typename R, typename... Args>
		struct function_traits_t<R(C::*)( Args... ) const volatile>
			: public function_traits_t<R(Args...)>
		{
			typedef const volatile C& owner_type;
		};


		template <typename FunctionType>
		struct function_traits_t<std::function<FunctionType>>
			: public function_traits_t<FunctionType>
		{};


		template <typename T>
		struct function_traits_t<T&> : public function_traits_t<T> {};
		template <typename T>
		struct function_traits_t<const T&> : public function_traits_t<T> {};
		template <typename T>
		struct function_traits_t<volatile T&> : public function_traits_t<T> {};
		template <typename T>
		struct function_traits_t<const volatile T&> : public function_traits_t<T> {};
		template <typename T>
		struct function_traits_t<T && > : public function_traits_t<T> {};
		template <typename T>
		struct function_traits_t<const T && > : public function_traits_t<T> {};
		template <typename T>
		struct function_traits_t<volatile T && > : public function_traits_t<T> {};
		template <typename T>
		struct function_traits_t<const volatile T && > : public function_traits_t<T> {};


		// is_callable
		template<typename Func, typename... Args, typename = decltype( std::declval<Func>( )( std::declval<Args>( )... ) )> std::true_type is_callable_helper(int);
		template<typename Func, typename... Args> std::false_type is_callable_helper(...);
		template<typename T> struct is_callable;
		template<typename Func, typename... Args> struct is_callable<Func(Args...)>: public decltype( is_callable_helper<Func, Args...>( 0 ) ) {};


		template < typename T >
		struct is_normal_t
		{
			typedef std::true_type type;
			static const std::uint32_t value = 1;
		};

		template <>
		struct is_normal_t<state_t>
		{
			typedef std::false_type type;
			static const std::uint32_t value = 0;
		};

		template <>
		struct is_normal_t<index_t>
		{
			typedef std::false_type type;
			static const std::uint32_t value = 0;
		};

		template < typename ...Args >
		struct is_normal_t<std::tuple<Args...>>
		{
			typedef std::true_type type;
			static const std::uint32_t value = sizeof...(Args);
		};


		template < typename ...Args >
		struct parameter_count_t;

		template <>
		struct parameter_count_t<>
		{
			static const std::uint32_t value = 0;
		};

		template < typename T >
		struct parameter_count_t<T>
		{
			typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type origi_type;

			static const std::uint32_t value = is_normal_t<origi_type>::value;
		};

		template < typename T, typename ...Args >
		struct parameter_count_t<T, Args...>
			: parameter_count_t<Args...>
		{
			typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type origi_type;

			static const std::uint32_t value = is_normal_t<origi_type>::value +
				parameter_count_t<Args...>::value;
		};


		template < typename ...Args >
		struct parameter_count_t<std::tuple<Args...>>
			: parameter_count_t<Args...>
		{
			static const std::uint32_t value = parameter_count_t<Args...>::value;
		};


		template <typename T, std::size_t N, typename... Args>
		struct number_of_element_from_tuple_t
		{
			static const auto value = N + 1;
		};

		template <typename T, std::size_t N, typename... Args>
		struct number_of_element_from_tuple_t<T, N, T, Args...>
		{
			static const auto value = N;
		};

		template <typename T, std::size_t N, typename U, typename... Args>
		struct number_of_element_from_tuple_t<T, N, U, Args...>
		{
			static const auto value = number_of_element_from_tuple_t<T, N + 1, Args...>::value;
		};

		template <typename T, typename... Args>
		T get_element_by_type(const std::tuple<Args...>& t)
		{
			return std::get<number_of_element_from_tuple_t<T, 0, Args...>::value>(t);
		}

		template < typename T, typename EnableT = void >
		struct has_special_type_t;

		template < typename T, typename ...Args >
		struct has_special_type_t<T, std::tuple<Args...>>
		{
			typedef typename std::conditional 
				< 
				number_of_element_from_tuple_t<T, 0, Args...>::value != std::tuple_size < std::tuple<Args... >>::value,
				std::true_type,
				std::false_type
				>::type type;
		};
		

	}
}

#endif