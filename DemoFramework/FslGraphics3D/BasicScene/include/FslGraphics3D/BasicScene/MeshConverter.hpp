#ifndef FSLGRAPHICS3D_BASICSCENE_MESHCONVERTER_HPP
#define FSLGRAPHICS3D_BASICSCENE_MESHCONVERTER_HPP
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
#include <FslGraphics3D/BasicScene/Mesh.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    class MeshConverter
    {
    public:
      //! @brief Convert from one mesh format to another. This method exist for convenience and its performance is probably pretty bad!
      //! @param rDst the mesh that should be copied to (will be resized to fit the src mesh).
      //! @param src the mesh that should be copied from.
      //! @note Fields that are present in dst but not in src are filled with the supplied default values. Src fields that isn't present in the dst format will be ignored.
      //! @note If you use primitive restart and convert to a small index size then be sure that the primitive restart value isn't used accidentally
      //! @note Index size conversion does not do any compacting of the values, the exact index value is converted, if it doesn't fit a exception will be thrown.
      template<typename TDstMesh>
      static void Convert(TDstMesh& rDst, const Mesh& src)
      {
        typename TDstMesh::vertex_type dstDefaultValue;
        GenericConvert(rDst, src, &dstDefaultValue, sizeof(typename TDstMesh::vertex_type));
      }


      //! @brief Convert from one mesh format to another. This method exist for convenience and its performance is probably pretty bad!
      //! @param rDst the mesh that should be copied to (will be resized to fit the src mesh).
      //! @param src the mesh that should be copied from.
      //! @param dstDefaultValues holds the default values which is used to fill in required fields in a dst vertex which are missing from a src vertex
      //! @note Fields that are present in dst but not in src are filled with the supplied default values. Src fields that isn't present in the dst format will be ignored.
      //! @note If you use primitive restart and convert to a small index size then be sure that the primitive restart value isn't used accidentally
      //! @note Index size conversion does not do any compacting of the values, the exact index value is converted, if it doesn't fit a exception will be thrown.
      template<typename TDstMesh>
      static void Convert(TDstMesh& rDst, const Mesh& src, const typename TDstMesh::vertex_type& dstDefaultValue)
      {
        GenericConvert(rDst, src, &dstDefaultValue, sizeof(typename TDstMesh::vertex_type));
      }


      //! @brief Convert from one mesh format to another. This method exist for convenience and its performance is probably pretty bad!
      //! @param rDst the mesh that should be copied to (will be resized to fit the src mesh).
      //! @param src the mesh that should be copied from.
      //! @param pDstDefaultValues points to one dst type vertex that holds the default values which is used to fill in required fields in a dst vertex which are missing from a src vertex
      //! @param cbDstDefaultValues the number of bytes used for the default vertex.
      //! @note Fields that are present in dst but not in src are filled with the supplied default values. Src fields that isn't present in the dst format will be ignored.
      //! @note If you use primitive restart and convert to a small index size then be sure that the primitive restart value isn't used accidentally
      //! @note Index size conversion does not do any compacting of the values, the exact index value is converted, if it doesn't fit a exception will be thrown.
      static void GenericConvert(Mesh& rDst, const Mesh& src, const void*const pDstDefaultValues, const int32_t cbDstDefaultValues);
    };
  }
}

#endif
