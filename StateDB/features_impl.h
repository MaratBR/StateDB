#pragma once
#include "config.h" 

#if STATEDB_FEATURE_RAM_MONITORING

#define _RAM_MONITORING_NS_BEGIN namespace statedb { namespace ram_monitoring {
#define _RAM_MONITORING_NS_END } }

_RAM_MONITORING_NS_BEGIN

template<typename...> constexpr void get_ram_size() {}

STATIC_SIGNATURE_CHECKER(get_size, size_t, const &U)

template<typename T>
constexpr size_t get_ram_size(const T& obj) noexcept
{
	return sizeof(T);
}

template<typename T, typename std::enable_if_t<has_sig_get_size<T>::value, int> = 0>
constexpr size_t get_ram_size(const T& obj) noexcept
{
	return T::get_size(obj);
}

_RAM_MONITORING_NS_END

#	define _RAM_MONITORING_FN_NAME get_ram_size
#	define RAM_MONITORING_IMPL(type) template<> size_t _RAM_MONITORING_FN_NAME <type>(const type & obj) noexcept

#else
#	define GET_OBJECT_SIZE(x) (size_t)(0)
#endif