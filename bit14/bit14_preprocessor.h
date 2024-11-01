//bit14_preprocessor.h

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
#if defined(__INTEL_LLVM_COMPILER)
#define BIT14_USING_ICPX
#elif defined(__INTEL_COMPILER)
#define BIT14_USING_ICC
#elif defined(__clang__)
#define BIT14_USING_CLANG
#elif defined(__GNUG__)
#define BIT14_USING_GCC
#elif defined (_MSC_VER)
#define BIT14_USING_MSVC
#elif defined(__open_xl__) && !defined(__COMPILER_VER__)
#define BIT14_USING_OPEN_XL
#elif defined(__ibmxl__) && !defined(__COMPILER_VER__)
#define BIT14_USING_XL
#endif

#if defined(i386) || defined(__i386__) || defined(__i386) ||\
defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) ||\
defined(_M_IX86) || defined(__LP64__) ||defined(_M_AMD64) || defined(_M_X64) ||\
((defined(_WIN32) || defined(_WIN64)) && !defined(_M_ARM64) && !defined(_M_ARM))
#define BIT14_USING_X86
#endif

#if defined(__arm__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM)
#define BIT14_USING_ARM
#endif

#if defined(_WIN64) || defined(__x86_64) || defined(__x86_64__) || defined(__LP64__ ) ||\
 defined(__LP64) || defined(_M_X64) || defined(_M_ARM64)
#define BIT14_USING_64BIT
#endif