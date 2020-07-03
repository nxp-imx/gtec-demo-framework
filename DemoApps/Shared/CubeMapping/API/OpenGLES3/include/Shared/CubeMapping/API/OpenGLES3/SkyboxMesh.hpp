#ifndef SHARED_CUBEMAPPING_API_OPENGLES3_SKYBOXMESH_HPP
#define SHARED_CUBEMAPPING_API_OPENGLES3_SKYBOXMESH_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLVertexArray.hpp>
#include <utility>

namespace Fsl
{
  class SkyboxMesh
  {
  public:
    GLES3::GLVertexBuffer VertexBuffer;
    GLES3::GLVertexArray VertexArray;

    SkyboxMesh(const SkyboxMesh&) = delete;
    SkyboxMesh& operator=(const SkyboxMesh&) = delete;


    //! @brief Move assignment operator
    SkyboxMesh& operator=(SkyboxMesh&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        VertexBuffer = std::move(other.VertexBuffer);
        VertexArray = std::move(other.VertexArray);

        // Remove the data from other
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    SkyboxMesh(SkyboxMesh&& other) noexcept
      : VertexBuffer(std::move(other.VertexBuffer))
      , VertexArray(std::move(other.VertexArray))
    {
      // Remove the data from other
    }

    SkyboxMesh();
    explicit SkyboxMesh(const GLES3::GLProgram& program);

    bool IsValid() const
    {
      return VertexBuffer.IsValid() && VertexArray.IsValid();
    }

    void Reset() noexcept;
    void Reset(const GLES3::GLProgram& program);
  };
}

#endif
