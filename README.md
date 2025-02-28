# bit14

This is a recreation of the C++20 bit header written in C++14.

https://en.cppreference.com/w/cpp/header/bit

This project is intended for anyone who wishes to use the standard library header bit.h but is stuck on c++14 or 17.
Simply add folder bit14 to your list of include directories and #include bit14.h where desired.
On msvc, gcc, clang, IBM XL / Open XL for Linux or AIX, or Intel ICC / ICPX compilers it uses x86, ARM or Power intrinsics whenever available.
Otherwise, it falls back on a generic solution. On other compilers, a fully generic solution is used.
All function descriptions on cppreference.com apply exactly to the functions in this header with the following exceptions:

* All functions and enums in this header exist within namespace bit14.
* Extended integer types are not supported.
* The target platform must use the ILP32, LLP64 or LP64 data model,
  meaning char = 1 byte, short = 2 bytes, int = 4 bytes,
  long = 4 or 8 bytes and long long = 8 bytes.
* Every function in this header other than bit14::bit_cast has a
  seperate constexpr version that takes its arguments as template
  parameters (see below). bit14::bit_cast has no constexpr version.
* The endian enum only exists on gcc, clang, msvc, IBM XL / Open XL
  for Linux or AIX and Intel ICC / ICPX compilers.
* If using c++14, bit14::byteswap does not check for padding bits.
* bit14::bitceil is noexcept

## constexpr functions

The interface for all constexpr functions are listed below:

    template <typename T, T Value>
    constexpr int bit14::popcount() noexcept;
    
    template <typename T, T Value>
    constexpr int bit14::countr_zero() noexcept;

    template <typename T, T Value>
    constexpr int bit14::countl_zero() noexcept;

    template <typename T, T Value>
    constexpr int bit14::countr_one() noexcept;

    template <typename T, T Value>
    constexpr int bit14::countl_one() noexcept;

    template <typename T, T Value, int Shift>
    constexpr T bit14::rotl() noexcept;

    template <typename T, T Value, int Shift>
    constexpr T bit14::rotr() noexcept;

    template <typename T, T Value>
    constexpr int bit14::bit_width() noexcept;

    template <typename T, T Value>
    constexpr T bit14::bit_floor() noexcept;

    template <typename T, T Value>
    constexpr T bit14::bit_ceil() noexcept;

    template <typename T, T Value>
    constexpr bool bit14::has_single_bit() noexcept;

    template<typename T, T Value>
    constexpr T bit14::byteswap() noexcept;
