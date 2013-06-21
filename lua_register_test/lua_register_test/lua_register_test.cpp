// lua_register_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdafx.h"

#include <iostream>
#include "lua_reg/lua_reg.hpp"



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

std::tuple<int, std::string, bool, std::pair<int, std::string>> test13(const std::tuple<int, std::string, bool, std::pair<int, std::string>> &val)
{
	return val;
}


std::map<int, std::string> test14(const std::map<int, std::string> &val)
{
	return val;
}


namespace lua = luareg;

int _tmain(int argc, _TCHAR* argv[])
{
	luareg::state_t state;

	{
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
		//lua::reg(state, "test11", std::ref(t), &test_t::test11);
		lua::reg(state, "test12", &test12);
		lua::reg(state, "test13", &test13);
		lua::reg(state, "test14", &test14);

		lua::execute(state, "test.lua");
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

