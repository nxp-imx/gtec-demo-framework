#ifndef FSLGRAPHICS3D_SCENEFORMAT_BASICSCENEFORMAT_HPP
#define FSLGRAPHICS3D_SCENEFORMAT_BASICSCENEFORMAT_HPP
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
#include <FslBase/IO/Path.hpp>
#include <FslGraphics3D/BasicScene/Scene.hpp>
#include <FslGraphics3D/BasicScene/SceneAllocator.hpp>
#include <FslGraphics3D/BasicScene/SceneAllocatorFunc.hpp>
#include <deque>
#include <memory>

namespace Fsl::SceneFormat
{
  class InternalSceneRecord;

  //! @brief Class that can load and save the basic scene format.
  class BasicSceneFormat
  {
    std::shared_ptr<InternalSceneRecord> m_sceneScratchpad;
    bool m_hostIsLittleEndian;

  public:
    BasicSceneFormat(const BasicSceneFormat&) = delete;
    BasicSceneFormat& operator=(const BasicSceneFormat&) = delete;

    BasicSceneFormat();
    ~BasicSceneFormat();


    //! @brief Load the given file
    //! @param filename the file to load.
    std::shared_ptr<Graphics3D::Scene> GenericLoad(const IO::Path& filename, const Graphics3D::SceneAllocatorFunc& sceneAllocator,
                                                   const void* const pDstDefaultValues, const int32_t cbDstDefaultValues);

    //! @brief Load the scene from the stream
    //! @param rStream the stream to load the scene from
    std::shared_ptr<Graphics3D::Scene> GenericLoad(std::ifstream& rStream, const Graphics3D::SceneAllocatorFunc& sceneAllocator,
                                                   const void* const pDstDefaultValues, const int32_t cbDstDefaultValues);


    //! @brief Load the given file
    //! @param filename the file to load.
    template <typename TScene>
    std::shared_ptr<TScene> Load(const IO::Path& filename)
    {
      Graphics3D::SceneAllocatorFunc sceneAllocator(Graphics3D::SceneAllocator::Allocate<TScene>);
      typename TScene::mesh_type::vertex_type defaultVertex;
      auto res =
        std::dynamic_pointer_cast<TScene>(GenericLoad(filename, sceneAllocator, &defaultVertex, sizeof(typename TScene::mesh_type::vertex_type)));
      if (!res)
      {
        throw std::runtime_error("Failed to allocate scene of the desired type");
      }
      return res;
    }

    //! @brief Save scene to file
    void Save(const IO::Path& strFilename, const Graphics3D::Scene& scene);

    //! @brief Save scene to stream
    //! @param stream the binary stream to save to.
    //! @param scene the scene to be saved.
    void Save(std::ofstream& rStream, const Graphics3D::Scene& scene);
  };
}

#endif
