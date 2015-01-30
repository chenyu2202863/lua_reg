

#include <iterator>
#include <iostream>
#include <thread>

#include "../../../include/lua_reg/lua_reg.hpp"


namespace lua = luareg;


namespace luareg  {

	template <>
	struct convertion_t<std::pair<const std::uint32_t *, std::uint32_t>>
	{
		static std::pair<const std::uint32_t *, std::uint32_t> from(state_t &state, int index)
		{
			std::uint32_t len = 0;
			auto p = ::lua_tolstring(state, index, &len);

			return{ reinterpret_cast<const std::uint32_t *>(p), len };
		}
	};
}

void test0()
{}



double test1(int n)
{
	return 1234567891234567;
}

int test2(int n, double d, const std::string &msg)
{
	return 10;
}

std::string test3(const std::string &msg)
{
	return msg;
}

bool test4(bool suc)
{
	return !suc;
}

std::string test5(const std::string &msg, const std::string &msg2)
{
	return msg + msg2;
}


struct test_t
{
	std::string m_ = { "123" };

	char test6(bool suc, const std::string &txt)
	{
		return 'a';
	}

	int test11() const
	{
		return 1;
	}
};


void test7(const std::pair<const char *, std::uint32_t> &buffer)
{

}

std::vector<int> test8(const std::vector<int> &v)
{
	return v;
}

std::vector<std::pair<int, std::string>> test9(int n, const std::vector<std::pair<int, std::string>> &v)
{
	return v;
}

std::pair<std::string, std::uint32_t> test10(const std::pair<std::string, std::uint32_t> &val)
{
	return val;
}

std::pair<std::string, std::pair<std::uint32_t, std::uint32_t>> test12(const std::vector<std::pair<std::string, std::pair<std::uint32_t, std::uint32_t>>> &val)
{
	return val.front();
}

std::tuple<bool, int, std::string> test13(const std::tuple<bool, int, std::string> &val)
{
	return val;
}


std::map<int, std::string> test14(const std::map<int, std::string> &val)
{
	return val;
}


void test15(const std::string &m, const lua::function_ref_t &func_ref, int n, lua::state_t &state)
{
	lua::call(state, func_ref, m, n);
}

void test16(const lua::function_ref_t &func_ref, lua::state_t &state)
{
	std::tuple<bool, std::string, std::uint32_t, std::string> ret = lua::call(state, func_ref, 10, "test");
}

void test17(const lua::function_ref_t &func_ref, lua::string_ref_t &string_ref, lua::state_t &state)
{
	auto string_ref1 = std::move(string_ref);
	lua::call(state, func_ref, string_ref1);
}

void test18(const lua::function_ref_t &func_ref, const lua::function_ref_t &func_param, lua::state_t &state)
{
	lua::call(state, func_ref, func_param);
}

std::uint32_t test20(lua::state_t &state, const lua::index_t idx)
{
	return 10;
}


struct base_t
{
	static const bool is_userdata = true;

	int m_ = 10, n_ = 20;

	void print()
	{
		std::cout << __FUNCTION__ << std::endl;
	}

	void cacu(int n, luareg::function_ref_t &func, luareg::state_t &state)
	{
		auto cnt = m_ + n_ + n;
	}
};

class foo_t
{
	int m_;
	double n_;

	static const bool is_userdata = true;

public:
	foo_t(int n)
		: m_(n)
		, n_(n)
	{}

	~foo_t()
	{

	}

	void add(int n1, int n2)
	{
		m_ = n1 + n2;
	}

	int get()
	{
		return m_;
	}

	foo_t *get_pointer()
	{
		return this;
	}

	base_t *get_base()
	{
		return new base_t;
	}
};




int _tmain(int argc, _TCHAR* argv[])
{
	std::allocator<char> std_allocator;
	luareg::state_t state(std_allocator);

	try
	{
		{
			test_t t;

			// free function & class method
			luareg::module(state, "cpp")
				<< lua::def("test0", &test0)
				<< lua::def("test1", &test1)
				<< lua::def("test2", &test2)
				<< lua::def("test3", &test3)
				<< lua::def("test4", &test4)
				<< lua::def("test5", &test5)
				<< lua::def("test6", &t, &test_t::test6);

			test_t *m = new test_t;
			luareg::module(state, "cpp")
				<< lua::def("test7", &test7)
				<< lua::def("test8", &test8)
				<< lua::def("test9", &test9)
				<< lua::def("test10", &test10)
				<< lua::def("test11", &t, &test_t::test11)
				<< lua::def("test12", &test12)
				<< lua::def("test13", &test13)
				<< lua::def("test14", &test14)
				<< lua::def("test15", &test15)
				<< lua::def("test16", &test16)
				<< lua::def("test17", &test17)
				<< lua::def("test18", &test18)
				<< lua::def("test19", [m](int n, const std::string &msg)->int
			{
				return n;
			})
				<< lua::def("test20", &test20);

			luareg::module(state, "cpp")("std")
				<< luareg::def("test21", []()
			{
				std::cout << __FUNCTION__ << std::endl;
			});

			luareg::module(state, "cpp")("std")
				<< luareg::def("test22", []()
			{
				std::cout << __FUNCTION__ << std::endl;
			});


		}

		{
		base_t base;
		auto p_base = &base;
		luareg::module(state)
			<< lua::def("base_print", &base, &base_t::print)
			<< lua::def("base_cacu", &base, &base_t::cacu);

		// object method

		luareg::module(state, "cpp")
			[
				luareg::class_t<foo_t>(state, "foo_t")
				<< luareg::constructor<int>()
				<< luareg::destructor()
				<< luareg::def("add", &foo_t::add)
				<< luareg::def("get", &foo_t::get)
				<< luareg::def("get_pointer", &foo_t::get_pointer)
				<< luareg::def("get_base", &foo_t::get_base)
			];
	}

		lua::execute(state, "test.lua");
	}
	catch( const luareg::parameter_error_t &e )
	{
		std::cout << e.what() << std::endl;
		e.dump(std::cout);
	}
	catch( const luareg::fatal_error_t &e )
	{
		std::cout << e.what() << std::endl;
		e.dump(std::cout);
	}

	try
	{
		lua::execute(state, "test2.lua");
		while( 1 )
		{
			std::pair<int, std::string> n = lua::call(state, "test_call", 1, "haha", 10.2, false);
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		auto val = std::make_pair("test abc", 10.2);
		lua::call(state, "test_call2", 1, "haha", val);
	}
	catch( const luareg::fatal_error_t &e )
	{
		std::cout << e.what() << std::endl;
		e.dump(std::cout);
	}


	return 0;
}

