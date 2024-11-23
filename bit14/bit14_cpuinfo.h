//bit14_cpuinfo.h

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

#include "bit14_preprocessor.h"

#if defined(BIT14_USING_X86)
#if(defined(BIT14_USING_MSVC) || defined(BIT14_USING_ICC) || defined(BIT14_USING_ICPX))
#define BIT14_HAS_CPUID
#elif defined(BIT14_USING_CLANG) || defined(BIT14_USING_GCC)
#define BIT14_HAS_BUILTIN_CPU_SUPPORTS
#endif
#endif

#ifdef BIT14_HAS_CPUID
#include <intrin.h>
#endif

namespace bit14
{
	namespace detail
	{
		class bit14_cpu_info
		{
		public:
			bit14_cpu_info() : m_bmi(false), m_lzcnt(false), m_popcount(false)
			{
				instruction_support();
			}
			
			bool has_popcnt() const noexcept
			{
				return m_popcount;
			}

			bool has_lzcnt() const noexcept
			{
				return m_lzcnt;
			}

			bool has_bmi() const noexcept
			{
				return m_bmi;
			}
			
		private:
			void instruction_support() noexcept
			{
#ifdef BIT14_HAS_CPUID
				int m_data[4] = { 0, 0, 0, 0 };

				__cpuid(m_data, 0x00000007);
				m_bmi = m_data[1] & 8;

				__cpuid(m_data, 0x01);
				m_popcount = m_data[2] & 8388608;

				__cpuid(m_data, 0x80000001);
				m_lzcnt = m_data[2] & 32;

#elif defined(BIT14_HAS_BUILTIN_CPU_SUPPORTS)
				__builtin_cpu_init();
				m_popcount = __builtin_cpu_supports("popcnt");
				m_lzcnt = m_popcount;
				m_bmi = __builtin_cpu_supports("bmi");
#endif
			}

			bool m_bmi;
			bool m_lzcnt;
			bool m_popcount;
		};
	}
}

#undef BIT14_HAS_CPUID
#undef BIT14_HAS_BUILTIN_CPU_SUPPORTS
#include "bit14_preprocessor_cleanup.h"
