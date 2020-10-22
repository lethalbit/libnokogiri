// SPDX-License-Identifier: LGPL-3.0-or-later
/* internal/defs.hh - Internal definitions for libnokogiri */
#pragma once
#if !defined(LIBNOKOGIRI_INTERNAL_DEFS_HH)
#define LIBNOKOGIRI_INTERNAL_DEFS_HH

#include <type_traits>
#include <variant>

#if defined(_MSC_VER) && !defined(_WINDOWS)
#define _WINDOWS 1
#endif

#if !defined(__has_cpp_attribute)
#	define __has_cpp_attribute(x) 0
#endif

#if __has_cpp_attribute(maybe_unused) || __cplusplus >= 201703L
#	define LIBNOKOGIRI_NOWARN_UNUSED(x) [[maybe_unused]] x
#elif defined(__GNUC__)
#	define LIBNOKOGIRI_NOWARN_UNUSED(x) x __attribute__((unused))
#else
#	define LIBNOKOGIRI_NOWARN_UNUSED(x) x
#endif

#if __has_cpp_attribute(nodiscard) || __cplusplus >= 201402L
#	define LIBNOKOGIRI_NO_DISCARD(...) [[nodiscard]] __VA_ARGS__
#elif defined(__GNUC__)
#	define LIBNOKOGIRI_NO_DISCARD(...) __VA_ARGS__ __attribute__((warn_unused_result))
#else
#	define LIBNOKOGIRI_NO_DISCARD(...) __VA_ARGS__
#endif

#if __cplusplus >= 201103L
#	define ALIGN(X) alignas(x)
#elif defined(__GNUC__)
#	define LIBNOKOGIRI_ALIGN(X) __attribute__ ((aligned (X)))
#else
#	define LIBNOKOGIRI_ALIGN(X)
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

#if __cplusplus >= 201402L
#	define LIBNOKOGIRI_DEPRECATE_R(reson) [[deprecated(reson)]]
#	define LIBNOKOGIRI_DEPRECATE() [[deprecated]]
#else
#	ifdef _WINDOWS
#		define LIBNOKOGIRI_DEPRECATE_R(reson) __declspec(deprecated(reson))
#		define LIBNOKOGIRI_DEPRECATE() __declspec(deprecated)
#	else
#		define LIBNOKOGIRI_DEPRECATE_R(reson) [[gnu::deprecated(reason)]]
#		define LIBNOKOGIRI_DEPRECATE() __attribute__ ((deprecated))
#	endif
#endif

namespace libnokogiri::internal {
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
}

#endif /* LIBNOKOGIRI_INTERNAL_DEFS_HH */
