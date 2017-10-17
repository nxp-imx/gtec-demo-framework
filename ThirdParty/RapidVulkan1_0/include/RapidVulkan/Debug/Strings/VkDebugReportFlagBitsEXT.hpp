#ifndef RAPIDVULKAN_DEBUG_STRINGS_VKDEBUGREPORTFLAGBITSEXT_HPP
#define RAPIDVULKAN_DEBUG_STRINGS_VKDEBUGREPORTFLAGBITSEXT_HPP
//***************************************************************************************************************************************************
//* BSD 3-Clause License
//*
//* Copyright (c) 2017, Rene Thrane
//* All rights reserved.
//*
//* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//*
//* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
//*    documentation and/or other materials provided with the distribution.
//* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
//*    software without specific prior written permission.
//*
//* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//***************************************************************************************************************************************************

// If you use the functionality in this file its recommended to 'WRAP' it in a hpp/cpp file combo so its only included in one file!

// Auto-generated Vulkan 1.0 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000/RAIIGen)

#include <vulkan/vulkan.h>

namespace RapidVulkan
{
  namespace Debug
  {
    inline const char* ToString(const VkDebugReportFlagBitsEXT& value)
    {
      switch(value)
      {
      case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
        return "VK_DEBUG_REPORT_INFORMATION_BIT_EXT";
      case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        return "VK_DEBUG_REPORT_WARNING_BIT_EXT";
      case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
        return "VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT";
      case VK_DEBUG_REPORT_ERROR_BIT_EXT:
        return "VK_DEBUG_REPORT_ERROR_BIT_EXT";
      case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
        return "VK_DEBUG_REPORT_DEBUG_BIT_EXT";
      default:
        return "*Unknown*";
      }
    };
  }
}

#endif
