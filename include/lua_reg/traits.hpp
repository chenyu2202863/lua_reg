#ifndef __LUAREG_TRAITS_HPP
#define __LUAREG_TRAITS_HPP

namespace luareg { namespace detail {

	template <typename T>
	struct function_traits_t
		: public function_traits_t<decltype( &T::operator() )>
	{};


	namespace xx_impl 
	{
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
			arity = sizeof...(Args)
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


}}

#endif