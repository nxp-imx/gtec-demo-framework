#ifndef FSLGRAPHICS3D_BASICSCENE_COMPLEXSCENE_HPP
#define FSLGRAPHICS3D_BASICSCENE_COMPLEXSCENE_HPP
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
#include <FslGraphics3D/BasicScene/Scene.hpp>
#include <deque>

namespace Fsl
{
  namespace Graphics3D
  {
    //! @brief A complex scene can contain meshes consisting of various mesh types.
    class ComplexScene : public Scene
    {
    public:
      std::deque<std::shared_ptr<Mesh> > Meshes;

      ComplexScene(const std::size_t numMeshes)
        : Scene()
        , Meshes(numMeshes)
      {
      }

      virtual int32_t GetMeshCount() const
      {
        return static_cast<int32_t>(Meshes.size());
      }


      virtual std::shared_ptr<Mesh> GetMeshAt(const int32_t index) const
      {
        return Meshes[index];
      }


      virtual void AddMesh(const std::shared_ptr<Mesh>& mesh)
      {
        if (!mesh)
          throw std::invalid_argument("mesh can not be null");

        Meshes.push_back(mesh);
      }

    };
  }
}

#endif
