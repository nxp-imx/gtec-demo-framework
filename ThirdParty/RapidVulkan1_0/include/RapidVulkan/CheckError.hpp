#ifndef RAPIDVULKAN_CHECKERROR_HPP
#define RAPIDVULKAN_CHECKERROR_HPP
//***************************************************************************************************************************************************
//* BSD 3-Clause License
//*
//* Copyright (c) 2016, Rene Thrane
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

#include <RapidVulkan/Exceptions.hpp>
#include <cassert>
#include <string>
#include <vulkan/vulkan.h>

namespace RapidVulkan
{
  inline void CheckError(const VkResult result)
  {
    if (result != VK_SUCCESS)
      throw VulkanErrorException("Unknown", result);
  }


  inline void CheckError(const VkResult result, const char*const pszMessage)
  {
    if (result != VK_SUCCESS)
    {
      assert(pszMessage != nullptr);
      throw VulkanErrorException(pszMessage, result);
    }
  }


  inline void CheckError(const VkResult result, const std::string& message)
  {
    if (result != VK_SUCCESS)
    {
      throw VulkanErrorException(message, result);
    }
  }


  inline void CheckError(const VkResult result, const char*const pszMessage, const char*const pszFileName, const int lineNumber)
  {
    if (result != VK_SUCCESS)
    {
      assert(pszMessage != nullptr);
      assert(pszFileName != nullptr);
      throw VulkanErrorException(pszMessage, result, pszFileName, lineNumber);
    }
  }


  inline void CheckError(const VkResult result, const char*const pszMessage, const std::string& fileName, const int lineNumber)
  {
    if (result != VK_SUCCESS)
    {
      assert(pszMessage != nullptr);
      throw VulkanErrorException(pszMessage, result, fileName, lineNumber);
    }
  }


  inline void CheckError(const VkResult result, const std::string& message, const char*const pszFileName, const int lineNumber)
  {
    if (result != VK_SUCCESS)
    {
      assert(pszFileName != nullptr);
      throw VulkanErrorException(message, result, pszFileName, lineNumber);
    }
  }


  inline void CheckError(const VkResult result, const std::string& message, const std::string& fileName, const int lineNumber)
  {
    if (result != VK_SUCCESS)
    {
      throw VulkanErrorException(message, result, fileName, lineNumber);
    }
  }
}

#endif
