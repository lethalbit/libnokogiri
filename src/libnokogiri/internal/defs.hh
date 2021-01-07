// SPDX-License-Identifier: LGPL-3.0-or-later
/* internal/defs.hh - Internal definitions for libnokogiri */
#pragma once
#if !defined(LIBNOKOGIRI_INTERNAL_DEFS_HH)
#define LIBNOKOGIRI_INTERNAL_DEFS_HH

#include <type_traits>
#include <variant>
#include <string_view>
#include <algorithm>

#if defined(_MSC_VER) && !defined(_WINDOWS)
#define _WINDOWS 1
#endif



#ifdef _WINDOWS
#	ifdef LIBNOKOGIRI_BUILD_INTERNAL
#		define LIBNOKOGIRI_CLS_API __declspec(dllexport)
#	else
#		define LIBNOKOGIRI_CLS_API __declspec(dllimport)
#	endif
#	define LIBNOKOGIRI_API extern LIBNOKOGIRI_CLS_API
#	define LIBNOKOGIRI_CLS_MAYBE_API
#else
#	define LIBNOKOGIRI_CLS_API __attribute__ ((visibility("default")))
#	define LIBNOKOGIRI_CLS_MAYBE_API LIBNOKOGIRI_CLS_API
#	define LIBNOKOGIRI_API extern LIBNOKOGIRI_CLS_API
#endif

#if defined(_WINDOWS)
#	define LIBNOKOGIRI_SWAP16(x) _byteswap_ushort(x)
#	define LIBNOKOGIRI_SWAP32(x) _byteswap_ulong(x)
#	define LIBNOKOGIRI_SWAP64(x) _byteswap_uint64(x)
#else
#	define LIBNOKOGIRI_SWAP16(x) __builtin_bswap16(x)
#	define LIBNOKOGIRI_SWAP32(x) __builtin_bswap32(x)
#	define LIBNOKOGIRI_SWAP64(x) __builtin_bswap64(x)
#endif

namespace libnokogiri::internal {
	using namespace std::literals::string_view_literals;

	template<typename T>
	struct has_nullable_ctor final {
		template<typename U>
		static std::true_type _ctor(decltype(U(std::nullptr_t()))*);
		template<typename U>
		static std::false_type _ctor(...);

		static const bool value = std::is_same<decltype(_ctor<T>(nullptr)), std::true_type>::value;
	};

	template<typename T>
	constexpr bool has_nullable_ctor_v = has_nullable_ctor<T>::value;

	template<typename T, typename U>
	using optional_pair = std::variant<std::monostate, T, U>;

	template<typename T>
	struct enum_pair_t final {
		using value_type = T;
	private:
		T _value;
		const std::string_view _name;
	public:
		constexpr enum_pair_t() noexcept : _value{}, _name{""sv}
			{ /* NOP */ }

		constexpr enum_pair_t(T v, const std::string_view n) noexcept :
			_value{v}, _name{n} { /* NOP */ }

		T value() const noexcept { return _value; }
		void value(const T value) noexcept { _value = value; }

		std::string_view name() const noexcept { return _name; }
		void name(const std::string_view name) noexcept { _name = name; }
	};

	template<typename map_t, typename value_t>
	std::string_view enum_name(map_t m, value_t v) noexcept {
		auto pos = std::find_if(
			std::begin(m), std::end(m),
			[&v](const typename map_t::value_type& t) {
				return (t.value() == v);
			});

		return pos != std::end(m) ? pos->name() : "?"sv;
	}

}

#endif /* LIBNOKOGIRI_INTERNAL_DEFS_HH */
