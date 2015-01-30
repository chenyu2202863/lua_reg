#ifndef __LUA_REG_STATE_HPP
#define __LUA_REG_STATE_HPP

#include "config.hpp"

#ifdef min
#undef min
#endif


namespace luareg {

	struct state_t
	{
		bool is_owner_ = false;
		lua_State *state_ = nullptr;
		std::uint64_t mem_size_ = 0;

		state_t() = default;

		template < typename AllocatorT >
		explicit state_t(const AllocatorT &allocator)
			: is_owner_(true)
			, state_(nullptr)
		{
			open(allocator);
		}

		explicit state_t(lua_State *state)
			: state_(state)
		{

		}

		state_t(const state_t &rhs)
			: is_owner_(false)
			, state_(rhs.state_)
			, mem_size_(rhs.mem_size_)
		{}


		state_t &operator=(const state_t &rhs)
		{
			assert(&rhs != this);
			is_owner_ = false;
			state_ = rhs.state_;
			mem_size_ = rhs.mem_size_;

			return *this;
		}

		~state_t()
		{
			close();
		}

	public:
		lua_State *get()
		{
			assert(state_ != nullptr);
			return state_;
		}

		operator lua_State*()
		{
			assert(state_ != nullptr);
			return state_;
		}

		operator lua_State*() const
		{
			assert(state_ != nullptr);
			return state_;
		}

		std::uint64_t use_mem_size() const
		{
			return mem_size_;
		}

		template < typename AllocatorT >
		void open(const AllocatorT &allocator)
		{
			assert(state_ == nullptr);

			struct wrapper_t
			{
				const AllocatorT &allocator_;
				std::uint64_t &mem_size_;
			};
			wrapper_t *wrapper = new wrapper_t{allocator, this->mem_size_};
			static const ::luaL_Reg lualibs[] = 
			{
				{"",				::luaopen_base},
				{LUA_LOADLIBNAME,	::luaopen_package},
				{LUA_TABLIBNAME,	::luaopen_table},
				{LUA_IOLIBNAME,		::luaopen_io},
				{LUA_OSLIBNAME,		::luaopen_os},
				{LUA_STRLIBNAME,	::luaopen_string},
				{LUA_MATHLIBNAME,	::luaopen_math},
				{LUA_DBLIBNAME,		::luaopen_debug}	
			};

			state_ = ::lua_newstate([](void *ud, void *ptr, std::size_t old_size, std::size_t new_size)->void *
			{
				assert(ud != nullptr);
				wrapper_t *wrapper = (wrapper_t *)ud;

				AllocatorT &alloc = (AllocatorT &)wrapper->allocator_;
				std::uint64_t &mem_size = wrapper->mem_size_;

				if( old_size != 0 && new_size != 0 )
				{
					mem_size += new_size;
					auto p = alloc.allocate(new_size);
					if( ptr != nullptr )
						std::memcpy(p, ptr, std::min(new_size, old_size));

					mem_size -= old_size;
					alloc.deallocate((char *)ptr, old_size);
					ptr = nullptr;

					return p;
				}

				if( new_size != 0 && old_size == 0 )
				{
					mem_size += new_size;
					auto p = alloc.allocate(new_size);
					return p;
				}

				if( old_size != 0 && new_size == 0 )
				{
					assert(ptr != nullptr);
					mem_size -= old_size;
					alloc.deallocate((char *)ptr, old_size);
					return nullptr;
				}

				assert(new_size == 0 && old_size == 0);

				return nullptr;
			}, wrapper);


			for( auto i = 0; i != _countof(lualibs); ++i )
			{
				::lua_pushcfunction(state_, lualibs[i].func);
				::lua_pushstring(state_, lualibs[i].name);
				::lua_call(state_, 1, 0);
			}
		}

		void close()
		{
			if( is_owner_ )
				::lua_close(state_);
		}
	};


	struct index_t
	{
		int idx_;

		explicit operator int( ) const
		{
			return idx_;
		}
	};
}

#endif