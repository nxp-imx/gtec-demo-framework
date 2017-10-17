#ifndef RAPIDVULKAN_DEBUG_STRINGS_VKPIPELINESTAGEFLAGBITS_HPP
#define RAPIDVULKAN_DEBUG_STRINGS_VKPIPELINESTAGEFLAGBITS_HPP
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
    inline const char* ToString(const VkPipelineStageFlagBits& value)
    {
      switch(value)
      {
      case VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT:
        return "VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT";
      case VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT:
        return "VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT";
      case VK_PIPELINE_STAGE_VERTEX_INPUT_BIT:
        return "VK_PIPELINE_STAGE_VERTEX_INPUT_BIT";
      case VK_PIPELINE_STAGE_VERTEX_SHADER_BIT:
        return "VK_PIPELINE_STAGE_VERTEX_SHADER_BIT";
      case VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT:
        return "VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT";
      case VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT:
        return "VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT";
      case VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT:
        return "VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT";
      case VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT:
        return "VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT";
      case VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT:
        return "VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT";
      case VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT:
        return "VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT";
      case VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT:
        return "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT";
      case VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT:
        return "VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT";
      case VK_PIPELINE_STAGE_TRANSFER_BIT:
        return "VK_PIPELINE_STAGE_TRANSFER_BIT";
      case VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT:
        return "VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT";
      case VK_PIPELINE_STAGE_HOST_BIT:
        return "VK_PIPELINE_STAGE_HOST_BIT";
      case VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT:
        return "VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT";
      case VK_PIPELINE_STAGE_ALL_COMMANDS_BIT:
        return "VK_PIPELINE_STAGE_ALL_COMMANDS_BIT";
      default:
        return "*Unknown*";
      }
    };
  }
}

#endif
