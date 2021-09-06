#ifndef FSLGRAPHICS3D_SCENEFORMAT_CONVERSION_HPP
#define FSLGRAPHICS3D_SCENEFORMAT_CONVERSION_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslGraphics/PrimitiveType.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <FslGraphics3D/SceneFormat/PrimitiveType.hpp>
#include "SFVertexDeclaration.hpp"

namespace Fsl
{
  namespace SceneFormat
  {
    class Conversion
    {
    public:
      static SceneFormat::PrimitiveType Convert(const Fsl::PrimitiveType value);
      static Fsl::PrimitiveType Convert(const SceneFormat::PrimitiveType value);

      static SceneFormat::VertexElementFormat Convert(const Fsl::VertexElementFormat value);
      static Fsl::VertexElementFormat Convert(const SceneFormat::VertexElementFormat value);
      static SceneFormat::VertexElementUsage Convert(const Fsl::VertexElementUsage value);
      static Fsl::VertexElementUsage Convert(const SceneFormat::VertexElementUsage value);

      static SFVertexElement Convert(const VertexElementEx& element);
      static SFVertexDeclaration Convert(VertexDeclarationSpan vertexDeclaration);
    };
  }
}

#endif
