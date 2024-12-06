//bit14.h

#pragma once

/*=======================================================================
=========================================================================
||| This is a recreation of the c++20 bit header written in ISO C++14.
||| https://en.cppreference.com/w/cpp/header/bit
|||	This project is intended for anyone who wishes to use the standard
|||	library header bit.h but is stuck on c++14 or c++17.
|||	Simply add the folder containing this header to the list of include
|||	directories in your project and #inclde header bit14.h where desired.
||| On msvc, gcc, clang, IBM XL / Open XL for Linux or AIX,
|||	or Intel ICC / ICPX compilers it uses x86, ARM or Power intrinsics
|||	whenever available. Otherwise, it falls back on a generic solution.
|||	On other compilers, a fully generic solution is used.
||| All function descriptions on cppreference.com apply exactly to
||| the functions in this header with the following exceptions:
|||
||| * All functions and enums in this header exist within namespace bit14.
|||	* Extended integer types are not supported.
||| * The target platform must use the ILP32, LLP64 or LP64 data model,
|||		meaning char = 1 byte, short = 2 bytes, int = 4 bytes,
|||		long = 4 or 8 bytes and long long = 8 bytes.
||| * Every function in this header other than bit14::bit_cast has a
|||		seperate constexpr version that takes its arguments as template
|||		parameters (see below). bit14::bit_cast has no constexpr version.
||| * The endian enum only exists on gcc, clang, msvc, IBM XL / Open XL
|||		for Linux or AIX and Intel ICC / ICPX compilers.
||| * If using c++14, bit14::byteswap does not check for padding bits.
|||	* bit14::bitceil is noexcept
|||
||| ====================================================================
|||	==		    		--- constexpr functions --		    		  ==
||| ====================================================================
|||		The interface for all constexpr functions are listed below:
|||
|||		template <typename T, T Value>
|||		constexpr int bit14::popcount() noexcept;
|||
|||		template <typename T, T Value>
|||		constexpr int bit14::countr_zero() noexcept;
|||
|||		template <typename T, T Value>
|||		constexpr int bit14::countl_zero() noexcept;
|||
|||		template <typename T, T Value>
|||		constexpr int bit14::countr_one() noexcept;
|||
|||		template <typename T, T Value>
|||		constexpr int bit14::countl_one() noexcept;
|||
|||		template <typename T, T Value, int Shift>
|||		constexpr T bit14::rotl() noexcept;
|||
|||		template <typename T, T Value, int Shift>
|||		constexpr T bit14::rotr() noexcept;
|||
|||		template <typename T, T Value>
|||		constexpr int bit14::bit_width() noexcept;
|||
|||		template <typename T, T Value>
|||		constexpr T bit14::bit_floor() noexcept;
|||
|||		template <typename T, T Value>
|||		constexpr T bit14::bit_ceil() noexcept;
|||
|||		template <typename T, T Value>
|||		constexpr bool bit14::has_single_bit() noexcept;
|||
|||		template<typename T, T Value>
|||		constexpr T bit14::byteswap() noexcept;
=========================================================================
=========================================================================*/

#include <cstring>			//memcpy
#include "bit14_detail.h"
#include "bit14_preprocessor.h"

#ifdef BIT14_USING_MSVC
#include <intrin.h>
#include <stdlib.h>	//_rotl, _rotl64, _rotr, _rotr64
#endif

namespace bit14
{
	/*=============================================
	||   vvvv    Start of msvc section    vvvv   ||
	===============================================*/
#ifdef BIT14_USING_MSVC
	enum class endian
	{
		little = 0,
		big = 1,
		native = little
	};

#ifdef __AVX2__
#define BIT14_HAS_ABM_INTRINSICS
#endif

#ifdef BIT14_HAS_ABM_INTRINSICS
#undef BIT14_POPCOUNT_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	int popcount(const T value) noexcept
	{
		return detail::popcount_abm(value);
	}

#elif defined(BIT14_MIGHT_HAVE_ABM_INTRINSICS)
#undef BIT14_POPCOUNT_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	int popcount(const T value) noexcept
	{
		const bool has_popcnt = detail::cpu_info.has_popcnt();

		if (has_popcnt)
			return detail::popcount_abm(value);
		else
			return detail::popcount_fallback(value);
	}
#endif

#ifdef BIT14_HAS_BMI_INTRINSICS
#undef BIT14_COUNTR_ZERO_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	int countr_zero(const T value) noexcept
	{
		return detail::countr_zero_bmi(value);
	}

#elif defined(BIT14_HAS_BSF_INTRINSICS) && defined(BIT14_MIGHT_HAVE_BMI_INTRINSICS)
#undef BIT14_COUNTR_ZERO_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	int countr_zero(const T value) noexcept
	{
		const bool has_bmi = detail::cpu_info.has_bmi();

		if (has_bmi)
			return detail::countr_zero_bmi(value);
		else
			return detail::countr_zero_bsf(value);
	}

#elif defined(BIT14_HAS_BSF_INTRINSICS)
#undef BIT14_COUNTR_ZERO_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	int countr_zero(const T value) noexcept
	{
		return detail::countr_zero_bsf(value);
	}
#endif

#ifdef BIT14_HAS_ABM_INTRINSICS
#undef BIT14_COUNTL_ZERO_FALLBACK
	template <typename T, detail::use_if_bit14_type<T> = true>
	int countl_zero(const T value) noexcept
	{
		return detail::countl_zero_lzcnt(value);
	}

#elif defined(BIT14_HAS_BSR_INTRINSICS) && defined(BIT14_MIGHT_HAVE_ABM_INTRINSICS)
#undef BIT14_COUNTL_ZERO_FALLBACK
	template <typename T, detail::use_if_bit14_type<T> = true>
	int countl_zero(const T value) noexcept
	{
		const bool has_lzcnt = detail::cpu_info.has_lzcnt();

		if (has_lzcnt)
			return detail::countl_zero_lzcnt(value);
		else
			return detail::countl_zero_bsr(value);
	}

#elif defined(BIT14_HAS_BSR_INTRINSICS)
#undef BIT14_COUNTL_ZERO_FALLBACK
	template <typename T, detail::use_if_bit14_type<T> = true>
	int countl_zero(const T value) noexcept
	{
		return detail::countl_zero_bsr(value);
	}
#endif

#if defined(_WIN64) || defined(_M_IX86) || defined(_M_ARM)
#undef BIT14_ROTL_FALLBACK
#undef BIT14_ROTR_FALLBACK
#undef BIT14_BYTESWAP_FALLBACK

#if defined(_M_ARM) || defined(_M_ARM64)
#undef BIT14_POPCOUNT_FALLBACK
#undef BIT14_COUNTR_ZERO_FALLBACK
#undef BIT14_COUNTL_ZERO_FALLBACK

	template <typename T, use_if_bit14_type<T> = true>
	int popcount(const T value) noexcept
	{
		return detail::popcount_arm(value);
	}

	template <typename T, use_if_bit14_type<T> = true>
	int countr_zero(const T value) noexcept
	{
		return detail::countr_zero_arm(value);
	}

	template <typename T, use_if_bit14_type<T> = true>
	int countl_zero(const T value) noexcept
	{
		return detail::countl_zero_arm(value);
	}

#endif //end of #if defined(_M_ARM) || defined(_M_ARM64)

	unsigned char rotl(const unsigned char value, int shift) noexcept
	{
		const bool is_neg = shift < 0;
		shift = shift % numeric_limits<unsigned char>::digits;

		if (is_neg)
			return _rotr8(value, static_cast<unsigned char>(-shift));
		else
			return _rotl8(value, static_cast<unsigned char>(shift));
	}

	unsigned short rotl(const unsigned short value, int shift) noexcept
	{
		const bool is_neg = shift < 0;
		shift = shift % numeric_limits<unsigned short>::digits;;

		if (is_neg)
			return _rotr16(value, static_cast<unsigned char>(-shift));
		else
			return _rotl16(value, static_cast<unsigned char>(shift));
	}

	unsigned char rotr(const unsigned char value, int shift) noexcept
	{
		const bool is_neg = shift < 0;
		shift = shift % numeric_limits<unsigned char>::digits;

		if (is_neg)
			return _rotl8(value, static_cast<unsigned char>(-shift));
		else
			return _rotr8(value, static_cast<unsigned char>(shift));
	}

	unsigned short rotr(const unsigned short value, int shift) noexcept
	{
		const bool is_neg = shift < 0;
		shift = shift % numeric_limits<unsigned short>::digits;;

		if (is_neg)
			return _rotl16(value, static_cast<unsigned char>(-shift));
		else
			return _rotr16(value, static_cast<unsigned char>(shift));
	}
#endif //end of #if defined(_WIN64) || defined(_M_IX86) || defined(_M_ARM)

	unsigned int rotl(const unsigned int value, int shift) noexcept
	{
		return _rotl(value, shift);
	}

	unsigned long rotl(const unsigned long value, int shift) noexcept
	{
		return _lrotl(value, shift);
	}

	//_rotl64 is still valid in x86 mode
	unsigned long long rotl(const unsigned long long value, int shift) noexcept
	{
		return _rotl64(static_cast<unsigned __int64>(value), shift);
	}

	unsigned int rotr(const unsigned int value, int shift) noexcept
	{
		return _rotr(value, shift);
	}

	unsigned long rotr(const unsigned long value, int shift) noexcept
	{
		return _lrotr(value, shift);
	}

	//_rotr64 is still valid in x86 mode
	unsigned long long rotr(const unsigned long long value, int shift) noexcept
	{
		return _rotr64(static_cast<unsigned __int64>(value), shift);
	}

	template <typename T, use_if_16_bit_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		detail::check_padding_bits<T>();
		const unsigned short result = _byteswap_ushort(static_cast<unsigned short>(value));
		return static_cast<T>(result);
	}

	template <typename T, use_if_32_bit_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		detail::check_padding_bits<T>();
		const unsigned long result = _byteswap_ulong(static_cast<unsigned long>(value));
		return static_cast<T>(result);
	}

	//64 bit still works in x86 mode
	template <typename T, use_if_64_bit_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		detail::check_padding_bits<T>();
		const unsigned __int64 result = _byteswap_uint64(static_cast<unsigned __int64>(value));
		return static_cast<T>(result);
	}
#endif //end of #ifdef BIT14_USING_MSVC

	/*===================================================
	||      ^^^^    End of msvc section    ^^^^		   ||
	||-------------------------------------------------||
	||	vvvv    Start of ICC / ICPX section     vvvv    ||
	===================================================*/

#if defined(BIT14_USING_ICC) || defined(BIT14_USING_ICPX)
#undef BIT14_COUNTL_ZERO_FALLBACK
#undef BIT14_POPCOUNT_FALLBACK
#undef BIT14_ROTL_FALLBACK
#undef BIT14_ROTR_FALLBACK
#undef BIT14_BYTESWAP_FALLBACK

#if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
	enum class endian
	{
		little = __ORDER_LITTLE_ENDIAN__,
		big = __ORDER_BIG_ENDIAN__,
		native = __BYTE_ORDER__
	};

#else
	enum class endian
	{
		little = 0,
		big = 1,
		native = little
	};

#endif

	template <typename T, use_if_bit14_type<T> = true>
	int popcount(const T value) noexcept
	{
		return detail::popcount(value);
	}

#ifdef __AVX2__
#define BIT14_HAS_BMI_INTRINSICS
#endif

#ifdef BIT14_USING_X86
#define BIT14_MIGHT_HAVE_BMI_INTRINSICS
#define BIT14_HAS_BSF_INTRINSICS
#define BIT14_HAS_BSR_INTRINSICS
#endif

#ifdef BIT14_HAS_BMI_INTRINSICS
#undef BIT14_COUNTR_ZERO_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	int countr_zero(const T value) noexcept
	{
		return detail::countr_zero_bmi(value);
	}
#elif defined(BIT14_MIGHT_HAVE_BMI_INTRINSICS) && defined(BIT14_HAS_BSF_INTRINSICS)
#undef BIT14_COUNTR_ZERO_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	int countr_zero(const T value) noexcept
	{
		const bool has_bmi = detail::cpu_info.has_bmi();

		if (has_bmi)
			return detail::countr_zero_bmi(value);
		else
			return detail::countr_zero_bsf(value);
	}
#elif defined(BIT14_HAS_BSF_INTRINSICS)
#undef BIT14_COUNTR_ZERO_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	int countr_zero(const T value) noexcept
	{
		return detail::countr_zero_bsf(value);
	}
#endif

	template <typename T, use_if_bit14_type<T> = true>
	int countl_zero(const T value) noexcept
	{
		return detail::countl_zero(value);
	}

	unsigned char rotl(const unsigned char value, const int shift) noexcept
	{
		return detail::rotl_fallback(value, shift);
	}

	unsigned short rotl(const unsigned short value, const int shift) noexcept
	{
		return detail::rotl_fallback(value, shift);
	}

	unsigned int rotl(const unsigned int value, const int shift) noexcept
	{
		return _rotl(value, shift);
	}

	unsigned long rotl(const unsigned long value, const int shift) noexcept
	{
		return _lrotl(value, shift);
	}

	unsigned long long rotl(const unsigned long long value, const int shift) noexcept
	{
		const unsigned __int64 result = _rotl64(static_cast<unsigned __int64>(value), shift);
		return static_cast<unsigned long long>(result);
	}

	unsigned char rotr(const unsigned char value, const int shift) noexcept
	{
		return detail::rotr_fallback(value, shift);
	}

	unsigned short rotr(const unsigned short value, const int shift) noexcept
	{
		return detail::rotr_fallback(value, shift);
	}

	unsigned int rotr(const unsigned int value, const int shift) noexcept
	{
		return _rotr(value, shift);
	}

	unsigned long rotr(const unsigned long value, const int shift) noexcept
	{
		return _lrotr(value, shift);
	}

	unsigned long long rotr(const unsigned long long value, const int shift) noexcept
	{
		const unsigned __int64 result = _rotr64(static_cast<unsigned __int64>(value), shift);
		return static_cast<unsigned long long>(result);
	}

	template <typename T, use_if_16_bit_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		return detail::byteswap_fallback(value);
	}

	template <typename T, use_if_32_bit_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		detail::check_padding_bits<T>();
		const int result = _bswap(static_cast<int>(value));
		return static_cast<T>(result);
	}

	template <typename T, use_if_64_bit_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		detail::check_padding_bits<T>();
		const __int64 result = _bswap64(static_cast<__int64>(value));
		return static_cast<T>(result);
	}
#endif //end of #if defined(BIT14_USING_ICC) || defined(BIT14_USING_ICPX)

	/*====================================================
	||	   ^^^^    End of ICC / ICPX section      ^^^^  ||
	||--------------------------------------------------||
	||	 vvvv    Start of gcc / clang section    vvvv   ||
	====================================================*/
#if defined(BIT14_USING_CLANG) || defined(BIT14_USING_GCC)
	enum class endian
	{
		little = __ORDER_LITTLE_ENDIAN__,
		big = __ORDER_BIG_ENDIAN__,
		native = __BYTE_ORDER__
	};

#if defined(BIT14_USING_X86) || defined(BIT14_USING_ARM)
#undef BIT14_POPCOUNT_FALLBACK
#undef BIT14_COUNTR_ZERO_FALLBACK
#undef BIT14_COUNTL_ZERO_FALLBACK
#undef BIT14_BYTESWAP_FALLBACK

	int popcount(const unsigned char value) noexcept
	{
		return __builtin_popcount(static_cast<unsigned int>(value));
	}

	int popcount(const unsigned short value) noexcept
	{
		return __builtin_popcount(static_cast<unsigned int>(value));
	}

	int popcount(const unsigned int value) noexcept
	{
		return __builtin_popcount(value);
	}

	int popcount(const unsigned long value) noexcept
	{
		return __builtin_popcountl(value);
	}

	int popcount(const unsigned long long value) noexcept
	{
		return __builtin_popcountll(value);
	}

	template<typename T, use_if_unsigned_char<T> = true>
	int countr_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		constexpr T mask = static_cast<T>(-1);
		const unsigned int result = __builtin_ctz(static_cast<unsigned int>((~mask) | value));
		return static_cast<int>(result);
	}

	template<typename T, use_if_unsigned_short<T> = true>
	int countr_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		constexpr T mask = static_cast<T>(-1);
		const unsigned int result = __builtin_ctz(static_cast<unsigned int>((~mask) | value));
		return static_cast<int>(result);
	}

	template<typename T, use_if_unsigned_int<T> = true>
	int countr_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		return __builtin_ctz(value);
	}

	template<typename T, use_if_unsigned_long<T> = true>
	int countr_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		return __builtin_ctzl(value);
	}

	template<typename T, use_if_unsigned_long_long<T> = true>
	int countr_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		return __builtin_ctzll(value);
	}

	template<typename T, use_if_unsigned_char<T> = true>
	int countl_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		constexpr int diff = numeric_limits<unsigned int>::digits - numeric_limits<T>::digits;
		const int result = __builtin_clz(static_cast<unsigned int>(value));
		return result - diff;
	}

	template<typename T, use_if_unsigned_short<T> = true>
	int countl_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		constexpr int diff = numeric_limits<unsigned int>::digits - numeric_limits<T>::digits;
		const int result = __builtin_clz(static_cast<unsigned int>(value));
		return result - diff;
	}

	template<typename T, use_if_unsigned_int<T> = true>
	int countl_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		return __builtin_clz(value);
	}

	template<typename T, use_if_unsigned_long<T> = true>
	int countl_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		return __builtin_clzl(value);
	}

	template<typename T, use_if_unsigned_long_long<T> = true>
	int countl_zero(const T value) noexcept
	{
		if (value == 0)
			return numeric_limits<T>::digits;

		return __builtin_clzll(value);
	}

	template <typename T, use_if_16_bit_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		detail::check_padding_bits<T>();
		const uint16_t result = __builtin_bswap16(static_cast<uint16_t>(value));
		return static_cast<T>(result);
	}

	template <typename T, use_if_32_bit_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		detail::check_padding_bits<T>();
		const uint32_t result = __builtin_bswap32(static_cast<uint32_t>(value));
		return static_cast<T>(result);
	}

	template <typename T, use_if_64_bit_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		detail::check_padding_bits<T>();
		const uint64_t result = __builtin_bswap64(static_cast<uint64_t>(value));
		return static_cast<T>(result);
	}
#endif //end of #ifdef BIT14_USING_X86 || defined(BIT14_USING_ARM)
#endif //end of #if defined(BIT14_USING_CLANG) || defined(BIT14_USING_GCC)

	/*====================================================
	||    ^^^^    End of gcc / clang section    ^^^^    ||
	||--------------------------------------------------||
	||    vvvv    Start of clang only section    vvvv   ||
	====================================================*/

#ifdef BIT14_USING_CLANG
#undef BIT14_ROTL_FALLBACK
#undef BIT14_ROTR_FALLBACK

#if __has_builtin(__builtin_rotateright8) && __has_builtin(__builtin_rotateleft8)
	template<typename T, use_if_unsigned_char<T> = true>
	T rotl(const T value, int shift) noexcept
	{
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			return __builtin_rotateright8(value, static_cast<T>(-shift));
		else
			return __builtin_rotateleft8(value, static_cast<T>(shift));
	}

	template<typename T, use_if_unsigned_char<T> = true>
	T rotr(const T value, int shift) noexcept
	{
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			return __builtin_rotateleft8(value, static_cast<T>(-shift));
		else
			return __builtin_rotateright8(value, static_cast<T>(shift));
	}

#else
	template<typename T, use_if_unsigned_char<T> = true>
	T rotl(const T value, const int shift) noexcept
	{
		return detail::rotl_fallback(value, shift);
	}

	template<typename T, use_if_unsigned_char<T> = true>
	T rotr(const T value, const int shift) noexcept
	{
		return detail::rotr_fallback(value, shift);
	}

#endif

#if __has_builtin(__builtin_rotateright16) && __has_builtin(__builtin_rotateleft16)
	template<typename T, use_if_unsigned_short<T> = true>
	T rotl(const T value, int shift) noexcept
	{
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			return __builtin_rotateright16(value, static_cast<T>(-shift));
		else
			return __builtin_rotateleft16(value, static_cast<T>(shift));
	}

	template<typename T, use_if_unsigned_short<T> = true>
	T rotr(const T value, int shift) noexcept
	{
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			return __builtin_rotateleft16(value, static_cast<T>(-shift));
		else
			return __builtin_rotateright16(value, static_cast<T>(shift));
	}

#else
	template<typename T, use_if_unsigned_short<T> = true>
	T rotl(const T value, const int shift) noexcept
	{
		return detail::rotl_fallback(value, shift);
	}

	template<typename T, use_if_unsigned_short<T> = true>
	T rotr(const T value, const int shift) noexcept
	{
		return detail::rotr_fallback(value, shift);
	}

#endif

#if __has_builtin(__builtin_rotateright32) && __has_builtin(__builtin_rotateleft32)
	template <typename T, use_if_bit14_32_bit_type<T> = true>
	T rotl(const T value, int shift) noexcept
	{
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			return __builtin_rotateright32(value, static_cast<T>(-shift));
		else
			return __builtin_rotateleft32(value, static_cast<T>(shift));
	}

	template <typename T, use_if_bit14_32_bit_type<T> = true>
	T rotr(const T value, int shift) noexcept
	{
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			return __builtin_rotateleft32(value, static_cast<T>(-shift));
		else
			return __builtin_rotateright32(value, static_cast<T>(shift));
	}
#else

	template <typename T, use_if_bit14_32_bit_type<T> = true>
	T rotl(const T value, const int shift) noexcept
	{
		return detail::rotl_fallback(value, shift);
	}

	template <typename T, use_if_bit14_32_bit_type<T> = true>
	T rotr(const T value, const int shift) noexcept
	{
		return detail::rotr_fallback(value, shift);
	}
#endif

#if __has_builtin(__builtin_rotateright64) && __has_builtin(__builtin_rotateleft64)
	template <typename T, use_if_bit14_64_bit_type<T> = true>
	T rotl(const T value, int shift) noexcept
	{
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			return __builtin_rotateright64(value, static_cast<T>(-shift));
		else
			return __builtin_rotateleft64(value, static_cast<T>(shift));
	}

	template <typename T, use_if_bit14_64_bit_type<T> = true>
	T rotr(const T value, int shift) noexcept
	{
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			return __builtin_rotateleft64(value, static_cast<T>(-shift));
		else
			return __builtin_rotateright64(value, static_cast<T>(shift));
	}
#else
	template <typename T, use_if_bit14_64_bit_type<T> = true>
	T rotl(const T value, const int shift) noexcept
	{
		return detail::rotl_fallback(value, shift);
	}

	template <typename T, use_if_bit14_64_bit_type<T> = true>
	T rotr(const T value, const int shift) noexcept
	{
		return detail::rotr_fallback(value, shift);
	}

#endif
#endif //end of #ifdef BIT14_USING_CLANG
	/*======================================================
	||     ^^^^    End of clang only section    ^^^^      ||
	||----------------------------------------------------||
	||    vvvv    Start of IBM Open XL section    vvvv    ||
	======================================================*/

#if defined(BIT14_USING_OPEN_XL) || defined(BIT14_USING_XL)
#undef BIT14_POPCOUNT_FALLBACK
#undef BIT14_ROTL_FALLBACK
#undef BIT14_ROTR_FALLBACK
#undef BIT14_COUNTR_ZERO_FALLBACK
#undef BIT14_COUNTL_ZERO_FALLBACK

#if defined (__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
	enum class endian
	{
		little = 0,
		big = 1,
		native = big
	};
#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
	enum class endian
	{
		little = 0,
		big = 1,
		native = little
	};
#endif

#ifdef BIT14_USING_OPEN_XL
#define IBM_CTZ_32(x) __builtin_ctz(x)
#define IBM_CTZ_64(x) __builtin_ctzll(x)
#define IBM_CLZ_32(x) __builtin_clz(x)
#define IBM_CLZ_64(x) __builtin_clzll(x)
#define IBM_POPCNT_32(x) __builtin_popcount(x)
#define IBM_POPCNT_64(x) __builtin_popcountll(x)
#define IBM_ROTL_32(x, y) __builtin_rotateleft32(x, y)
#define IBM_ROTL_64(x, y) __builtin_rotateleft64(x, y)
#else
#define IBM_CTZ_32(x) __cnttz4(x)
#define IBM_CTZ_64(x) __cnttz8(x)
#define IBM_CLZ_32(x) __cntlz4(x)
#define IBM_CLZ_64(x) __cntlz8(x)
#define IBM_POPCNT_32(x) __popcnt4(x)
#define IBM_POPCNT_64(x) __popcnt8(x)
#define IBM_ROTL_32(x, y) __rotatel4(x, y)
#define IBM_ROTL_64(x, y) __rotatel8(x, y)
#endif

	int countr_zero(const unsigned char value) noexcept
	{
		constexpr unsigned char mask = static_cast<unsigned char>(-1);
		const unsigned int result = IBM_CTZ_32(static_cast<unsigned int>((~mask) | value));
		return static_cast<int>(result);
	}

	int countr_zero(const unsigned short value) noexcept
	{
		constexpr unsigned short mask = static_cast<unsigned short>(-1);
		const unsigned int result = IBM_CTZ_32(static_cast<unsigned int>((~mask) | value));
		return static_cast<int>(result);
	}

	template <typename T, use_if_bit14_32_bit_type<T> = true>
	int countr_zero(const T value) noexcept
	{
		const unsigned int result = IBM_CTZ_32(static_cast<unsigned int>(value));
		return static_cast<int>(result);
	}

	template <typename T, use_if_bit14_64_bit_type<T> = true>
	int countr_zero(const T value) noexcept
	{
		const unsigned int result = IBM_CTZ_64(static_cast<unsigned long long>(value));
		return static_cast<int>(result);
	}

	int countl_zero(const unsigned char value) noexcept
	{
		constexpr unsigned int diff = numeric_limits<unsigned int>::digits -
			numeric_limits<unsigned char>::digits;
		const unsigned int result = IBM_CLZ_32(static_cast<unsigned int>(value));
		return static_cast<int>(result) - diff;
	}

	int countl_zero(const unsigned short value) noexcept
	{
		constexpr unsigned int diff = numeric_limits<unsigned int>::digits -
			numeric_limits<unsigned short>::digits;
		const unsigned int result = IBM_CLZ_32(static_cast<unsigned int>(value));
		return static_cast<int>(result) - diff;
	}

	template <typename T, use_if_bit14_32_bit_type<T> = true>
	int countl_zero(const T value) noexcept
	{
		const unsigned int result = IBM_CLZ_32(static_cast<unsigned int>(value));
		return static_cast<int>(result);
	}

	template <typename T, use_if_bit14_64_bit_type<T> = true>
	int countl_zero(const T value) noexcept
	{
		const unsigned int result = IBM_CLZ_64(static_cast<unsigned long long>(value));
		return static_cast<int>(result);
	}

	int popcount(const unsigned char value) noexcept
	{
		return IBM_POPCNT_32(static_cast<unsigned int>(value));
	}

	int popcount(const unsigned short value) noexcept
	{
		return IBM_POPCNT_32(static_cast<unsigned int>(value));
	}

	template <typename T, use_if_bit14_32_bit_type<T> = true>
	int popcount(const T value) noexcept
	{
		return IBM_POPCNT_32(static_cast<unsigned int>(value));
	}

	template <typename T, use_if_bit14_64_bit_type<T> = true>
	int popcount(const T value) noexcept
	{
		return IBM_POPCNT_64(static_cast<unsigned long long>(value));
	}

	unsigned char rotl(const unsigned char value, const int shift) noexcept
	{
		return detail::rotl_fallback(value, shift);
	}

	unsigned short rotl(const unsigned short value, const int shift) noexcept
	{
		return detail::rotl_fallback(value, shift);
	}

	template <typename T, use_if_bit14_32_bit_type<T> = true>
	T rotl(const T value, int shift) noexcept
	{
		unsigned int result;

		if (shift < 0)
		{
			shift = numeric_limits<T>::digits + (shift % numeric_limits<T>::digits);
			result = IBM_ROTL_32(static_cast<unsigned int>(value), static_cast<unsigned int>(shift));
		}
		else
			result = IBM_ROTL_32(static_cast<unsigned int>(value), static_cast<unsigned int>(shift));

		return static_cast<T>(result);
	}

	template <typename T, use_if_bit14_64_bit_type<T> = true>
	T rotl(const T value, int shift) noexcept
	{
		unsigned long long result;

		if (shift < 0)
		{
			shift = numeric_limits<T>::digits + (shift % numeric_limits<T>::digits);
			result = IBM_ROTL_64(static_cast<unsigned long long>(value), static_cast<unsigned long long>(shift));
		}
		else
			result = IBM_ROTL_64(static_cast<unsigned long long>(value), static_cast<unsigned long long>(shift));

		return static_cast<T>(result);
	}

	unsigned char rotr(const unsigned char value, const int shift) noexcept
	{
		return detail::rotr_fallback(value, shift);
	}

	unsigned short rotr(const unsigned short value, const int shift) noexcept
	{
		return detail::rotr_fallback(value, shift);
	}

	template <typename T, use_if_bit14_32_bit_type<T> = true>
	T rotr(const T value, int shift) noexcept
	{
		unsigned int result;
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			result = IBM_ROTL_32(static_cast<unsigned int>(value), static_cast<unsigned int>(-shift));
		else
		{
			shift = numeric_limits<T>::digits - shift;
			result = IBM_ROTL_32(static_cast<unsigned int>(value), static_cast<unsigned int>(shift));
		}

		return static_cast<T>(result);
	}

	template <typename T, use_if_bit14_64_bit_type<T> = true>
	T rotr(const T value, const int shift) noexcept
	{
		unsigned long long result;
		shift = shift % numeric_limits<T>::digits;

		if (shift < 0)
			result = IBM_ROTL_64(static_cast<unsigned long long>(value), static_cast<unsigned long long>(-shift));
		else
		{
			shift = numeric_limits<T>::digits - shift;
			result = IBM_ROTL_64(static_cast<unsigned long long>(value), static_cast<unsigned long long>(shift));
		}

		return static_cast<T>(result);
	}

#undef IBM_CTZ_32
#undef IBM_CTZ_64
#undef IBM_CLZ_32
#undef IBM_CLZ_64
#undef IBM_POPCNT_32
#undef IBM_POPCNT_64
#undef IBM_ROTL_32
#undef IBM_ROTL_64
#endif //end of #if defined(BIT14_USING_OPEN_XL) || defined(BIT14_USING_XL)

	/*===================================================
	||    ^^^^    End of IBM Open XL section    ^^^^   ||
	||-------------------------------------------------||
	||     vvvv    Start of generic section    vvvv    ||
	===================================================*/

#ifdef BIT14_POPCOUNT_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	int popcount(const T value) noexcept
	{
		return detail::popcount_fallback(value);
	}
#endif //end of #ifdef BIT14_POPCOUNT_FALLBACK

#ifdef BIT14_COUNTR_ZERO_FALLBACK
	template <typename T, use_if_bit14_64_bit_type<T> = true>
	int countr_zero(T value) noexcept
	{
		const bool is_zero = (value == 0);
		value += is_zero;
		int right_zero_bits = numeric_limits<T>::digits * is_zero;

		while (value % 2 == 0)
		{
			int section_bits = 32;
			const uint32_t value_2 = (static_cast<uint32_t>(value)) & (-static_cast<int32_t>(value));

			if (value_2)
				--section_bits;
			if (value_2 & 0x0000FFFF)
				section_bits -= 16;
			if (value_2 & 0x00FF00FF)
				section_bits -= 8;
			if (value_2 & 0x0F0F0F0F)
				section_bits -= 4;
			if (value_2 & 0x33333333)
				section_bits -= 2;
			if (value_2 & 0x55555555)
				section_bits -= 1;

			right_zero_bits += section_bits;
			value = value >> section_bits;
		}

		return right_zero_bits;
	}

	template <typename T, use_if_bit14_32_bit_or_less_type<T> = true>
	int countr_zero(T value) noexcept
	{
		const bool is_zero = (value == 0);
		value += is_zero;
		int right_zero_bits = numeric_limits<T>::digits * is_zero;
		int section_bits = 32;
		const uint32_t value_2 = (static_cast<uint32_t>(value)) & (-static_cast<int32_t>(value));

		if (value_2)
			--section_bits;
		if (value_2 & 0x0000FFFF)
			section_bits -= 16;
		if (value_2 & 0x00FF00FF)
			section_bits -= 8;
		if (value_2 & 0x0F0F0F0F)
			section_bits -= 4;
		if (value_2 & 0x33333333)
			section_bits -= 2;
		if (value_2 & 0x55555555)
			section_bits -= 1;

		right_zero_bits += section_bits;
		return right_zero_bits;
	}
#endif //end of #ifdef BIT14_COUNTR_ZERO_FALLBACK

#ifdef BIT14_COUNTL_ZERO_FALLBACK
	int countl_zero(unsigned char value) noexcept
	{
		value |= (value >> 1);
		value |= (value >> 2);
		value |= (value >> 4);

		return numeric_limits<unsigned char>::digits - bit14::popcount(value);
	}

	int countl_zero(unsigned short value) noexcept
	{
		value |= (value >> 1);
		value |= (value >> 2);
		value |= (value >> 4);
		value |= (value >> 8);

		return numeric_limits<unsigned short>::digits - bit14::popcount(value);
	}

	template <typename T, use_if_bit14_32_bit_type<T> = true>
	int countl_zero(T value) noexcept
	{
		value |= (value >> 1);
		value |= (value >> 2);
		value |= (value >> 4);
		value |= (value >> 8);
		value |= (value >> 16);

		return numeric_limits<T>::digits - bit14::popcount(value);
	}

	template <typename T, use_if_bit14_64_bit_type<T> = true>
	int countl_zero(T value) noexcept
	{
		value |= (value >> 1);
		value |= (value >> 2);
		value |= (value >> 4);
		value |= (value >> 8);
		value |= (value >> 16);
		value |= (value >> 32);

		return numeric_limits<T>::digits - bit14::popcount(value);
	}
#endif //end of BIT14_COUNTL_ZERO_FALLBACK

#ifdef BIT14_ROTL_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	T rotl(const T value, const int shift) noexcept
	{
		return detail::rotl_fallback(value, shift);
	}
#endif //end of BIT14_ROTL_FALLBACK

#ifdef BIT14_ROTR_FALLBACK
	template <typename T, use_if_bit14_type<T> = true>
	T rotr(const T value, const int shift) noexcept
	{
		return detail::rotr_fallback(value, shift);
	}
#endif //end of BIT14_ROTR_FALLBACK

#ifdef BIT14_BYTESWAP_FALLBACK
	template<typename T, use_if_integral<T> = true>
	T byteswap(const T value) noexcept
	{
		return detail::byteswap_fallback(value);
	}
#endif //end of BIT14_BYTESWAP_FALLBACK

	/*===================================================================================
	||			   ^^^^    End of generic (non-intrinsic) section    ^^^^			   ||
	||---------------------------------------------------------------------------------||
	||   vvvv    These functions are used universally regardless of compiler    vvvv   ||
	===================================================================================*/

	template <typename T, use_if_bit14_type<T> = true>
	int countr_one(const T value) noexcept
	{
		return bit14::countr_zero<T>(~value);
	}

	template <typename T, use_if_bit14_type<T> = true>
	int countl_one(const T value) noexcept
	{
		return bit14::countl_zero<T>(~value);
	}

	template <typename T, use_if_bit14_type<T> = true>
	T bit_floor(const T value) noexcept
	{
		constexpr int digits = numeric_limits<T>::digits - 1;
		const bool is_zero = (value == 0);
		const int shift = digits + is_zero - bit14::countl_zero(value);
		return T{ !is_zero } << shift;
	}

	template <typename T, use_if_bit14_type<T> = true>
	T bit_ceil(const T value) noexcept
	{
		const bool not_zero = (value != 0);
		const int shift = numeric_limits<T>::digits - bit14::countl_zero(static_cast<T>(value - not_zero));
		assert(shift < numeric_limits<T>::digits);
		return T{ 1 } << shift;
	}

	template <typename T, use_if_bit14_type<T> = true>
	int bit_width(const T value) noexcept
	{
		return numeric_limits<T>::digits - bit14::countl_zero(value);
	}

	template<class To, class From, enable_if_types_are_trivally_copyable_and_same_size<To, From> = true>
	To bit_cast(const From& src) noexcept
	{
		static_assert(std::is_trivially_constructible<To>::value,
			"Library bit14 additionally requires "
				"destination type to be trivially constructible.\n");

		To dst;
		std::memcpy(&dst, &src, sizeof(To));
		return dst;
	}

	unsigned char byteswap(const unsigned char value) noexcept
	{
		return value;
	}

	template <typename T, use_if_bit14_type<T> = true>
	bool has_single_bit(const T value) noexcept
	{
		return value != 0 && (value & (value - 1)) == 0;
	}

	/*=====================================================================================
	||    ^^^^    These functions are used universally regardless of compiler    ^^^^    ||
	||-----------------------------------------------------------------------------------||
	||				      vvvv    Start of constexpr section    vvvv					 ||
	=====================================================================================*/

	template <typename T, T Value, detail::use_if_bit14_type<T> = true>
	constexpr int popcount() noexcept
	{
		constexpr T first_const = numeric_limits<T>::max() / T{ 3 };
		constexpr T second_const = numeric_limits<T>::max() / T{ 15 } * T{ 3 };
		constexpr T third_const = numeric_limits<T>::max() / T{ 255 } * T{ 15 };
		constexpr T fourth_const = numeric_limits<T>::max() / T{ 255 };
		constexpr int fifth_const = (sizeof(T) - 1) * numeric_limits<uint8_t>::digits;

		T v = Value;
		v = v - ((v >> 1) & first_const);
		v = (v & second_const) + ((v >> 2) & second_const);
		v = (v + (v >> 4)) & third_const;
		T result = static_cast<T>(v * fourth_const) >> fifth_const;
		return static_cast<int>(result);
	}

	template <typename T, T Value, detail::use_if_bit14_64_bit_type<T> = true>
	constexpr int countr_zero() noexcept
	{
		T value = Value;
		constexpr bool is_zero = (Value == 0);
		value += is_zero;
		int right_zero_bits = numeric_limits<T>::digits * is_zero;

		while (value % 2 == 0)
		{
			int section_bits = 32;
			const uint32_t value_2 = (static_cast<uint32_t>(value)) & (-static_cast<int32_t>(value));

			if (value_2)
				--section_bits;
			if (value_2 & 0x0000FFFF)
				section_bits -= 16;
			if (value_2 & 0x00FF00FF)
				section_bits -= 8;
			if (value_2 & 0x0F0F0F0F)
				section_bits -= 4;
			if (value_2 & 0x33333333)
				section_bits -= 2;
			if (value_2 & 0x55555555)
				section_bits -= 1;

			right_zero_bits += section_bits;
			value = value >> section_bits;
		}

		return right_zero_bits;
	}

	template <typename T, T Value, use_if_bit14_32_bit_or_less_type<T> = true>
	constexpr int countr_zero() noexcept
	{
		constexpr bool is_zero = (Value == 0);
		T value = Value;
		value += is_zero;
		int right_zero_bits = numeric_limits<T>::digits * is_zero;
		int section_bits = 32;
		const uint32_t value_2 = (static_cast<uint32_t>(value)) & (-static_cast<int32_t>(value));

		if (value_2)
			--section_bits;
		if (value_2 & 0x0000FFFF)
			section_bits -= 16;
		if (value_2 & 0x00FF00FF)
			section_bits -= 8;
		if (value_2 & 0x0F0F0F0F)
			section_bits -= 4;
		if (value_2 & 0x33333333)
			section_bits -= 2;
		if (value_2 & 0x55555555)
			section_bits -= 1;

		right_zero_bits += section_bits;
		return right_zero_bits;
	}

	template <typename T, T Value, use_if_unsigned_char<T> = true>
	constexpr int countl_zero() noexcept
	{
		T value = Value;
		value |= (value >> 1);
		value |= (value >> 2);
		value |= (value >> 4);

		return numeric_limits<T>::digits - bit14::popcount<T, value>();
	}

	template <typename T, T Value, use_if_unsigned_short<T> = true>
	constexpr int countl_zero() noexcept
	{
		constexpr T value = Value | (Value >> 1);
		constexpr T value2 = value | (value >> 2);
		constexpr T value3 = value2 | (value2 >> 4);
		constexpr T value4 = value3 | (value3 >> 8);

		return numeric_limits<T>::digits - bit14::popcount<T, value4>();
	}

	template <typename T, T Value, use_if_bit14_32_bit_type<T> = true>
	constexpr int countl_zero() noexcept
	{
		constexpr T value = Value | (Value >> 1);
		constexpr T value2 = value | (value >> 2);
		constexpr T value3 = value2 | (value2 >> 4);
		constexpr T value4 = value3 | (value3 >> 8);
		constexpr T value5 = value4 | (value4 >> 16);

		return numeric_limits<T>::digits - bit14::popcount<T, value>();
	}

	template <typename T, T Value, use_if_bit14_64_bit_type<T> = true>
	constexpr int countl_zero() noexcept
	{
		constexpr T value = Value | (Value >> 1);
		constexpr T value2 = value | (value >> 2);
		constexpr T value3 = value2 | (value2 >> 4);
		constexpr T value4 = value3 | (value3 >> 8);
		constexpr T value5 = value4 | (value4 >> 16);
		constexpr T value6 = value5 | (value5 >> 32);

		return numeric_limits<T>::digits - bit14::popcount<T, value>();
	}

	template <typename T, T Value, use_if_bit14_type<T> = true>
	constexpr int countl_one() noexcept
	{
		return countl_zero<T, ~Value>();
	}

	template <typename T, T Value, use_if_bit14_type<T> = true>
	constexpr int countr_one() noexcept
	{
		return countr_zero<T, ~Value>();
	}

	template <typename T, T Value, int Shift, use_if_bit14_type<T> = true>
	constexpr T rotl() noexcept
	{
		constexpr int shift = Shift % numeric_limits<T>::digits;
		T rs;
		T ls;

		if (Shift < 0)
		{
			rs = Value >> -shift;
			ls = Value << (numeric_limits<T>::digits + shift);
		}
		else
		{
			rs = Value >> (numeric_limits<T>::digits - shift);
			ls = Value << shift;
		}

		return ls + rs;
	}

	template <typename T, T Value, int Shift, use_if_bit14_type<T> = true>
	constexpr T rotr() noexcept
	{
		constexpr int shift = Shift % numeric_limits<T>::digits;
		T rs;
		T ls;

		if (shift < 0)
		{
			rs = Value >> (numeric_limits<T>::digits + shift);
			ls = Value << -shift;
		}
		else
		{
			rs = Value >> shift;
			ls = Value << (numeric_limits<T>::digits - shift);
		}

		return ls + rs;
	}

	template <typename T, T Value, use_if_bit14_type<T> = true>
	constexpr int bit_width() noexcept
	{
		return numeric_limits<T>::digits - countl_zero<T, Value>();
	}

	template <typename T, T Value, use_if_bit14_type<T> = true>
	constexpr T bit_floor() noexcept
	{
		constexpr int digits = numeric_limits<T>::digits - 1;
		constexpr bool is_zero = (Value == 0);
		constexpr int shift = digits + is_zero - countl_zero<T, Value>();
		return T{ !is_zero } << shift;
	}

	template <typename T, T Value, use_if_bit14_type<T> = true>
	constexpr T bit_ceil() noexcept
	{
		if (Value == 0)
			return 1;

		constexpr int shift = numeric_limits<T>::digits -
			countl_zero<T, static_cast<T>(Value - 1)>();

		static_assert(shift < numeric_limits<T>::digits,
			"Returned value cannot be represented by the return type.");

		return T{ 1 } << shift;
	}

	template <typename T, T Value, use_if_bit14_type<T> = true>
	constexpr bool has_single_bit() noexcept
	{
		return (popcount<T, Value>() == 1);
	}

	template<typename T, T Value, use_if_8_bit_integral<T> = true>
	constexpr T byteswap() noexcept
	{
		detail::check_padding_bits<T>();
		return Value;
	}

	template<typename T, T Value, use_if_16_bit_integral<T> = true>
	constexpr T byteswap() noexcept
	{
		detail::check_padding_bits<T>();
		return static_cast<unsigned short>((Value << 8) | (Value >> 8));
	}

	template<typename T, T Value, use_if_32_bit_integral<T> = true>
	constexpr T byteswap() noexcept
	{
		detail::check_padding_bits<T>();
		return (Value << 24) | ((Value << 8) & 0x00FF'0000) | ((Value >> 8) & 0x0000'FF00) | (Value >> 24);
	}

	template<typename T, T Value, use_if_64_bit_integral<T> = true>
	constexpr T byteswap() noexcept
	{
		detail::check_padding_bits<T>();
		return (Value << 56) | ((Value << 40) & 0x00FF'0000'0000'0000) | ((Value << 24) & 0x0000'FF00'0000'0000)
			| ((Value << 8) & 0x0000'00FF'0000'0000) | ((Value >> 8) & 0x0000'0000'FF00'0000)
			| ((Value >> 24) & 0x0000'0000'00FF'0000) | ((Value >> 40) & 0x0000'0000'0000'FF00) | (Value >> 56);
	}

	template<typename To, typename From, From Value>
	constexpr To bit_cast() noexcept
	{
		static_assert(detail::always_false<To>,
			"bit14::bit_cast does not support constant expressions.\n");

		return {};
	}

	/*==================================================
	||    ^^^^    End of constexpr section    ^^^^    ||
	==================================================*/
}//end namespace bit14

#undef BIT14_HAS_ABM_INTRINSICS
#undef BIT14_MIGHT_HAVE_BMI_INTRINSICS
#undef BIT14_HAS_BMI_INTRINSICS
#undef BIT14_HAS_BSF_INTRINSICS
#undef BIT14_HAS_BSR_INTRINSICS
#undef BIT14_MIGHT_HAVE_ABM_INTRINSICS
#undef BIT14_POPCOUNT_FALLBACK
#undef BIT14_COUNTR_ZERO_FALLBACK
#undef BIT14_COUNTL_ZERO_FALLBACK
#undef BIT14_ROTL_FALLBACK
#undef BIT14_ROTR_FALLBACK
#undef BIT14_BYTESWAP_FALLBACK

#include "bit14_preprocessor_cleanup.h"