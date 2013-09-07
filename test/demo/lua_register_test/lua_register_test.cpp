// lua_register_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <lua_reg/lua_reg.hpp>

#include <iterator>
#include <iostream>
#include <comutil.h>

#pragma comment(lib, "comsuppw.lib")
namespace lua = luareg;


void test0()
{}



void test1(int n)
{
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
	char test6(bool suc)
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

	void print()
	{
		std::cout << __FUNCTION__ << std::endl;
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
		lua::reg(state, "test0", &test0);
		lua::reg(state, "test1", &test1);
		lua::reg(state, "test2", &test2);
		lua::reg(state, "test3", &test3);
		lua::reg(state, "test4", &test4);
		lua::reg(state, "test5", &test5);

		test_t t;
		lua::reg(state, "test6", std::ref(t), &test_t::test6);
		lua::reg(state, "test7", &test7);
		lua::reg(state, "test8", &test8);
		lua::reg(state, "test9", &test9);
		lua::reg(state, "test10", &test10);
		lua::reg(state, "test11", std::ref(t), &test_t::test11);
		lua::reg(state, "test12", &test12);
		lua::reg(state, "test13", &test13);
		lua::reg(state, "test14", &test14);
		lua::reg(state, "test15", &test15);
		lua::reg(state, "test16", &test16);
		lua::reg(state, "test17", &test17);
		lua::reg(state, "test18", &test18);
		lua::reg(state, "test19", [](int n)->int{ return n; });
		lua::reg(state, "test20", &test20);


		luareg::class_t<foo_t>(state, "foo_t")
			<< luareg::constructor<int>()
			<< luareg::destructor()
			<< luareg::def<1>("add", &foo_t::add)
			<< luareg::def<2>("get", &foo_t::get)
			<< luareg::def<3>("get_pointer", &foo_t::get_pointer)
			<< luareg::def<4>("get_base", &foo_t::get_base);


		luareg::class_t<base_t>(state, "base_t")
			<< luareg::def<1>("print", &base_t::print);

		lua::execute(state, "test.lua");
	}
	catch (const luareg::parameter_error_t &e)
	{
		std::cout << e.what() << std::endl;
		e.dump(std::cout);
	}
	catch(const luareg::fatal_error_t &e)
	{
		std::cout << e.what() << std::endl;
		e.dump(std::cout);
	}

	try
	{
		lua::execute(state, "test2.lua");
		std::pair<int, std::string> n = lua::call(state, "test_call", 1, "haha", 10.2, false);

		auto val = std::make_pair("test abc", 10.2);
		lua::call(state, "test_call2", 1, "haha", val);
	}
	catch(const luareg::fatal_error_t &e)
	{
		std::cout << e.what() << std::endl;
		e.dump(std::cout);
	}


	system("pause");
	return 0;
}

