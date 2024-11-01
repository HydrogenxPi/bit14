//bit14_detail.h

#pragma once

/*=================================================================================
===================================================================================
|||	MIT License
|||
|||	Copyright (c) 2024, agrem44@gmail.com
|||
|||	Permission is hereby granted, free of charge, to any person obtaining a copy
|||	of this software and associated documentation files (the "Software"), to deal
|||	in the Software without restriction, including without limitation the rights
|||	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
|||	copies of the Software, and to permit persons to whom the Software is
|||	furnished to do so, subject to the following conditions:
|||
|||	The above copyright notice and this permission notice shall be included in all
|||	copies or substantial portions of the Software.
|||
|||	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
|||	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
|||	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
|||	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
|||	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
|||	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
|||	SOFTWARE.
===================================================================================
===================================================================================*/

#include <cstdint>			//uint8_t, uint16_t, uint32_t, uint64_t, int32_t
#include <type_traits>		//true_type, false_type, is_integral, conditional, is_trivially_copyable, is_same,
								//remove_cv, has_unique_object_representations_v
#include <limits>			//numeric_limits
#include <utility>			//swap
#include <array>			//array
#include "bit14_cpuinfo.h"
#include "bit14_preprocessor.h"

#define BIT14_POPCOUNT_FALLBACK
#define BIT14_COUNTR_ZERO_FALLBACK
#define BIT14_COUNTL_ZERO_FALLBACK
#define BIT14_ROTL_FALLBACK
#define BIT14_ROTR_FALLBACK
#define BIT14_BYTESWAP_FALLBACK

#ifdef BIT14_USING_MSVC
#include <intrin.h>
#endif

#if defined(BIT14_USING_ICC) || defined(BIT14_USING_ICPX)
#include <immintrin.h>
#endif

namespace bit14
{
	using std::numeric_limits;
	using std::uint8_t;
	using std::uint16_t;
	using std::uint32_t;
	using std::uint64_t;
	using std::int32_t;

	namespace detail
	{
		static_assert(sizeof(unsigned char) == 1,
			"Unsupported bit width of type unsigned char.\n");
		static_assert(sizeof(unsigned short) == 2,
			"Unsupported bit width of type unsigned short.\n");
		static_assert(sizeof(unsigned int) == 4,
			"Unsupported bit width of type unsigned int.\n");
		static_assert(sizeof(unsigned long) == 4 || sizeof(unsigned long) == 8,
			"Unsupported bit width of type unsigned long.\n");
		static_assert(sizeof(unsigned long long) == 8,
			"Unsupported bit width of type unsigned long long.\n");

		template <typename T>
		constexpr bool always_false = false;

		template <bool b>
		struct t_or_f_type : public std::conditional<b, std::true_type, std::false_type>::type {};

		template <typename T>
		struct is_bit14_type : public std::false_type {};

		template <>
		struct is_bit14_type<unsigned char> : public std::true_type {};

		template <>
		struct is_bit14_type<unsigned short> : public std::true_type {};

		template <>
		struct is_bit14_type<unsigned int> : public std::true_type {};

		template <>
		struct is_bit14_type<unsigned long> : public std::true_type {};

		template <>
		struct is_bit14_type<unsigned long long> : public std::true_type {};

		template <typename T>
		struct is_bit14_8_bit_type : public t_or_f_type<is_bit14_type<T>::value &&
			numeric_limits<T>::digits == 8>::type {};

		template <typename T>
		struct is_bit14_16_bit_type : public t_or_f_type<is_bit14_type<T>::value &&
			numeric_limits<T>::digits == 16>::type {};

		template <typename T>
		struct is_bit14_32_bit_type : public t_or_f_type<is_bit14_type<T>::value &&
			numeric_limits<T>::digits == 32>::type {};

		template <typename T>
		struct is_bit14_64_bit_type : public t_or_f_type<is_bit14_type<T>::value &&
			numeric_limits<T>::digits == 64>::type {};

		template <typename T, bool = numeric_limits<T>::is_signed>
		struct is_8_bit_int {};

		template <typename T>
		struct is_8_bit_int<T, true> : public t_or_f_type<numeric_limits<T>::digits == 7> {};

		template <typename T>
		struct is_8_bit_int<T, false> : public t_or_f_type<numeric_limits<T>::digits == 8> {};

		template <typename T, bool = numeric_limits<T>::is_signed>
		struct is_16_bit_int {};

		template <typename T>
		struct is_16_bit_int<T, true> : public t_or_f_type<numeric_limits<T>::digits == 15> {};

		template <typename T>
		struct is_16_bit_int<T, false> : public t_or_f_type<numeric_limits<T>::digits == 16> {};

		template <typename T, bool = numeric_limits<T>::is_signed>
		struct is_32_bit_int {};

		template <typename T>
		struct is_32_bit_int<T, true> : public t_or_f_type<numeric_limits<T>::digits == 31> {};

		template <typename T>
		struct is_32_bit_int<T, false> : public t_or_f_type<numeric_limits<T>::digits == 32> {};

		template <typename T, bool = numeric_limits<T>::is_signed>
		struct is_64_bit_int {};

		template <typename T>
		struct is_64_bit_int<T, true> : public t_or_f_type<numeric_limits<T>::digits == 63> {};

		template <typename T>
		struct is_64_bit_int<T, false> : public t_or_f_type<numeric_limits<T>::digits == 64> {};

		template <bool b>
		struct use_if {};

		template <>
		struct use_if<true>
		{
			using type = bool;
		};

		template <typename T>
		using use_if_bit14_type = typename use_if<is_bit14_type<T>::value>::type;

		template <typename T>
		using use_if_bit14_8_bit_type = typename use_if<is_bit14_8_bit_type<T>::value>::type;

		template <typename T>
		using use_if_bit14_16_bit_type = typename use_if<is_bit14_16_bit_type<T>::value>::type;

		template <typename T>
		using use_if_bit14_32_bit_type = typename use_if<is_bit14_32_bit_type<T>::value>::type;

		template <typename T>
		using use_if_bit14_64_bit_type = typename use_if<is_bit14_64_bit_type<T>::value>::type;

		template <typename T>
		using use_if_bit14_32_bit_or_less_type = typename use_if<is_bit14_32_bit_type<T>::value ||
			is_bit14_16_bit_type<T>::value || is_bit14_8_bit_type<T>::value>::type;

		template <typename T>
		using use_if_integral = typename use_if<std::is_integral<T>::value>::type;

		template <typename T>
		using use_if_8_bit_integral = typename use_if<std::is_integral<T>::value && is_8_bit_int<T>::value>::type;

		template <typename T>
		using use_if_16_bit_integral = typename use_if<std::is_integral<T>::value && is_16_bit_int<T>::value>::type;

		template <typename T>
		using use_if_32_bit_integral = typename use_if<std::is_integral<T>::value && is_32_bit_int<T>::value>::type;

		template <typename T>
		using use_if_64_bit_integral = typename use_if<std::is_integral<T>::value && is_64_bit_int<T>::value>::type;

		template <typename T>
		using use_if_non_standard_size_integral = typename use_if<std::is_integral<T>::value &&
			!is_8_bit_int<T>::value && !is_16_bit_int<T>::value &&
				!is_32_bit_int<T>::value && !is_64_bit_int<T>::value>::type;

		template <typename T>
		using use_if_unsigned_char = typename use_if<
			std::is_same<
				typename std::remove_cv<T>::type, unsigned char>::value>::type;

		template <typename T>
		using use_if_unsigned_short = typename use_if<
			std::is_same<
				typename std::remove_cv<T>::type, unsigned short>::value>::type;

		template <typename T>
		using use_if_unsigned_int = typename use_if<
			std::is_same<
				typename std::remove_cv<T>::type, unsigned int>::value>::type;

		template <typename T>
		using use_if_unsigned_long = typename use_if<
			std::is_same <
				typename std::remove_cv<T>::type, unsigned long>::value> ::type;

		template <typename T>
		using use_if_unsigned_long_long = typename use_if<
			std::is_same<
				typename std::remove_cv<T>::type, unsigned long long>::value>::type;

		template <typename To, typename From>
		using enable_if_types_are_trivally_copyable_and_same_size =
			typename use_if<
				std::is_trivially_copyable<From>::value &&
					std::is_trivially_copyable<To>::value &&
						sizeof(To) == sizeof(From)>::type;
	} //end namespace detail

	template<class To, class From, detail::enable_if_types_are_trivally_copyable_and_same_size<To, From>>
	To bit_cast(const From& src) noexcept;

	namespace detail
	{
		template <typename T>
		constexpr void check_padding_bits() noexcept
		{
#if	(__cplusplus >= 201703L) || (_MSVC_LANG >= 201703L)
			static_assert(std::has_unique_object_representations_v<T>,
				"Type bit14::byteswap argument may not have padding bits.\n");
#endif
		}

		template <typename T, use_if_bit14_type<T> = true>
		int popcount_fallback(T value) noexcept
		{
			constexpr T first_const = numeric_limits<T>::max() / T{ 3 };
			constexpr T second_const = numeric_limits<T>::max() / T{ 15 } * T{ 3 };
			constexpr T third_const = numeric_limits<T>::max() / T{ 255 } * T{ 15 };
			constexpr T fourth_const = numeric_limits<T>::max() / T{ 255 };
			constexpr int fifth_const = (sizeof(T) - 1) * numeric_limits<uint8_t>::digits;

			value = value - ((value >> 1) & first_const);
			value = (value & second_const) + ((value >> 2) & second_const);
			value = (value + (value >> 4)) & third_const;
			T result = static_cast<T>(value * fourth_const) >> fifth_const;

			return static_cast<int>(result);
		}

		template<typename T, use_if_integral<T> = true>
		T byteswap_fallback(const T value) noexcept
		{
			detail::check_padding_bits<T>();

			auto bit_array = bit14::bit_cast<std::array<unsigned char, sizeof(T)>>(value);
			auto left_it = bit_array.begin();
			auto right_it = bit_array.end() - 1;

			while (left_it < right_it)
			{
				std::swap(*left_it, *right_it);
				++left_it;
				--right_it;
			}

			return bit14::bit_cast<T, std::array<unsigned char, sizeof(T)>>(bit_array);
		}

		template <typename T, use_if_bit14_type<T> = true>
		T rotl_fallback(const T value, int shift) noexcept
		{
			shift = shift % numeric_limits<T>::digits;
			T rs;
			T ls;

			if (shift < 0)
			{
				rs = value >> -shift;
				ls = value << (numeric_limits<T>::digits + shift);
			}
			else
			{
				rs = value >> (numeric_limits<T>::digits - shift);
				ls = value << shift;
			}

			return ls + rs;
		}

		template <typename T, use_if_bit14_type<T> = true>
		T rotr_fallback(const T value, int shift) noexcept
		{
			shift = shift % numeric_limits<T>::digits;
			T rs;
			T ls;

			if (shift < 0)
			{
				rs = value >> (numeric_limits<T>::digits + shift);
				ls = value << -shift;
			}
			else
			{
				rs = value >> shift;
				ls = value << (numeric_limits<T>::digits - shift);
			}

			return ls + rs;
		}

#if defined(BIT14_USING_MSVC) || defined(BIT14_USING_ICC) || defined(BIT14_USING_ICPX)
		const detail::bit14_cpu_info cpu_info;
#endif

		/*=============================================
		||   vvvv  Start of msvc intrinsics   vvvv   ||
		==============================================*/

#if defined(BIT14_USING_MSVC)
#if defined(BIT14_USING_X86) 
#if defined(_M_IX86) || defined(_M_ARM) || defined(_WIN64)
#define BIT14_HAS_BSF_INTRINSICS
#endif

#if	defined(_M_IX86) || (defined(_WIN64) && !defined(_M_ARM64) && !defined(_M_ARM64EC))
#define BIT14_MIGHT_HAVE_BMI_INTRINSICS
#endif

#ifdef __AVX2__
#define BIT14_HAS_BMI_INTRINSICS
#endif

#ifdef BIT14_MIGHT_HAVE_BMI_INTRINSICS
		int countr_zero_bmi(const unsigned char value) noexcept
		{
			constexpr unsigned char mask = static_cast<unsigned char>(-1);
			const unsigned int result = _tzcnt_u32(static_cast<unsigned int>((~mask) | value));
			return static_cast<int>(result);
		}

		int countr_zero_bmi(const unsigned short value) noexcept
		{
			constexpr unsigned short mask = static_cast<unsigned short>(-1);
			const unsigned int result = _tzcnt_u32(static_cast<unsigned int>((~mask) | value));
			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countr_zero_bmi(const T value) noexcept
		{
			const unsigned int result = _tzcnt_u32(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}

#ifdef BIT14_USING_64BIT 
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countr_zero_bmi(const T value) noexcept
		{
			const unsigned __int64 result = _tzcnt_u64(static_cast<unsigned __int64>(value));
			return static_cast<int>(result);
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countr_zero_bmi(const T value) noexcept
		{
			const uint32_t low_bits = static_cast<uint32_t>(value);

			if (low_bits)
			{
				const unsigned int result = _tzcnt_u32(static_cast<unsigned int>(low_bits));
				return static_cast<int>(result);
			}

			const uint32_t high_bits = static_cast<uint32_t>(value >> 32);
			const unsigned int result = _tzcnt_u32(static_cast<unsigned int>(high_bits));
			return static_cast<int>(result) + 32;
		}
#endif //end of #ifdef BIT14_USING_64BIT
#endif //end of #ifdef BIT14_MIGHT_HAVE_BMI_INTRINSICS

#ifdef BIT14_HAS_BSF_INTRINSICS
		int countr_zero_bsf(const unsigned char value) noexcept
		{
			constexpr unsigned long mask = ~static_cast<unsigned long>(-1);
			unsigned long result;

			if (!_BitScanForward(&result, mask | static_cast<unsigned long>(value)))
				return numeric_limits<unsigned char>::digits;

			return static_cast<int>(result);
		}

		int countr_zero_bsf(const unsigned short value) noexcept
		{
			constexpr unsigned long mask = ~static_cast<unsigned long>(-1);
			unsigned long result;

			if (!_BitScanForward(&result, mask | static_cast<unsigned long>(value)))
				return numeric_limits<unsigned short>::digits;

			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countr_zero_bsf(const T value) noexcept
		{
			unsigned long result;

			if (!_BitScanForward(&result, static_cast<unsigned long>(value)))
				return numeric_limits<T>::digits;

			return static_cast<int>(result);
		}

#ifdef BIT14_USING_64BIT 
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countr_zero_bsf(const T value) noexcept
		{
			unsigned long result;

			if (!_BitScanForward64(&result, static_cast<unsigned __int64>(value)))
				return numeric_limits<T>::digits;

			return static_cast<int>(result);
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countr_zero_bsf(const T value) noexcept
		{
			unsigned long result;
			const uint32_t low_bits = static_cast<uint32_t>(value);

			if (_BitScanForward(&result, static_cast<unsigned long>(low_bits)))
				return static_cast<int>(result);

			if (_BitScanForward(&result, static_cast<unsigned long>(value >> 32)))
				return static_cast<int>(result) + 32;

			return numeric_limits<T>::digits;
		}
#endif //end of #ifdef _WIN64
#endif //end of #ifndef BIT14_HAS_BSF_INTRINSICS

#if defined(_M_IX86 ) || defined(_WIN64)
#define BIT14_HAS_BSR_INTRINSICS
#if defined(_M_IX86 ) || (defined(_M_X64 ) && !defined(_M_ARM64EC))
#define BIT14_MIGHT_HAVE_ABM_INTRINSICS
#endif
#endif

#ifdef BIT14_MIGHT_HAVE_ABM_INTRINSICS
		int countl_zero_lzcnt(const unsigned char value) noexcept
		{
			constexpr unsigned int diff = numeric_limits<unsigned int>::digits -
				numeric_limits<unsigned char>::digits;
			const unsigned int result = __lzcnt(static_cast<unsigned int>(value));
			return static_cast<int>(result - diff);
		}

		int countl_zero_lzcnt(const unsigned short value) noexcept
		{
			const unsigned short result = __lzcnt16(value);
			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countl_zero_lzcnt(const T value) noexcept
		{
			const unsigned int result = __lzcnt(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}
#ifdef BIT14_USING_64BIT
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countl_zero_lzcnt(const T value) noexcept
		{
			const unsigned __int64 result = __lzcnt64(static_cast<unsigned __int64>(value));
			return static_cast<int>(result);
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countl_zero_lzcnt(const T value) noexcept
		{
			constexpr int digits = numeric_limits<uint32_t>::digits;
			const unsigned int high_bits = value >> digits;
			const unsigned int result = __lzcnt(high_bits);

			if (result != digits)
				return result;

			const unsigned int low_bits = static_cast<unsigned int>(value);
			return __lzcnt(low_bits) + digits;
		}
#endif //end of #ifdef BIT14_USING_64BIT
#endif //end of #ifdef BIT14_MIGHT_HAVE_ABM_INTRINSICS
#ifdef BIT14_HAS_BSR_INTRINSICS
		int countl_zero_bsr(const unsigned char value) noexcept
		{
			constexpr int digits = numeric_limits<unsigned char>::digits - 1;
			unsigned long result;

			if (_BitScanReverse(&result, static_cast<unsigned long>(value)))
				return digits - static_cast<int>(result);

			return numeric_limits<unsigned char>::digits;
		}

		int countl_zero_bsr(const unsigned short value) noexcept
		{
			constexpr int digits = numeric_limits<unsigned short>::digits - 1;
			unsigned long result;

			if (_BitScanReverse(&result, static_cast<unsigned long>(value)))
				return digits - static_cast<int>(result);

			return numeric_limits<unsigned short>::digits;
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countl_zero_bsr(const T value) noexcept
		{
			constexpr int digits = numeric_limits<T>::digits - 1;
			unsigned long result;

			if (_BitScanReverse(&result, static_cast<unsigned long>(value)))
				return digits - static_cast<int>(result);

			return numeric_limits<T>::digits;
		}
#ifdef BIT14_USING_64BIT
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countl_zero_bsr(const T value) noexcept
		{
			constexpr int digits = numeric_limits<T>::digits - 1;
			unsigned long result;

			if (_BitScanReverse64(&result, static_cast<unsigned __int64>(value)))
				return digits - static_cast<int>(result);

			return numeric_limits<T>::digits;
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countl_zero_bsr(const T value) noexcept
		{
			constexpr int digits_32 = numeric_limits<uint32_t>::digits - 1;
			constexpr int digits_64 = numeric_limits<uint64_t>::digits - 1;
			const unsigned long high_bits = static_cast<unsigned long>(value >> 32);
			unsigned long result;

			if (_BitScanReverse(&result, high_bits))
				return digits_32 - static_cast<int>(result);

			if (_BitScanReverse(&result, static_cast<unsigned long>(value)))
				return digits_64 - static_cast<int>(result);

			return numeric_limits<T>::digits;
		}
#endif //end of #ifdef _WIN64
#endif //end of #ifdef BIT14_HAS_BSR_INTRINSICS
#ifdef BIT14_MIGHT_HAVE_ABM_INTRINSICS
		int popcount_abm(const unsigned char value) noexcept
		{
			const unsigned int result = __popcnt(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}

		int popcount_abm(const unsigned short value) noexcept
		{
			const unsigned int result = __popcnt(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int popcount_abm(const T value) noexcept
		{
			const unsigned int result = __popcnt(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}
#ifdef BIT14_USING_64BIT 
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int popcount_abm(const T value) noexcept
		{
			const unsigned __int64 result = __popcnt64(static_cast<unsigned __int64>(value));
			return static_cast<int>(result);
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int popcount_abm(const T value) noexcept
		{
			const uint32_t low_bits = static_cast<uint32_t>(value);
			const uint32_t high_bits = static_cast<uint32_t>(value >> 32);
			const  unsigned int low_result = __popcnt(static_cast<unsigned int>(low_bits));
			const  unsigned int high_result = __popcnt(static_cast<unsigned int>(high_bits));

			return static_cast<int>(low_result + high_result);
		}
#endif //end of #ifdef _WIN64
#endif //end of #ifdef BIT14_MIGHT_HAVE_ABM_INTRINSICS
#endif //end of #if defined(BIT14_USING_X86) 

#ifdef BIT14_USING_ARM
		template <typename T, use_if_bit14_32_bit_or_less_type<T> = true>
		int popcount_arm(const T value) noexcept
		{
			const unsigned int result = _CountOneBits(static_cast<unsigned long>(value));
			return static_cast<int>(result);
		}
		
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int popcount_arm(const T value) noexcept
		{
			const unsigned int result = _CountOneBits64(static_cast<unsigned __int64>(value));
			return static_cast<int>(result);
		}
		
		int countr_zero_arm(const unsigned char value) noexcept
		{
			constexpr unsigned char mask = static_cast<unsigned char>(-1);
			const unsigned int result = _CountTrailingZeros(static_cast<unsigned long>((~mask) | value));
			return static_cast<int>(result);
		}

		int countr_zero_arm(const unsigned short value) noexcept
		{
			constexpr unsigned short mask = static_cast<unsigned short>(-1);
			const unsigned int result = _CountTrailingZeros(static_cast<unsigned long>((~mask) | value));
			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countr_zero_arm(const T value) noexcept
		{
			const unsigned int result = _CountTrailingZeros(static_cast<unsigned long>(value));
			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countr_zero_arm(const T value) noexcept
		{
			const unsigned __int64 result = _CountTrailingZeros64(static_cast<unsigned __int64>(value));
			return static_cast<int>(result);
		}

		int countl_zero_arm(const unsigned char value) noexcept
		{
			constexpr unsigned int diff = numeric_limits<unsigned long>::digits -
				numeric_limits<unsigned char>::digits;
			const unsigned int result = _CountLeadingZeros(static_cast<unsigned long>(value));
			return static_cast<int>(result - diff);
		}

		int countl_zero_arm(const unsigned short value) noexcept
		{
			constexpr unsigned int diff = numeric_limits<unsigned long>::digits -
				numeric_limits<unsigned short>::digits;
			const unsigned int result = _CountLeadingZeros(static_cast<unsigned long>(value));
			return static_cast<int>(result - diff);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countl_zero_arm(const T value) noexcept
		{
			const unsigned int result = _CountLeadingZeros(static_cast<unsigned long>(value));
			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countl_zero_arm(const T value) noexcept
		{
			const unsigned int result = _CountLeadingZeros64(static_cast<unsigned __int64>(value));
			return static_cast<int>(result);
		}
#endif //end of #if defined(_M_ARM64) || defined(_M_ARM))
#endif //end of #ifdef _MSC_VER

		/*======================================================
		||		^^^^    End of msvc intrinsics    ^^^^		  ||
		||----------------------------------------------------||
		||    vvvv    Start of ICC/ICPX intrinsics    vvvv	  || 
		=======================================================*/

#if defined(BIT14_USING_ICC) || defined(BIT14_USING_ICPX)
#ifdef BIT14_USING_X86
#define BIT14_HAS_BSF_INTRINSICS
#define BIT14_MIGHT_HAVE_BMI_INTRINSICS
#endif

#ifdef BIT14_MIGHT_HAVE_BMI_INTRINSICS
		int countr_zero_bmi(const unsigned char value) noexcept
		{
			constexpr unsigned char mask = static_cast<unsigned char>(-1);
			const unsigned int result = _tzcnt_u32(static_cast<unsigned int>((~mask) | value));
			return static_cast<int>(result);
		}

		int countr_zero_bmi(const unsigned short value) noexcept
		{
			const unsigned int result = _tzcnt_u16(value);
			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countr_zero_bmi(const T value) noexcept
		{
			const unsigned int result = _tzcnt_u32(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}
#ifdef BIT14_USING_64BIT 
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countr_zero_bmi(const T value) noexcept
		{
			const unsigned __int64 result = _tzcnt_u64(static_cast<unsigned __int64>(value));
			return static_cast<int>(result);
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countr_zero_bmi(const T value) noexcept
		{
			const uint32_t low_bits = static_cast<uint32_t>(value);

			if (low_bits)
			{
				const unsigned int result = _tzcnt_u32(static_cast<unsigned int>(low_bits));
				return static_cast<int>(result);
			}

			const uint32_t high_bits = static_cast<uint32_t>(value >> 32);
			const unsigned int result = _tzcnt_u32(static_cast<unsigned int>(high_bits));
			return static_cast<int>(result) + 32;
		}
#endif //end of #ifdef BIT14_USING_64BIT
#endif //end of #ifdef BIT14_MIGHT_HAVE_BMI_INTRINSICS

#ifdef BIT14_HAS_BSF_INTRINSICS
		int countr_zero_bsf(const unsigned char value) noexcept
		{
			constexpr unsigned long mask = ~static_cast<unsigned long>(-1);
			unsigned long result;

			if (!_BitScanForward(&result, mask | static_cast<unsigned long>(value)))
				return numeric_limits<unsigned char>::digits;

			return static_cast<int>(result);
		}

		int countr_zero_bsf(const unsigned short value) noexcept
		{
			constexpr unsigned long mask = ~static_cast<unsigned long>(-1);
			unsigned long result;

			if (!_BitScanForward(&result, mask | static_cast<unsigned long>(value)))
				return numeric_limits<unsigned short>::digits;

			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countr_zero_bsf(const T value) noexcept
		{
			unsigned long result;

			if (!_BitScanForward(&result, static_cast<unsigned long>(value)))
				return numeric_limits<T>::digits;

			return static_cast<int>(result);
		}

#ifdef BIT14_USING_64BIT 
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countr_zero_bsf(const T value) noexcept
		{
			unsigned long result;

			if (!_BitScanForward64(&result, static_cast<unsigned __int64>(value)))
				return numeric_limits<T>::digits;

			return static_cast<int>(result);
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countr_zero_bsf(const T value) noexcept
		{
			unsigned long result;
			const uint32_t low_bits = static_cast<uint32_t>(value);

			if (_BitScanForward(&result, static_cast<unsigned long>(low_bits)))
				return static_cast<int>(result);

			if (_BitScanForward(&result, static_cast<unsigned long>(value >> 32)))
				return static_cast<int>(result) + 32;

			return numeric_limits<T>::digits;
		}
#endif //end of #ifdef _WIN64
#endif //end of #ifndef BIT14_HAS_BSF_INTRINSICS

#ifdef BIT14_USING_X86
#define BIT14_HAS_BSR_INTRINSICS
#endif

#ifdef __AVX2__
#define BIT14_HAS_ABM_INTRINSICS
#endif

#ifdef BIT14_HAS_ABM_INTRINSICS
		int countl_zero(const unsigned char value) noexcept
		{
			constexpr unsigned int diff = numeric_limits<unsigned int>::digits -
				numeric_limits<unsigned char>::digits;
			const unsigned int result = _lzcnt_u32(static_cast<unsigned int>(value));
			return static_cast<int>(result - diff);
		}

		int countl_zero(const unsigned short value) noexcept
		{
			constexpr unsigned int diff = numeric_limits<unsigned int>::digits -
				numeric_limits<unsigned short>::digits;
			const unsigned int result = _lzcnt_u32(static_cast<unsigned int>(value));
			return static_cast<int>(result - diff);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countl_zero(const T value) noexcept
		{
			const unsigned int result = _lzcnt_u32(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}

#ifdef BIT14_USING_64BIT
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countl_zero(const T value) noexcept
		{
			const unsigned __int64 result = _lzcnt_u64(static_cast<unsigned __int64>(value));
			return static_cast<int>(result);
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countl_zero(const T value) noexcept
		{
			constexpr int digits = numeric_limits<uint32_t>::digits;
			const unsigned int high_bits = value >> digits;
			const unsigned int result = _lzcnt_u32(high_bits);

			if (result != digits)
				return result;

			const unsigned int low_bits = static_cast<unsigned int>(value);
			return _lzcnt_u32(low_bits) + digits;
		}
#endif //end of #ifdef BIT14_USING_64BIT
#elif defined(BIT14_HAS_BSR_INTRINSICS)
		int countl_zero(const unsigned char value) noexcept
		{
			constexpr int digits = numeric_limits<unsigned char>::digits - 1;
			unsigned long result;

			if (_BitScanReverse(&result, static_cast<unsigned long>(value)))
				return digits - static_cast<int>(result);

			return numeric_limits<unsigned char>::digits;
		}

		int countl_zero(const unsigned short value) noexcept
		{
			constexpr int digits = numeric_limits<unsigned short>::digits - 1;
			unsigned long result;

			if (_BitScanReverse(&result, static_cast<unsigned long>(value)))
				return digits - static_cast<int>(result);

			return numeric_limits<unsigned short>::digits;
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int countl_zero(const T value) noexcept
		{
			constexpr int digits = numeric_limits<T>::digits - 1;
			unsigned long result;

			if (_BitScanReverse(&result, static_cast<unsigned long>(value)))
				return digits - static_cast<int>(result);

			return numeric_limits<T>::digits;
		}

#ifdef BIT14_USING_64BIT
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countl_zero(const T value) noexcept
		{
			constexpr int digits = numeric_limits<T>::digits - 1;
			unsigned long result;

			if (_BitScanReverse64(&result, static_cast<unsigned __int64>(value)))
				return digits - static_cast<int>(result);

			return numeric_limits<T>::digits;
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int countl_zero(const T value) noexcept
		{
			constexpr int digits_32 = numeric_limits<uint32_t>::digits - 1;
			constexpr int digits_64 = numeric_limits<uint64_t>::digits - 1;
			const unsigned long high_bits = static_cast<unsigned long>(value >> 32);
			unsigned long result;

			if (_BitScanReverse(&result, high_bits))
				return digits_32 - static_cast<int>(result);

			if (_BitScanReverse(&result, static_cast<unsigned long>(value)))
				return digits_64 - static_cast<int>(result);

			return numeric_limits<T>::digits;
		}
#endif //end of #ifdef BIT14_USING_64BIT
#endif //end of #ifdef BIT14_HAS_BSR_INTRINSICS

#ifdef BIT14_HAS_ABM_INTRINSICS
		int popcount(const unsigned char value) noexcept
		{
			const unsigned int result = _mm_popcnt_u32(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}

		int popcount(const unsigned short value) noexcept
		{
			const unsigned int result = _mm_popcnt_u32(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}

		template <typename T, use_if_bit14_32_bit_type<T> = true>
		int popcount(const T value) noexcept
		{
			const unsigned int result = _mm_popcnt_u32(static_cast<unsigned int>(value));
			return static_cast<int>(result);
		}

#ifdef BIT14_USING_64BIT 
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int popcount(const T value) noexcept
		{
			const unsigned __int64 result = _mm_popcnt_u64(static_cast<unsigned __int64>(value));
			return static_cast<int>(result);
		}
#else
		template <typename T, use_if_bit14_64_bit_type<T> = true>
		int popcount(const T value) noexcept
		{
			const uint32_t low_bits = static_cast<uint32_t>(value);
			const uint32_t high_bits = static_cast<uint32_t>(value >> 32);
			const  unsigned int low_result = _mm_popcnt_u32(static_cast<unsigned int>(low_bits));
			const  unsigned int high_result = _mm_popcnt_u32(static_cast<unsigned int>(high_bits));

			return static_cast<int>(low_result + high_result);
		}
#endif //end of #ifdef BIT14_USING_64BIT 
#else
		template <typename T, use_if_bit14_type<T> = true>
		int popcount(const T value) noexcept
		{
			return popcount_fallback(value);
		}
#endif //end of #ifdef BIT14_HAS_ABM_INTRINSICS
#endif //end of #if defined(BIT14_USING_ICC) || defined(BIT14_USING_ICPX)
	}//end namespace detail

		/*======================================================
		||    ^^^^    End of ICC / ICPX intrinsics    ^^^^    ||
		=======================================================*/

	using detail::use_if_bit14_type;
	using detail::use_if_bit14_8_bit_type;
	using detail::use_if_bit14_16_bit_type;
	using detail::use_if_bit14_32_bit_type;
	using detail::use_if_bit14_64_bit_type;
	using detail::use_if_bit14_32_bit_or_less_type;
	using detail::use_if_unsigned_char;
	using detail::use_if_unsigned_short;
	using detail::use_if_unsigned_long;
	using detail::use_if_unsigned_long_long;
	using detail::use_if_integral;
	using detail::use_if_8_bit_integral;
	using detail::use_if_16_bit_integral;
	using detail::use_if_32_bit_integral;
	using detail::use_if_64_bit_integral;
	using detail::enable_if_types_are_trivally_copyable_and_same_size;

} //end namespace bit14

#include "bit14_preprocessor_cleanup.h"