#ifndef FSLUTIL_VULKAN1_0_UTIL_VUBUFFERMEMORYUTIL_GLM_HPP
#define FSLUTIL_VULKAN1_0_UTIL_VUBUFFERMEMORYUTIL_GLM_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the NXP. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>

namespace Fsl::Vulkan::VUBufferMemoryUtil
{
  inline void Upload(VUBufferMemory& rBuffer, const uint32_t offset, const glm::vec2& vec, const VkMemoryMapFlags memoryMapFlags = 0)
  {
    rBuffer.Upload(offset, glm::value_ptr(vec), sizeof(float) * 2, memoryMapFlags);
  }

  inline void Upload(VUBufferMemory& rBuffer, const uint32_t offset, const glm::vec3& vec, const VkMemoryMapFlags memoryMapFlags = 0)
  {
    rBuffer.Upload(offset, glm::value_ptr(vec), sizeof(float) * 3, memoryMapFlags);
  }

  inline void Upload(VUBufferMemory& rBuffer, const uint32_t offset, const glm::vec4& vec, const VkMemoryMapFlags memoryMapFlags = 0)
  {
    rBuffer.Upload(offset, glm::value_ptr(vec), sizeof(float) * 4, memoryMapFlags);
  }

  inline void Upload(VUBufferMemory& rBuffer, const uint32_t offset, const glm::mat2& mat, const VkMemoryMapFlags memoryMapFlags = 0)
  {
    // Only upload relevant columns and rows.
    rBuffer.Upload(offset, glm::value_ptr(glm::mat4(mat)), sizeof(float) * 6, memoryMapFlags);
  }

  inline void Upload(VUBufferMemory& rBuffer, const uint32_t offset, const glm::mat3& mat, const VkMemoryMapFlags memoryMapFlags = 0)
  {
    // Only upload relevant columns and rows.
    rBuffer.Upload(offset, glm::value_ptr(glm::mat4(mat)), sizeof(float) * 11, memoryMapFlags);
  }

  inline void Upload(VUBufferMemory& rBuffer, const uint32_t offset, const glm::mat4& mat, const VkMemoryMapFlags memoryMapFlags = 0)
  {
    rBuffer.Upload(offset, glm::value_ptr(mat), sizeof(float) * 16, memoryMapFlags);
  }
}

#endif
