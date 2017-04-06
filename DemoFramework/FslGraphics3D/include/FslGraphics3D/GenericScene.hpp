#ifndef FSLGRAPHICS3D_GENERICSCENE_HPP
#define FSLGRAPHICS3D_GENERICSCENE_HPP
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

#include <FslGraphics3D/GenericScene_fwd.hpp>
#include <FslGraphics3D/MeshAllocator.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    // The class is defined in GenericScene_fwd.hpp and this file contains the implementation of the 'templated' functions of GenericScene.
    // This allows us to use templates without 'spreading' dependencies to files that don't need it by allowing them to only include the _fwd file

    template<typename TMesh>
    GenericScene<TMesh>::GenericScene()
      : Scene(MeshAllocator::Allocate<TMesh>)
      , Meshes()
    {
    }


    template<typename TMesh>
    GenericScene<TMesh>::GenericScene(const std::size_t numMeshes)
      : Scene(MeshAllocator::Allocate<TMesh>)
      , Meshes() // We dont initialize to capacity here since the deque doesnt play nice with that
    {
    }


    template<typename TMesh>
    GenericScene<TMesh>::GenericScene(const MeshAllocatorFunc& meshAllocator, const std::size_t numMeshes)
      : Scene(meshAllocator)
      , Meshes() // We dont initialize to capacity here since the deque doesnt play nice with that
    {
    }


    template<typename TMesh>
    void GenericScene<TMesh>::AddMesh(const std::shared_ptr<TMesh>& mesh)
    {
      if (!mesh)
        throw std::invalid_argument("mesh can not be null");

      Meshes.push_back(mesh);
    }


    template<typename TMesh>
    int32_t GenericScene<TMesh>::GetMeshCount() const
    {
      return static_cast<int32_t>(Meshes.size());
    }


    template<typename TMesh>
    std::shared_ptr<Mesh> GenericScene<TMesh>::GetMeshAt(const int32_t index) const
    {
      return Meshes[index];
    }


    template<typename TMesh>
    void GenericScene<TMesh>::AddMesh(const std::shared_ptr<Mesh>& mesh)
    {
      std::shared_ptr<TMesh> meshEx = std::dynamic_pointer_cast<TMesh>(mesh);
      if (! meshEx)
        throw std::runtime_error("The mesh was not of the expected type");
      AddMesh(meshEx);
    }

  }
}

#endif
