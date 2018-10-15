/*
 * The code contained herein is licensed under the MIT License.
 * Copyright (C) 2013-2015 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef FSL_PLATFORM_YOCTO
#include <mmdc.h>

extern "C" {

int get_mmdc_info(MMDC_INFO_t* pInfo)
{
  return 0;
}


int begin_mmdc(MMDC_CONTEXT_t* pContext)
{
  return 0;
}

void end_mmdc(MMDC_CONTEXT_t* pContext)
{
}


int auto_configuration_mmdc_profiling(MMDC_CONTEXT_t* pContext, char* pszType, int enablePrint)
{
  return 0;
}


void manual_configuration_mmdc_profiling(MMDC_CONTEXT_t* pContext, unsigned int value)
{
}


void start_mmdc_profiling(pMMDC_t mmdc)
{
}


void stop_mmdc_profiling(pMMDC_t mmdc)
{
}


void pause_mmdc_profiling(pMMDC_t mmdc)
{
}


void resume_mmdc_profiling(pMMDC_t mmdc)
{
}


void get_mmdc_profiling_results(pMMDC_t mmdc, MMDC_PROFILE_RES_t* results)
{
}


void print_mmdc_profiling_results(MMDC_PROFILE_RES_t results, MMDC_RES_TYPE_t print_type, int time)
{
}


void clear_mmdc_results(pMMDC_t mmdc)
{
}


void load_mmdc_results(pMMDC_t mmdc)
{
}
}

#endif
