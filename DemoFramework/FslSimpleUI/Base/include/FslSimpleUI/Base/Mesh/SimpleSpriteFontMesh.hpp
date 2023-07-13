#ifndef FSLSIMPLEUI_BASE_MESH_SIMPLESPRITEFONTMESH_HPP
#define FSLSIMPLEUI_BASE_MESH_SIMPLESPRITEFONTMESH_HPP
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Base/Mesh/SpriteFontMeasureInfo.hpp>
#include <FslSimpleUI/Base/Mesh/SpriteMesh.hpp>

namespace Fsl::UI
{
  class SimpleSpriteFontMesh : public SpriteMesh<SpriteFont>
  {
  public:
    explicit SimpleSpriteFontMesh(const std::shared_ptr<IMeshManager>& meshManager)
      : SpriteMesh(meshManager)
    {
    }

    SimpleSpriteFontMesh(const std::shared_ptr<IMeshManager>& meshManager, const std::shared_ptr<SpriteFont>& sprite)
      : SimpleSpriteFontMesh(meshManager)
    {
      SetSprite(sprite);
    }

    void SetText(StringViewLite str);

    PxSize2D MinimalMeasure(StringViewLite str) const;
    PxSize2D Measure(StringViewLite str) const;
    SpriteFontMeasureInfo ComplexMeasure(StringViewLite str) const;
  };
}

#endif
