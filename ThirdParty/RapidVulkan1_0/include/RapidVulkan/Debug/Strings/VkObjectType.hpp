#ifndef RAPIDVULKAN_DEBUG_STRINGS_VKOBJECTTYPE_HPP
#define RAPIDVULKAN_DEBUG_STRINGS_VKOBJECTTYPE_HPP
#if VK_HEADER_VERSION >= 51
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
    inline const char* ToString(const VkObjectType& value)
    {
      switch(value)
      {
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_UNKNOWN:
        return "VK_OBJECT_TYPE_UNKNOWN";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_INSTANCE:
        return "VK_OBJECT_TYPE_INSTANCE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
        return "VK_OBJECT_TYPE_PHYSICAL_DEVICE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_DEVICE:
        return "VK_OBJECT_TYPE_DEVICE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_QUEUE:
        return "VK_OBJECT_TYPE_QUEUE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_SEMAPHORE:
        return "VK_OBJECT_TYPE_SEMAPHORE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_COMMAND_BUFFER:
        return "VK_OBJECT_TYPE_COMMAND_BUFFER";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_FENCE:
        return "VK_OBJECT_TYPE_FENCE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_DEVICE_MEMORY:
        return "VK_OBJECT_TYPE_DEVICE_MEMORY";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_BUFFER:
        return "VK_OBJECT_TYPE_BUFFER";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_IMAGE:
        return "VK_OBJECT_TYPE_IMAGE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_EVENT:
        return "VK_OBJECT_TYPE_EVENT";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_QUERY_POOL:
        return "VK_OBJECT_TYPE_QUERY_POOL";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_BUFFER_VIEW:
        return "VK_OBJECT_TYPE_BUFFER_VIEW";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_IMAGE_VIEW:
        return "VK_OBJECT_TYPE_IMAGE_VIEW";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_SHADER_MODULE:
        return "VK_OBJECT_TYPE_SHADER_MODULE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_PIPELINE_CACHE:
        return "VK_OBJECT_TYPE_PIPELINE_CACHE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
        return "VK_OBJECT_TYPE_PIPELINE_LAYOUT";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_RENDER_PASS:
        return "VK_OBJECT_TYPE_RENDER_PASS";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_PIPELINE:
        return "VK_OBJECT_TYPE_PIPELINE";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
        return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_SAMPLER:
        return "VK_OBJECT_TYPE_SAMPLER";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
        return "VK_OBJECT_TYPE_DESCRIPTOR_POOL";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_DESCRIPTOR_SET:
        return "VK_OBJECT_TYPE_DESCRIPTOR_SET";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_FRAMEBUFFER:
        return "VK_OBJECT_TYPE_FRAMEBUFFER";
#endif
#if VK_HEADER_VERSION >= 51
      case VK_OBJECT_TYPE_COMMAND_POOL:
        return "VK_OBJECT_TYPE_COMMAND_POOL";
#endif
      default:
        return "*Unknown*";
      }
    };
  }
}

#endif
#endif
