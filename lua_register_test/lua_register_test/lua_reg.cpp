#include "lua_reg.hpp"

#include "multithread/tls.hpp"


namespace lua {

	template < std::uint32_t N >
	struct callback_t
	{
		static std::int32_t handler(lua_State *state)
		{
			auto iter = register_instance().handler_map_.find(N);
			assert(iter != register_instance().handler_map_.end());
			
			return iter->second->handle(state);
		}
	};


	template < std::uint32_t N >
	struct auto_register_callback_t
	{
		static void reg(register_t::callback_map_t &callback_map)
		{
			callback_map[N] = &callback_t<N>::handler;
			auto_register_callback_t<N - 1>::reg(callback_map);
		}
	};

	template <>
	struct auto_register_callback_t<0>
	{
		static void reg(register_t::callback_map_t &callback_map)
		{
			callback_map[0] = &callback_t<0>::handler;
		}
	};


	register_t::register_t()
	{
		auto_register_callback_t<MAX_CALLBACK_NUM>::reg(callback_map_);
		callback_index_ = 0;
	}

	multi_thread::tls_ptr_t<register_t> tls_reg_val;
	register_t &register_instance()
	{
		if( tls_reg_val == nullptr )
			tls_reg_val = new register_t;

		return *tls_reg_val;
	}
}