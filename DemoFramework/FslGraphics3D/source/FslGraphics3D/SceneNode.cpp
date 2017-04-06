/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslGraphics3D/SceneNode.hpp>
#include <FslGraphics3D/Mesh.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    SceneNode::SceneNode()
      : m_name()
      , m_meshes()
      , m_parent()
      , m_transformation()
    {

    }

    SceneNode::SceneNode(const std::size_t meshCapacity)
      : m_name()
      , m_meshes() // We dont initialize to capacity here since the deque doesnt play nice with that
      , m_parent()
      , m_transformation()
    {

    }


    const UTF8String& SceneNode::GetName() const
    {
      return m_name;
    }


    void SceneNode::SetName(const UTF8String& name)
    {
      m_name = name;
    }


    std::shared_ptr<SceneNode> SceneNode::GetParent() const
    {
      return m_parent.lock();
    }


    void SceneNode::SetParent(const std::shared_ptr<SceneNode>& parent)
    {
      m_parent = parent;
    }


    const Matrix& SceneNode::GetTransformation() const
    {
      return m_transformation;
    }


    void SceneNode::SetTransformation(const Matrix& transformation)
    {
      m_transformation = transformation;
    }


    int32_t SceneNode::GetMeshCount() const
    {
      return static_cast<int32_t>(m_meshes.size());
    }


    int32_t SceneNode::GetMeshAt(const int32_t index) const
    {
      return m_meshes[index];
    }


    void SceneNode::AddMesh(const int32_t meshIndex)
    {
      if (meshIndex < 0)
        throw std::invalid_argument("mesh index can not be negative");

      m_meshes.push_back(meshIndex);
    }


    int32_t SceneNode::GetChildCount() const
    {
      return static_cast<int32_t>(m_children.size());
    }


    std::shared_ptr<SceneNode> SceneNode::GetChildAt(const int32_t index) const
    {
      return m_children[index];
    }


    void SceneNode::AddChild(const std::shared_ptr<SceneNode>& node)
    {
      if (!node)
        throw std::invalid_argument("node can not be null");

      node->SetParent(shared_from_this());
      m_children.push_back(node);
    }

  }
}
