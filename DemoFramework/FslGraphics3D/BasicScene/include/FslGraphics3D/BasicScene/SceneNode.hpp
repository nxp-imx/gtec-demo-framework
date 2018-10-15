#ifndef FSLGRAPHICS3D_BASICSCENE_SCENENODE_HPP
#define FSLGRAPHICS3D_BASICSCENE_SCENENODE_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/String/UTF8String.hpp>
#include <deque>
#include <memory>

namespace Fsl
{
  namespace Graphics3D
  {
    class Mesh;

    class SceneNode : public std::enable_shared_from_this<SceneNode>
    {
      UTF8String m_name;
      std::deque<int32_t> m_meshes;
      std::deque<std::shared_ptr<SceneNode>> m_children;
      std::weak_ptr<SceneNode> m_parent;
      Matrix m_transformation;

    public:
      SceneNode();
      SceneNode(const std::size_t meshCapacity);

      virtual ~SceneNode() = default;

      //! @brief Get the name of the node
      const UTF8String& GetName() const;

      //! @brief Set the name of the node
      void SetName(const UTF8String& name);

      //! @brief Get the parent node
      std::shared_ptr<SceneNode> GetParent() const;

      //! @brief Set the parent node
      void SetParent(const std::shared_ptr<SceneNode>& parent);

      //! @brief Get the transformation relative to the parent
      const Matrix& GetTransformation() const;

      //! @brief set the transformation relative to the parent
      void SetTransformation(const Matrix& transformation);

      int32_t GetMeshCount() const;
      int32_t GetMeshAt(const int32_t index) const;
      void AddMesh(const int32_t meshIndex);

      int32_t GetChildCount() const;

      std::shared_ptr<SceneNode> GetChildAt(const int32_t index) const;

      //! @brief Adding a child automatically sets the parent node to this one
      void AddChild(const std::shared_ptr<SceneNode>& node);
    };
  }
}

#endif
