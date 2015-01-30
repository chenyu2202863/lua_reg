#ifndef __LUA_REG_CONFIG_HPP
#define __LUA_REG_CONFIG_HPP

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
};

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <tuple>

namespace luareg
{

#ifndef _countof
	template <typename _CountofType, size_t _SizeOfArray>
	char(*__countof_helper(_CountofType(&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) (sizeof(*__countof_helper(_Array)) + 0)
#endif
}

#endif