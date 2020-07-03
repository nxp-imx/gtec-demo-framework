#ifndef FSLASSIMP_SCENEIMPORTER_HPP
#define FSLASSIMP_SCENEIMPORTER_HPP
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

#include <assimp/Importer.hpp>    //OO version Header!
#include <assimp/postprocess.h>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslAssimp/MeshImporter.hpp>
#include <FslGraphics3D/BasicScene/SceneAllocator.hpp>
#include <FslGraphics3D/BasicScene/SceneAllocatorFunc.hpp>
#include <memory>

namespace Fsl
{
  namespace Graphics3D
  {
    class ComplexScene;
    class Scene;
  }

  class SceneImporter
  {
    // Hint: Destroying the importer is expensive, so don't use more than once instance per thread.
    Assimp::Importer m_importer;
    MeshImporter m_meshImporter;

  public:
    SceneImporter();
    explicit SceneImporter(Graphics3D::SceneAllocatorFunc sceneAllocator);

    //! @brief Load the given file using the supplied pFlags
    //! @param filename the file to load.
    //! @param pFlags will be passed directly to Assimp::Importer ReadFile
    std::shared_ptr<Graphics3D::Scene> Load(const IO::Path& filename, unsigned int pFlags = aiProcessPreset_TargetRealtime_Quality);

    //! @brief Load the given file using the supplied pFlags
    //! @param filename the file to load.
    //! @param desiredSize The scene bounding box will be calculated and then scaled to the desired size. WARNING: we do not process the nodes
    //! according to this size!!
    //! @param pFlags will be passed directly to Assimp::Importer ReadFile
    std::shared_ptr<Graphics3D::Scene> Load(const IO::Path& filename, const float desiredSize, const bool centerModel,
                                            unsigned int pFlags = aiProcessPreset_TargetRealtime_Quality);


    //! @brief Load the given file using the supplied pFlags
    //! @param filename the file to load.
    //! @param pFlags will be passed directly to Assimp::Importer ReadFile
    //! @param meshAllocator the scene allocator to use.
    std::shared_ptr<Graphics3D::Scene> Load(const Graphics3D::SceneAllocatorFunc& sceneAllocator, const IO::Path& filename,
                                            unsigned int pFlags = aiProcessPreset_TargetRealtime_Fast);

    //! @brief Load the given file using the supplied pFlags
    //! @param filename the file to load.
    //! @param pFlags will be passed directly to Assimp::Importer ReadFile
    //! @param desiredSize The scene bounding box will be calculated and then scaled to the desired size. WARNING: we do not process the nodes
    //! according to this size!!
    //! @param meshAllocator the scene allocator to use.
    std::shared_ptr<Graphics3D::Scene> Load(const Graphics3D::SceneAllocatorFunc& sceneAllocator, const IO::Path& filename, const float desiredSize,
                                            const bool centerModel, unsigned int pFlags = aiProcessPreset_TargetRealtime_Fast);


    template <typename TScene>
    std::shared_ptr<TScene> Load(const IO::Path& filename, unsigned int pFlags = aiProcessPreset_TargetRealtime_Quality)
    {
      Graphics3D::SceneAllocatorFunc sceneAllocator(Graphics3D::SceneAllocator::Allocate<TScene>);
      auto res = std::dynamic_pointer_cast<TScene>(Load(sceneAllocator, filename, pFlags));
      if (!res)
      {
        throw std::runtime_error("Failed to allocate scene of the desired type");
      }
      return res;
    }

    template <typename TScene>
    std::shared_ptr<TScene> Load(const IO::Path& filename, const float desiredSize, const bool centerModel,
                                 unsigned int pFlags = aiProcessPreset_TargetRealtime_Fast)
    {
      Graphics3D::SceneAllocatorFunc sceneAllocator(Graphics3D::SceneAllocator::Allocate<TScene>);
      auto res = std::dynamic_pointer_cast<TScene>(Load(sceneAllocator, filename, desiredSize, centerModel, pFlags));
      if (!res)
      {
        throw std::runtime_error("Failed to allocate scene of the desired type");
      }
      return res;
    }

  private:
    Graphics3D::SceneAllocatorFunc m_sceneAllocator;
  };
}

#endif
