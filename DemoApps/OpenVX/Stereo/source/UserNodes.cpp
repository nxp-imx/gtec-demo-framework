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

// OpenVX 1.1 project
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <RapidOpenVX/Check.hpp>
#include "UserNodes.hpp"

#define VX_KERNEL_NAME_CENSUS "com.nxp.extension.CensusTransform"
#define VX_KERNEL_ENUM_CENSUS 100
#define VX_KERNEL_NAME_MATCH_LR "com.nxp.extension.stereo_matching_kernel_LeftReference"
#define VX_KERNEL_ENUM_MATCH_LR 101
#define VX_KERNEL_NAME_MATCH_RR "com.nxp.extension.stereo_matching_kernel_RightReference"
#define VX_KERNEL_ENUM_MATCH_RR 102
#define VX_KERNEL_NAME_OUTLIERS "com.nxp.extension.outlier_detection_naive"
#define VX_KERNEL_ENUM_OUTLIERS 103
#define VX_KERNEL_NAME_LEFT_RIGHT_CONSISTENCY "com.nxp.extension.left_right_consistency_kernel_naive"
#define VX_KERNEL_ENUM_LEFT_RIGHT_CONSISTENCY 104
#define VX_KERNEL_NAME_DISPARITY_REFINEMENT "com.nxp.extension.disparity_refinement_dense_kernel_naive"
#define VX_KERNEL_ENUM_DISPARITY_REFINEMENT 105
#define VX_KERNEL_NAME_MEDIAN_FILTER "com.nxp.extension.median_filter_kernel_3x3"
#define VX_KERNEL_ENUM_MEDIAN_FILTER 106
#define VX_KERNEL_NAME_FINER_MATCHING_LR "com.nxp.extension.stereo_matching_finer_kernel_LeftReference"
#define VX_KERNEL_ENUM_FINER_MATCHING_LR 107
#define VX_KERNEL_NAME_FINER_MATCHING_RR "com.nxp.extension.stereo_matching_finer_kernel_RightReference"
#define VX_KERNEL_ENUM_FINER_MATCHING_RR 108

namespace Fsl
{
  UserNodes::UserNodes(const std::shared_ptr<IContentManager>& contentManager)
    : m_contentManager(contentManager)
  {
  }


  UserNodes::~UserNodes() = default;

  vx_node UserNodes::NXPCensusNode(vx_graph graph, vx_image in, vx_image out)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_uint32 index = 0u;

    context = vxGetContext(reinterpret_cast<vx_reference>(graph));
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)context));

    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_CENSUS);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status1 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(in));
    index++;
    vx_status status2 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(out));
    if ((status1 != VX_SUCCESS) || (status2 != VX_SUCCESS))
    {
      RAPIDOPENVX_CHECK(vxReleaseNode(&node));
    }
    RAPIDOPENVX_CHECK(vxReleaseKernel(&kernel));

    return node;
  }

  vx_status UserNodes::VXInputValidator(vx_node nodeObj, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;

    if (index == 0u)
    {
      vx_image imgObj = nullptr;
      vx_df_image imgFmt = 0;
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        vx_int32 ref = VX_PARAMETER_REF;
        if (VX_SUCCESS == vxQueryParameter(paramObj, ref, &imgObj, sizeof(vx_image)))
        {
          vx_int32 fmt = VX_IMAGE_FORMAT;
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, fmt, &imgFmt, sizeof(imgFmt)));
          if (VX_DF_IMAGE_U8 == imgFmt)
          {
            status = VX_SUCCESS;
          }
          else
          {
            status = VX_ERROR_INVALID_VALUE;
          }
          RAPIDOPENVX_CHECK(vxReleaseImage(&imgObj));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }

    return status;
  }

  vx_status UserNodes::VXOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_uint32 width = 0u;
    vx_uint32 height = 0u;
    vx_df_image imgFmt = VX_DF_IMAGE_U8;

    if (index == 1u)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if (vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image)) == VX_SUCCESS)
        {
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width)));
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height)));
          RAPIDOPENVX_CHECK(vxReleaseImage(&imgObj));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }
    return VX_SUCCESS;
  }

  vx_status UserNodes::VXValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_uint32 index = 0u;
    for (index = 0u; index < num; index++)
    {
      if (index == 1u)
      {
        RAPIDOPENVX_CHECK(VXOutputValidator(node, index, metas[index]));
      }
      else
      {
        RAPIDOPENVX_CHECK(VXInputValidator(node, index));
      }
    }
    return VX_SUCCESS;
  }

  vx_status UserNodes::VXCensusInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    if (paraNum == 0u)
    {
      FSLLOG3_ERROR("Error numbers of parameters.");
    }
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_uint32 imgWid = 0u;
    vx_uint32 imgHei = 0u;
    auto imgObj = reinterpret_cast<vx_image>(paramObj[0]);

    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32)));
    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32)));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 16;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      ((((imgWid + shaderParam.globalWorkScale[0] - 1u) / shaderParam.globalWorkScale[0]) + shaderParam.localWorkSize[0] - 1u) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1u) / shaderParam.globalWorkScale[1];

    RAPIDOPENVX_CHECK(
      vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t)));
    return VX_SUCCESS;
  }

  vx_node UserNodes::NXPMatchLRNode(vx_graph graph, vx_image inputCensusL, vx_image inputCensusR, vx_image outputDisparityLeft,
                                    const vx_int32 minDisparity, const vx_int32 maxDisparity)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_uint32 index = 0u;

    context = vxGetContext(reinterpret_cast<vx_reference>(graph));
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)context));

    vx_scalar sMinDisparity = vxCreateScalar(context, VX_TYPE_INT32, &minDisparity);
    vx_scalar sMaxDisparity = vxCreateScalar(context, VX_TYPE_INT32, &maxDisparity);

    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_MATCH_LR);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status1 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(inputCensusL));
    index++;
    vx_status status2 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(inputCensusR));
    index++;
    vx_status status3 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(outputDisparityLeft));
    index++;
    vx_status status4 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sMinDisparity));
    index++;
    vx_status status5 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sMaxDisparity));
    vx_int32 retSuccess = VX_SUCCESS;
    if ((status1 != retSuccess) || (status2 != retSuccess) || (status3 != retSuccess) || (status4 != retSuccess) || (status5 != retSuccess))
    {
      RAPIDOPENVX_CHECK(vxReleaseNode(&node));
    }

    RAPIDOPENVX_CHECK(vxReleaseScalar(&sMinDisparity));
    RAPIDOPENVX_CHECK(vxReleaseScalar(&sMaxDisparity));
    RAPIDOPENVX_CHECK(vxReleaseKernel(&kernel));

    return node;
  }

  vx_node UserNodes::NXPMatchRRNode(vx_graph graph, vx_image inputCensusL, vx_image inputCensusR, vx_image outputDisparityRight,
                                    const vx_int32 minDisparity, const vx_int32 maxDisparity)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_uint32 index = 0u;

    context = vxGetContext(reinterpret_cast<vx_reference>(graph));
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)context));

    vx_scalar sMinDisparity = vxCreateScalar(context, VX_TYPE_INT32, &minDisparity);
    vx_scalar sMaxDisparity = vxCreateScalar(context, VX_TYPE_INT32, &maxDisparity);

    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_MATCH_RR);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status1 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(inputCensusL));
    index++;
    vx_status status2 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(inputCensusR));
    index++;
    vx_status status3 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(outputDisparityRight));
    index++;
    vx_status status4 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sMinDisparity));
    index++;
    vx_status status5 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sMaxDisparity));
    vx_int32 retSuccess = VX_SUCCESS;
    if ((status1 != retSuccess) || (status2 != retSuccess) || (status3 != retSuccess) || (status4 != retSuccess) || (status5 != retSuccess))
    {
      RAPIDOPENVX_CHECK(vxReleaseNode(&node));
    }

    RAPIDOPENVX_CHECK(vxReleaseScalar(&sMinDisparity));
    RAPIDOPENVX_CHECK(vxReleaseScalar(&sMaxDisparity));
    RAPIDOPENVX_CHECK(vxReleaseKernel(&kernel));

    return node;
  }

  vx_status UserNodes::VXMatchInputValidator(vx_node nodeObj, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;

    if (index == 0u || index == 1u)
    {
      vx_image imgObj = nullptr;
      vx_df_image imgFmt = 0;
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if (vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image)) == VX_SUCCESS)
        {
          vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));
          if (VX_DF_IMAGE_U8 == imgFmt)
          {
            status = VX_SUCCESS;
          }
          else
          {
            status = VX_ERROR_INVALID_VALUE;
          }
          RAPIDOPENVX_CHECK(vxReleaseImage(&imgObj));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }
    else if ((index == 3u) || (index == 4u))
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        vx_scalar scalar = nullptr;
        vx_enum stype = 0;
        if (VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &scalar, sizeof(scalar)))
        {
          vxQueryScalar(scalar, VX_SCALAR_TYPE, &stype, sizeof(stype));
          if ((VX_TYPE_INT32 == stype) || (VX_TYPE_UINT32 == stype))
          {
            status = VX_SUCCESS;
          }
          else
          {
            status = VX_ERROR_INVALID_TYPE;
          }
          RAPIDOPENVX_CHECK(vxReleaseScalar(&scalar));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }

    return status;
  }

  vx_status UserNodes::VXMatchOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_uint32 width = 0u;
    vx_uint32 height = 0u;
    vx_df_image imgFmt = VX_DF_IMAGE_U8;

    if (index == 2u)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width)));
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height)));

          RAPIDOPENVX_CHECK(vxReleaseImage(&imgObj));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }
    return VX_SUCCESS;
  }

  vx_status UserNodes::VXMatchValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_uint32 index = 0u;
    for (index = 0u; index < num; index++)
    {
      if (index == 2u)
      {
        RAPIDOPENVX_CHECK(VXMatchOutputValidator(node, index, metas[index]));
      }
      else
      {
        RAPIDOPENVX_CHECK(VXMatchInputValidator(node, index));
      }
    }
    return VX_SUCCESS;
  }

  vx_status UserNodes::VXMatchInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    if (paraNum == 0u)
    {
      FSLLOG3_ERROR("Error numbers of parameters.");
    }
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_uint32 imgWid = 0u;
    vx_uint32 imgHei = 0u;
    auto imgObj = reinterpret_cast<vx_image>(paramObj[2]);

    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32)));
    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32)));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 4;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      ((((imgWid + shaderParam.globalWorkScale[0] - 1u) / shaderParam.globalWorkScale[0]) + shaderParam.localWorkSize[0] - 1u) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1u) / shaderParam.globalWorkScale[1];

    RAPIDOPENVX_CHECK(
      vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t)));
    return VX_SUCCESS;
  }

  vx_node UserNodes::NXPOutliersDetectionNode(vx_graph graph, vx_image disparityInput, vx_image disparytyOutput)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_uint32 index = 0u;

    context = vxGetContext(reinterpret_cast<vx_reference>(graph));
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)context));

    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_OUTLIERS);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status1 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(disparityInput));
    index++;
    vx_status status2 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(disparytyOutput));
    if ((status1 != VX_SUCCESS) || (status2 != VX_SUCCESS))
    {
      RAPIDOPENVX_CHECK(vxReleaseNode(&node));
    }

    RAPIDOPENVX_CHECK(vxReleaseKernel(&kernel));

    return node;
  }

  vx_status UserNodes::VXOutliersInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    if (paraNum == 0u)
    {
      FSLLOG3_ERROR("Error numbers of parameters.");
    }
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_uint32 imgWid = 0u;
    vx_uint32 imgHei = 0u;
    auto imgObj = reinterpret_cast<vx_image>(paramObj[0]);

    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32)));
    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32)));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 2;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      ((((imgWid + shaderParam.globalWorkScale[0] - 1u) / shaderParam.globalWorkScale[0]) + shaderParam.localWorkSize[0] - 1u) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1u) / shaderParam.globalWorkScale[1];

    RAPIDOPENVX_CHECK(
      vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t)));
    return VX_SUCCESS;
  }

  vx_node UserNodes::NXPLeftRightConsistencyNode(vx_graph graph, vx_image inL, vx_image inR, vx_image outL, vx_image outR)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_uint32 index = 0u;

    context = vxGetContext(reinterpret_cast<vx_reference>(graph));
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)context));

    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_LEFT_RIGHT_CONSISTENCY);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status1 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(inL));
    index++;
    vx_status status2 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(inR));
    index++;
    vx_status status3 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(outL));
    index++;
    vx_status status4 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(outR));
    vx_int32 retSuccess = VX_SUCCESS;
    if ((status1 != retSuccess) || (status2 != retSuccess) || (status3 != retSuccess) || (status4 != retSuccess))
    {
      RAPIDOPENVX_CHECK(vxReleaseNode(&node));
    }

    RAPIDOPENVX_CHECK(vxReleaseKernel(&kernel));

    return node;
  }

  vx_status UserNodes::VXConsistencyInputValidator(vx_node nodeObj, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_int32 retSuccess = VX_SUCCESS;
    vx_parameter paramObj = nullptr;

    if ((index == 0u) || (index == 1u))
    {
      vx_image imgObj = nullptr;
      vx_df_image imgFmt = 0;
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if ((retSuccess == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt)));
          if (VX_DF_IMAGE_U8 == imgFmt)
          {
            status = retSuccess;
          }
          else
          {
            status = VX_ERROR_INVALID_VALUE;
          }
          RAPIDOPENVX_CHECK(vxReleaseImage(&imgObj));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }

    return status;
  }

  vx_status UserNodes::VXConsistencyOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_uint32 width = 0u;
    vx_uint32 height = 0u;
    vx_df_image imgFmt = VX_DF_IMAGE_U8;

    if ((index == 2u) || (index == 3u))
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if (VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image)))
        {
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width)));
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height)));

          RAPIDOPENVX_CHECK(vxReleaseImage(&imgObj));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }
    return VX_SUCCESS;
  }

  vx_status UserNodes::VXConsistencyValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_status status = VX_SUCCESS;
    vx_uint32 index = 0u;
    for (index = 0u; index < num; index++)
    {
      if ((index == 2u) || (index == 3u))
      {
        status = VXConsistencyOutputValidator(node, index, metas[index]);
      }
      else
      {
        status = VXConsistencyInputValidator(node, index);
      }
    }
    return status;
  }

  vx_status UserNodes::VXConsistencyInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    if (paraNum == 0u)
    {
      FSLLOG3_ERROR("Error numbers of parameters.");
    }
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_uint32 imgWid = 0u;
    vx_uint32 imgHei = 0u;
    auto imgObj = reinterpret_cast<vx_image>(paramObj[0]);

    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32)));
    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32)));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 2;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      ((((imgWid + shaderParam.globalWorkScale[0] - 1u) / shaderParam.globalWorkScale[0]) + shaderParam.localWorkSize[0] - 1u) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1u) / shaderParam.globalWorkScale[1];

    RAPIDOPENVX_CHECK(
      vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t)));
    return VX_SUCCESS;
  }

  vx_node UserNodes::NXPDisparityRefinementNode(vx_graph graph, vx_image imageIn, vx_image imageOut)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_uint32 index = 0U;

    context = vxGetContext(reinterpret_cast<vx_reference>(graph));
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)context));

    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_DISPARITY_REFINEMENT);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status1 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(imageIn));
    index++;
    vx_status status2 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(imageOut));
    if ((status1 != VX_SUCCESS) || (status2 != VX_SUCCESS))
    {
      RAPIDOPENVX_CHECK(vxReleaseNode(&node));
    }

    RAPIDOPENVX_CHECK(vxReleaseKernel(&kernel));

    return node;
  }

  vx_status UserNodes::VXDisparityInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    if (paraNum == 0u)
    {
      FSLLOG3_ERROR("Error numbers of parameters.");
    }
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_uint32 imgWid = 0u;
    vx_uint32 imgHei = 0u;
    auto imgObj = reinterpret_cast<vx_image>(paramObj[0]);

    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32)));
    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32)));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 2;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      ((((imgWid + shaderParam.globalWorkScale[0] - 1u) / shaderParam.globalWorkScale[0]) + shaderParam.localWorkSize[0] - 1u) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1u) / shaderParam.globalWorkScale[1];

    RAPIDOPENVX_CHECK(
      vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t)));
    return VX_SUCCESS;
  }

  vx_node UserNodes::NXPMedianFilterNode(vx_graph graph, vx_image imageIn, vx_image imageOut)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_uint32 index = 0u;

    context = vxGetContext(reinterpret_cast<vx_reference>(graph));
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)context));

    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_MEDIAN_FILTER);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(imageIn));
    index++;
    status |= vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(imageOut));
    if (status != VX_SUCCESS)
    {
      RAPIDOPENVX_CHECK(vxReleaseNode(&node));
    }

    RAPIDOPENVX_CHECK(vxReleaseKernel(&kernel));

    return node;
  }

  vx_status UserNodes::VXMedianInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    if (paraNum == 0u)
    {
      FSLLOG3_ERROR("Error numbers of parameters.");
    }
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_uint32 imgWid = 0u;
    vx_uint32 imgHei = 0u;
    auto imgObj = reinterpret_cast<vx_image>(paramObj[0]);

    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32)));
    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32)));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 14;
    shaderParam.globalWorkScale[1] = imgHei;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      ((((imgWid + shaderParam.globalWorkScale[0] - 1u) / shaderParam.globalWorkScale[0]) + shaderParam.localWorkSize[0] - 1u) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1u) / shaderParam.globalWorkScale[1];

    RAPIDOPENVX_CHECK(vxSetNodeUniform(nodObj, "height", 1, &imgHei));

    RAPIDOPENVX_CHECK(
      vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t)));
    return VX_SUCCESS;
  }

  vx_node UserNodes::NXPMatchFinerLRNode(vx_graph graph, const vx_image censusL, const vx_image censusR, vx_image outputLeft, const vx_int32 minDis,
                                         const vx_int32 maxDis, const vx_image estimateLeft, const vx_image estimateRight,
                                         const vx_int32 rangeVariation)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_uint32 index = 0u;

    context = vxGetContext(reinterpret_cast<vx_reference>(graph));
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)context));

    vx_scalar sMinDisparity = vxCreateScalar(context, VX_TYPE_INT32, &minDis);
    vx_scalar sMaxDisparity = vxCreateScalar(context, VX_TYPE_INT32, &maxDis);
    vx_scalar sRangeVariation = vxCreateScalar(context, VX_TYPE_INT32, &rangeVariation);

    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_FINER_MATCHING_LR);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status1 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(censusL));
    index++;
    vx_status status2 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(censusR));
    index++;
    vx_status status3 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(outputLeft));
    index++;
    vx_status status4 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sMinDisparity));
    index++;
    vx_status status5 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sMaxDisparity));
    index++;
    vx_status status6 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(estimateLeft));
    index++;
    vx_status status7 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(estimateRight));
    index++;
    vx_status status8 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sRangeVariation));
    vx_status retSuccess = VX_SUCCESS;
    if ((status1 != retSuccess) || (status2 != retSuccess) || (status3 != retSuccess) || (status4 != retSuccess) || (status5 != retSuccess) ||
        (status6 != retSuccess) || (status7 != retSuccess) || (status8 != retSuccess))
    {
      RAPIDOPENVX_CHECK(vxReleaseNode(&node));
    }

    RAPIDOPENVX_CHECK(vxReleaseScalar(&sMinDisparity));
    RAPIDOPENVX_CHECK(vxReleaseScalar(&sMaxDisparity));
    RAPIDOPENVX_CHECK(vxReleaseScalar(&sRangeVariation));
    RAPIDOPENVX_CHECK(vxReleaseKernel(&kernel));

    return node;
  }

  vx_node UserNodes::NXPMatchFinerRRNode(vx_graph graph, const vx_image censusL, const vx_image censusR, vx_image outputRight, const vx_int32 minDis,
                                         const vx_int32 maxDis, const vx_image estimateLeft, const vx_image estimateRight,
                                         const vx_int32 rangeVariation)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_uint32 index = 0u;

    context = vxGetContext(reinterpret_cast<vx_reference>(graph));
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)context));

    vx_scalar sMinDisparity = vxCreateScalar(context, VX_TYPE_INT32, &minDis);
    vx_scalar sMaxDisparity = vxCreateScalar(context, VX_TYPE_INT32, &maxDis);
    vx_scalar sRangeVariation = vxCreateScalar(context, VX_TYPE_INT32, &rangeVariation);

    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_FINER_MATCHING_RR);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status1 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(censusL));
    index++;
    vx_status status2 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(censusR));
    index++;
    vx_status status3 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(outputRight));
    index++;
    vx_status status4 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sMinDisparity));
    index++;
    vx_status status5 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sMaxDisparity));
    index++;
    vx_status status6 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(estimateLeft));
    index++;
    vx_status status7 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(estimateRight));
    index++;
    vx_status status8 = vxSetParameterByIndex(node, index, reinterpret_cast<vx_reference>(sRangeVariation));
    vx_status retSuccess = VX_SUCCESS;
    if ((status1 != retSuccess) || (status2 != retSuccess) || (status3 != retSuccess) || (status4 != retSuccess) || (status5 != retSuccess) ||
        (status6 != retSuccess) || (status7 != retSuccess) || (status8 != retSuccess))
    {
      RAPIDOPENVX_CHECK(vxReleaseNode(&node));
    }

    RAPIDOPENVX_CHECK(vxReleaseScalar(&sMinDisparity));
    RAPIDOPENVX_CHECK(vxReleaseScalar(&sMaxDisparity));
    RAPIDOPENVX_CHECK(vxReleaseScalar(&sRangeVariation));
    RAPIDOPENVX_CHECK(vxReleaseKernel(&kernel));

    return node;
  }

  vx_status UserNodes::VXMatchFinerInputValidator(vx_node nodeObj, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;

    if ((index == 0u) || (index == 1u) || (index == 5u) || (index == 6u))
    {
      vx_image imgObj = nullptr;
      vx_df_image imgFmt = 0;
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if (VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image)))
        {
          vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));
          if (VX_DF_IMAGE_U8 == imgFmt)
          {
            status = VX_SUCCESS;
          }
          else
          {
            status = VX_ERROR_INVALID_VALUE;
          }
          RAPIDOPENVX_CHECK(vxReleaseImage(&imgObj));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }
    else
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        vx_scalar scalar = nullptr;
        vx_enum stype = 0;
        if (VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &scalar, sizeof(scalar)))
        {
          RAPIDOPENVX_CHECK(vxQueryScalar(scalar, VX_SCALAR_TYPE, &stype, sizeof(stype)));
          if ((VX_TYPE_INT32 == stype) || (VX_TYPE_UINT32 == stype))
          {
            status = VX_SUCCESS;
          }
          else
          {
            status = VX_ERROR_INVALID_TYPE;
          }
          RAPIDOPENVX_CHECK(vxReleaseScalar(&scalar));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }

    return status;
  }

  vx_status UserNodes::VXMatchFinerOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_uint32 width = 0u;
    vx_uint32 height = 0u;
    vx_df_image imgFmt = VX_DF_IMAGE_U8;

    if (index == 2u)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if (VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image)))
        {
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width)));
          RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width)));
          RAPIDOPENVX_CHECK(vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height)));

          RAPIDOPENVX_CHECK(vxReleaseImage(&imgObj));
        }
        RAPIDOPENVX_CHECK(vxReleaseParameter(&paramObj));
      }
    }
    return status;
  }

  vx_status UserNodes::VXMatchFinerValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_status status = VX_SUCCESS;
    vx_uint32 index = 0u;
    for (index = 0u; index < num; index++)
    {
      if (index == 2u)
      {
        status = VXMatchFinerOutputValidator(node, index, metas[index]);
      }
      else
      {
        status = VXMatchFinerInputValidator(node, index);
      }
    }
    return status;
  }

  vx_status UserNodes::VXMatchFinerInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    if (paraNum == 0u)
    {
      FSLLOG3_ERROR("Error numbers of parameters.");
    }
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_uint32 imgWid = 0u;
    vx_uint32 imgHei = 0u;
    auto imgObj = reinterpret_cast<vx_image>(paramObj[2]);

    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32)));
    RAPIDOPENVX_CHECK(vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32)));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 2;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      ((((imgWid + shaderParam.globalWorkScale[0] - 1u) / shaderParam.globalWorkScale[0]) + shaderParam.localWorkSize[0] - 1u) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1u) / shaderParam.globalWorkScale[1];

    RAPIDOPENVX_CHECK(
      vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t)));
    return VX_SUCCESS;
  }

  vx_status UserNodes::VXPublishNodeKernels(vx_context contextVX)
  {
    vx_param_description_t vxKernelParam[] = {{VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                              {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED}};

    vx_param_description_t vxMatchKernelParam[] = {{VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                                   {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                                   {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                                   {VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED},
                                                   {VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED}};

    vx_param_description_t vxConsistencyKernelParam[] = {{VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                                         {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                                         {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                                         {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED}};

    vx_param_description_t vxMatchFinerKernelParam[] = {
      {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},  {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
      {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED}, {VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED},
      {VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED}, {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
      {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},  {VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED}};

    vx_kernel_description_t vxCensusKernelVXCInfo = {VX_KERNEL_ENUM_CENSUS,
                                                     VX_KERNEL_NAME_CENSUS,
                                                     nullptr,
                                                     vxKernelParam,
                                                     static_cast<vx_uint32>(sizeof(vxKernelParam) / sizeof(vxKernelParam[0])),
                                                     VXValidator,
                                                     nullptr,
                                                     nullptr,
                                                     VXCensusInitializer,
                                                     nullptr};

    vx_kernel_description_t vxMatchLRKernelVXCInfo = {VX_KERNEL_ENUM_MATCH_LR,
                                                      VX_KERNEL_NAME_MATCH_LR,
                                                      nullptr,
                                                      vxMatchKernelParam,
                                                      static_cast<vx_uint32>(sizeof(vxMatchKernelParam) / sizeof(vxMatchKernelParam[0])),
                                                      VXMatchValidator,
                                                      nullptr,
                                                      nullptr,
                                                      VXMatchInitializer,
                                                      nullptr};
    vx_kernel_description_t vxMatchRRKernelVXCInfo = {VX_KERNEL_ENUM_MATCH_RR,
                                                      VX_KERNEL_NAME_MATCH_RR,
                                                      nullptr,
                                                      vxMatchKernelParam,
                                                      static_cast<vx_uint32>(sizeof(vxMatchKernelParam) / sizeof(vxMatchKernelParam[0])),
                                                      VXMatchValidator,
                                                      nullptr,
                                                      nullptr,
                                                      VXMatchInitializer,
                                                      nullptr};

    vx_kernel_description_t vxOutliersKernelVXCInfo = {VX_KERNEL_ENUM_OUTLIERS,
                                                       VX_KERNEL_NAME_OUTLIERS,
                                                       nullptr,
                                                       vxKernelParam,
                                                       static_cast<vx_uint32>(sizeof(vxKernelParam) / sizeof(vxKernelParam[0])),
                                                       VXValidator,
                                                       nullptr,
                                                       nullptr,
                                                       VXOutliersInitializer,
                                                       nullptr};

    vx_kernel_description_t vxConsistencyKernelVXCInfo = {
      VX_KERNEL_ENUM_LEFT_RIGHT_CONSISTENCY,
      VX_KERNEL_NAME_LEFT_RIGHT_CONSISTENCY,
      nullptr,
      vxConsistencyKernelParam,
      static_cast<vx_uint32>(sizeof(vxConsistencyKernelParam) / sizeof(vxConsistencyKernelParam[0])),
      VXConsistencyValidator,
      nullptr,
      nullptr,
      VXConsistencyInitializer,
      nullptr};

    vx_kernel_description_t vxDisparityRefinementKernelVXCInfo = {VX_KERNEL_ENUM_DISPARITY_REFINEMENT,
                                                                  VX_KERNEL_NAME_DISPARITY_REFINEMENT,
                                                                  nullptr,
                                                                  vxKernelParam,
                                                                  static_cast<vx_uint32>(sizeof(vxKernelParam) / sizeof(vxKernelParam[0])),
                                                                  VXValidator,
                                                                  nullptr,
                                                                  nullptr,
                                                                  VXDisparityInitializer,
                                                                  nullptr};

    vx_kernel_description_t vxMedianFilterKernelVXCInfo = {VX_KERNEL_ENUM_MEDIAN_FILTER,
                                                           VX_KERNEL_NAME_MEDIAN_FILTER,
                                                           nullptr,
                                                           vxKernelParam,
                                                           static_cast<vx_uint32>(sizeof(vxKernelParam) / sizeof(vxKernelParam[0])),
                                                           VXValidator,
                                                           nullptr,
                                                           nullptr,
                                                           VXMedianInitializer,
                                                           nullptr};

    vx_kernel_description_t vxMatchFinerLRKernelVXCInfo = {
      VX_KERNEL_ENUM_FINER_MATCHING_LR,
      VX_KERNEL_NAME_FINER_MATCHING_LR,
      nullptr,
      vxMatchFinerKernelParam,
      static_cast<vx_uint32>(sizeof(vxMatchFinerKernelParam) / sizeof(vxMatchFinerKernelParam[0])),
      VXMatchFinerValidator,
      nullptr,
      nullptr,
      VXMatchFinerInitializer,
      nullptr};

    vx_kernel_description_t vxMatchFinerRRKernelVXCInfo = {
      VX_KERNEL_ENUM_FINER_MATCHING_RR,
      VX_KERNEL_NAME_FINER_MATCHING_RR,
      nullptr,
      vxMatchFinerKernelParam,
      static_cast<vx_uint32>(sizeof(vxMatchFinerKernelParam) / sizeof(vxMatchFinerKernelParam[0])),
      VXMatchFinerValidator,
      nullptr,
      nullptr,
      VXMatchFinerInitializer,
      nullptr};

    vx_kernel_description_t* kernels[9] = {&vxCensusKernelVXCInfo,       &vxMatchLRKernelVXCInfo,      &vxMatchRRKernelVXCInfo,
                                           &vxOutliersKernelVXCInfo,     &vxConsistencyKernelVXCInfo,  &vxDisparityRefinementKernelVXCInfo,
                                           &vxMedianFilterKernelVXCInfo, &vxMatchFinerLRKernelVXCInfo, &vxMatchFinerRRKernelVXCInfo};

    vx_status status = VX_FAILURE;
    vx_kernel kernelObj = nullptr;

    vx_program programObj = nullptr;
    std::string programStr = m_contentManager->ReadAllText("CTF_kernels_vxc.vx");
    const vx_char* programSrc[] = {programStr.c_str()};
    auto programLen = static_cast<vx_uint64>(programStr.size());

    programObj = vxCreateProgramWithSource(contextVX, 1, programSrc, &programLen);
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)programObj));

    IO::Path contentPath = m_contentManager->GetContentPath();
    std::string buildOptionStr = "-cl-viv-vx-extension -I ";
    buildOptionStr = buildOptionStr + contentPath.ToUTF8String();
    const vx_char* buildOptions = buildOptionStr.c_str();
    RAPIDOPENVX_CHECK(vxBuildProgram(programObj, buildOptions));

    std::size_t kernelNum = 0;
    kernelNum = sizeof(kernels) / sizeof(kernels[0]);
    for (std::size_t i = 0; i < kernelNum; i++)
    {
      kernelObj = vxAddKernelInProgram(programObj, kernels[i]->name, kernels[i]->enumeration, kernels[i]->numParams, kernels[i]->validate,
                                       kernels[i]->initialize, kernels[i]->deinitialize);
      RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)kernelObj));

      status = VX_SUCCESS;
      for (std::size_t j = 0; j < kernels[i]->numParams; j++)
      {
        RAPIDOPENVX_CHECK(vxAddParameterToKernel(kernelObj, j, kernels[i]->parameters[j].direction, kernels[i]->parameters[j].data_type,
                                                 kernels[i]->parameters[j].state));
      }
      if (VX_SUCCESS == status)
      {
        RAPIDOPENVX_CHECK(vxFinalizeKernel(kernelObj));
        FSLLOG3_INFO("Succeeded to finalize kernel[{}]= {}", i, kernels[i]->name);
      }
      else
      {
        RAPIDOPENVX_CHECK(vxRemoveKernel(kernelObj));
      }
    }
    return status;
  }
}
