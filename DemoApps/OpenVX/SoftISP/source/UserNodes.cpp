/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <RapidOpenVX/Check.hpp>
#include "UserNodes.hpp"

#define VX_KERNEL_NAME_BADPIXEL "com.nxp.extension.badpixel"
#define VX_KERNEL_ENUM_BADPIXEL 100
#define VX_KERNEL_NAME_HISTOGRAM "com.nxp.extension.histogram"
#define VX_KERNEL_ENUM_HISTOGRAM 101
#define VX_KERNEL_NAME_EQUALIZEHIST0 "com.nxp.extension.EqualizeHist0"
#define VX_KERNEL_ENUM_EQUALIZEHIST0 104
#define VX_KERNEL_NAME_WHITEBALANCE "com.nxp.extension.whitebalance"
#define VX_KERNEL_ENUM_WHITEBALANCE 102
#define VX_KERNEL_NAME_EQUALIZEHIST "com.nxp.extension.EqualizeHist"
#define VX_KERNEL_ENUM_EQUALIZEHIST 103
#define VX_KERNEL_NAME_DEBAYER "com.nxp.extension.debayer"
#define VX_KERNEL_ENUM_DEBAYER 105
#define VX_KERNEL_NAME_RGBA2YUV "com.nxp.extension.rgba2yuv"
#define VX_KERNEL_ENUM_RGBA2YUV 106
#define VX_KERNEL_NAME_BILATERAL "com.nxp.extension.bilateral"
#define VX_KERNEL_ENUM_BILATERAL 107
#define VX_KERNEL_NAME_YUV2RGBA "com.nxp.extension.yuv2rgba"
#define VX_KERNEL_ENUM_YUV2RGBA 108

#define gcmALIGN_NP2(n, align) (((n) + (align)-1) - (((n) + (align)-1) % (align)))

namespace Fsl
{
  UserNodes::UserNodes(const std::shared_ptr<IContentManager>& contentManager)
    : m_contentManager(contentManager)
  {
    m_denoiseEn = false;
  }


  UserNodes::~UserNodes()
  {
  }


  vx_node UserNodes::nxpBadPixelDetectNode(vx_graph graph, vx_image in, vx_image out)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_int32 index = 0;

    context = vxGetContext((vx_reference)graph);
    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_BADPIXEL);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, index++, (vx_reference)in);
    status |= vxSetParameterByIndex(node, index++, (vx_reference)out);
    if (status != VX_SUCCESS)
    {
      status = VX_ERROR_INVALID_PARAMETERS;
      vxReleaseNode(&node);
    }
    vxReleaseKernel(&kernel);

    return node;
  }


  vx_status UserNodes::vxInputValidator(vx_node nodeObj, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_df_image imgFmt = 0;

    if (index == 0)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));
          if (VX_DF_IMAGE_U8 == imgFmt)
            status = VX_SUCCESS;
          else
            status = VX_ERROR_INVALID_VALUE;
          vxReleaseImage(&imgObj);
        }
        vxReleaseParameter(&paramObj);
      }
    }
    return status;
  }

  vx_status UserNodes::vxOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_uint32 width = 0;
    vx_uint32 height = 0;
    vx_df_image imgFmt = VX_DF_IMAGE_U8;

    if (index == 1)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          status = vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxReleaseImage(&imgObj);
        }
        status |= vxReleaseParameter(&paramObj);
      }
    }
    return status;
  }


  vx_status UserNodes::vxValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_status status = VX_SUCCESS;
    vx_uint32 index = 0;
    for (index = 0; index < num; index++)
    {
      if (index < 1)
      {
        status |= vxInputValidator(node, index);
      }
      else
      {
        status |= vxOutputValidator(node, index, metas[index]);
      }
    }
    return status;
  }


  vx_status UserNodes::vxBadPixelDetectInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_int32 imgWid = 0, imgHei = 0;
    vx_image imgObj = (vx_image)paramObj[0];

    vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32));
    vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 8;
    shaderParam.globalWorkScale[1] = 2;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      (((imgWid + shaderParam.globalWorkScale[0] - 1) / shaderParam.globalWorkScale[0] + shaderParam.localWorkSize[0] - 1) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1) / shaderParam.globalWorkScale[1];

    vx_uint32 uni_selectM[16] = {
      0x00011111, 0x00000000,                                                                           // TCfg
      0x40600c00, 0x00000c02, 0x00000000, 0x00000000, 0x00000000,                                       // BinSelect
      0x00007400,                                                                                       // AccumType, ConstantType, and PostShift
      0x00010001, 0x00010001, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_rr0_rr2[16] = {
      0x11111111, 0x01111111,                                                                           // TCfg
      0x80400800, 0x40180400, 0x18080180, 0x08028080, 0x0000c028,                                       // BinSelect
      0x00007400,                                                                                       // AccumType, ConstantType, and PostShift
      0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00000001    // Constant
    };
    vx_uint32 uni_acc2[16] = {
      0x55555555, 0x00000055,                                                                           // TCfg
      0x48390820, 0x9c20e6a9, 0x39acdad4, 0x0000000f, 0x00000000,                                       // BinSelect
      0x00006400,                                                                                       // AccumType, ConstantType, and PostShift
      0x01010101, 0x01010101, 0x01010101, 0x01010101, 0x01010101, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_rr1[16] = {
      0x11111111, 0x01111111,                                                                           // TCfg
      0x80401000, 0x40180600, 0x18080200, 0x080280a0, 0x0000c030,                                       // BinSelect
      0x00000700,                                                                                       // AccumType, ConstantType, and PostShift
      0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00000001    // Constant
    };
    vx_uint32 uni_acc1[16] = {
      0x55555555, 0x00000055,                                                                           // TCfg
      0x48380820, 0x9b20e699, 0x39accad4, 0x0000000e, 0x00000000,                                       // BinSelect
      0x00006400,                                                                                       // AccumType, ConstantType, and PostShift
      0x01010101, 0x01010101, 0x01010101, 0x01010101, 0x01010101, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_diff[16] = {
      0x0b0b0b0b, 0x0000000b,                                                                           // TCfg
      0x22100200, 0x30024200, 0x02840026, 0x00000000, 0x00000000,                                       // BinSelect
      0x00003400,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000100, 0x00000100, 0x00000100, 0x00000100, 0x00000100, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_avg_128[16] = {
      0x00ffffff, 0x00000000,                                                                           // TCfg
      0xd4248500, 0x06958458, 0x00000000, 0x00000000, 0x00000000,                                       // BinSelect
      0x00006407,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_avg_32[16] = {
      0x00ffffff, 0x00000000,                                                                           // TCfg
      0xd4248500, 0x06958458, 0x00000000, 0x00000000, 0x00000000,                                       // BinSelect
      0x00006405,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_avg_8[16] = {
      0x00ffffff, 0x00000000,                                                                           // TCfg
      0xd4248500, 0x06958458, 0x00000000, 0x00000000, 0x00000000,                                       // BinSelect
      0x00006403,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_bb0_bb2[16] = {
      0x33333333, 0x03333333,                                                                           // TCfg
      0xc0500c01, 0x501c0500, 0x1c0901c0, 0x0902c090, 0x0000d02c,                                       // BinSelect
      0x00007400,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_bb1[16] = {
      0x33333333, 0x03333333,                                                                           // TCfg
      0xc0501401, 0x501c0700, 0x1c090240, 0x0902c0b0, 0x0000d034,                                       // BinSelect
      0x00007400,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_rg_gg0_gg2[16] = {
      0x33333333, 0x03333333,                                                                           // TCfg
      0x00404c02, 0x60181501, 0x200805c0, 0x0a028190, 0x0000c06c,                                       // BinSelect
      0x00007400,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_rg_gg1[16] = {
      0x33333333, 0x03333333,                                                                           // TCfg
      0xc0501401, 0x501c0700, 0x1c090240, 0x0902c0b0, 0x0000d034,                                       // BinSelect
      0x00007400,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_gb_gg0_gg2[16] = {
      0x33333333, 0x03333333,                                                                           // TCfg
      0xc0304801, 0x50141400, 0x1c070580, 0x09024180, 0x0000b068,                                       // BinSelect
      0x00007400,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uni_gb_gg1[16] = {
      0x33333333, 0x03333333,                                                                           // TCfg
      0x80401000, 0x40180600, 0x18080200, 0x080280a0, 0x0000c030,                                       // BinSelect
      0x00007400,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000    // Constant
    };

    vxSetNodeUniform(nodObj, "RR0_RR2_OneLines", 1, uni_rr0_rr2);
    vxSetNodeUniform(nodObj, "RR1_OneLines", 1, uni_rr1);
    vxSetNodeUniform(nodObj, "Acc1_TwoLines", 1, uni_acc1);
    vxSetNodeUniform(nodObj, "Acc2_TwoLines", 1, uni_acc2);
    vxSetNodeUniform(nodObj, "Select_M", 1, uni_selectM);
    vxSetNodeUniform(nodObj, "RM_Max_Min", 1, uni_diff);
    vxSetNodeUniform(nodObj, "BB0_BB2_OneLines", 1, uni_bb0_bb2);
    vxSetNodeUniform(nodObj, "BB1_OneLines", 1, uni_bb1);
    vxSetNodeUniform(nodObj, "RG_GG0_GG2_OneLines", 1, uni_rg_gg0_gg2);
    vxSetNodeUniform(nodObj, "RG_GG1_OneLines", 1, uni_rg_gg1);
    vxSetNodeUniform(nodObj, "GB_GG0_GG2_OneLines", 1, uni_gb_gg0_gg2);
    vxSetNodeUniform(nodObj, "GB_GG1_OneLines", 1, uni_gb_gg1);
    vxSetNodeUniform(nodObj, "Sum_Div_128", 1, uni_avg_128);
    vxSetNodeUniform(nodObj, "Sum_Div_32", 1, uni_avg_32);
    vxSetNodeUniform(nodObj, "Sum_Div_8", 1, uni_avg_8);

    vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t));
    return VX_SUCCESS;
  }


  vx_node UserNodes::nxpHistogramNode(vx_graph graph, vx_image in, vx_distribution distR, vx_distribution distG, vx_distribution distB)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;

    context = vxGetContext((vx_reference)graph);
    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_HISTOGRAM);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, 0, (vx_reference)in);
    status |= vxSetParameterByIndex(node, 1, (vx_reference)distR);
    status |= vxSetParameterByIndex(node, 2, (vx_reference)distG);
    status |= vxSetParameterByIndex(node, 3, (vx_reference)distB);

    if (status != VX_SUCCESS)
    {
      status = VX_ERROR_INVALID_PARAMETERS;
      vxReleaseNode(&node);
    }
    vxReleaseKernel(&kernel);

    return node;
  }


  vx_status UserNodes::vxhistogramInputValidator(vx_node nodeObj, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_df_image imgFmt = 0;

    if (index == 0)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));

          if (VX_DF_IMAGE_U8 == imgFmt)
            status = VX_SUCCESS;
          else
            status = VX_ERROR_INVALID_VALUE;

          vxReleaseImage(&imgObj);
        }
        vxReleaseParameter(&paramObj);
      }
    }

    return status;
  }


  vx_status UserNodes::vxhistogramOutputValidator(vx_node node, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_status status = VX_SUCCESS;
    if (index == 1)
    {
      vx_parameter dst_param = vxGetParameterByIndex(node, 1);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if (dist)
      {
        vx_size numBins = 0;
        vx_uint32 windows;
        status = vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status |= vxQueryDistribution(dist, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));

        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 2)
    {
      vx_parameter dst_param = vxGetParameterByIndex(node, 2);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if (dist)
      {
        vx_size numBins = 0;
        vx_uint32 windows;
        status |= vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status |= vxQueryDistribution(dist, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));

        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 3)
    {
      vx_parameter dst_param = vxGetParameterByIndex(node, 3);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if (dist)
      {
        vx_size numBins = 0;
        vx_uint32 windows;
        status |= vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status |= vxQueryDistribution(dist, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));

        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    status = VX_SUCCESS;
    return status;
  }


  vx_status UserNodes::vxhistogramValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_status status = VX_SUCCESS;
    vx_uint32 index = 0;
    for (index = 0; index < num; index++)
    {
      if (index < 1)
      {
        status |= vxhistogramInputValidator(node, index);
      }
      else
      {
        status |= vxhistogramOutputValidator(node, index, metas[index]);
      }
    }
    return status;
  }


  vx_status UserNodes::vxhistogramInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    vx_status status = VX_SUCCESS;
    vx_kernel_execution_parameters_t shaderParam = {
      2, {0, 0, 0}, {1, 1, 0}, {1, 1, 0}, {0, 0, 0},
    };
    vx_uint32 width = 0;
    vx_uint32 height = 0;
    vx_image src = (vx_image)paramObj[0];

    vxQueryImage(src, VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
    vxQueryImage(src, VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));

    shaderParam.workDim = 2;
    shaderParam.globalWorkScale[0] = 2;
    shaderParam.globalWorkScale[1] = 2;

    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] = gcmALIGN_NP2(width, shaderParam.globalWorkScale[0]) / shaderParam.globalWorkScale[0];
    shaderParam.globalWorkSize[1] = gcmALIGN_NP2(height, shaderParam.globalWorkScale[1]) / shaderParam.globalWorkScale[1];

    status |= vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t));

    return VX_SUCCESS;
  }

  vx_node UserNodes::nxpEqualizeHistStep1Node(vx_graph graph, vx_distribution distR, vx_distribution distG, vx_distribution distB,
                                              vx_distribution outDistR, vx_distribution outDistG, vx_distribution outDistB, vx_distribution alpha,
                                              vx_distribution beta)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;

    context = vxGetContext((vx_reference)graph);
    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_EQUALIZEHIST0);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, 0, (vx_reference)distR);
    status |= vxSetParameterByIndex(node, 1, (vx_reference)distG);
    status |= vxSetParameterByIndex(node, 2, (vx_reference)distB);
    status |= vxSetParameterByIndex(node, 3, (vx_reference)outDistR);
    status |= vxSetParameterByIndex(node, 4, (vx_reference)outDistG);
    status |= vxSetParameterByIndex(node, 5, (vx_reference)outDistB);
    status |= vxSetParameterByIndex(node, 6, (vx_reference)alpha);
    status |= vxSetParameterByIndex(node, 7, (vx_reference)beta);

    if (status != VX_SUCCESS)
    {
      status = VX_ERROR_INVALID_PARAMETERS;
      vxReleaseNode(&node);
    }
    vxReleaseKernel(&kernel);

    return node;
  }


  vx_status UserNodes::vxEqualizeHistStep1InputValidator(vx_node nodeObj, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (index == 0)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 0);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));

      if (dist)
      {
        vx_size numBins = 0;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 1)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 1);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));

      if (dist)
      {
        vx_size numBins = 0;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 2)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 2);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if (dist)
      {
        vx_size numBins = 0;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }

    return status;
  }


  vx_status UserNodes::vxEqualizeHistStep1Validator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_status status = VX_SUCCESS;
    vx_uint32 index = 0;
    for (index = 0; index < num; index++)
    {
      if (index < 3)
      {
        status |= vxEqualizeHistStep1InputValidator(node, index);
      }
      else
      {
        status |= vxEqualizeHistStep1OutputValidator(node, index, metas[index]);
      }
    }
    return status;
  }


  vx_status UserNodes::vxEqualizeHistStep1Initializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    vx_status status = VX_SUCCESS;
    vx_kernel_execution_parameters_t shaderParam = {
      2, {0, 0, 0}, {1, 1, 0}, {1, 1, 0}, {0, 0, 0},
    };

    shaderParam.workDim = 2;
    shaderParam.globalWorkScale[0] = 1;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] = gcmALIGN_NP2(32, shaderParam.globalWorkScale[0]) / shaderParam.globalWorkScale[0];
    shaderParam.globalWorkSize[1] = gcmALIGN_NP2(1, shaderParam.globalWorkScale[1]) / shaderParam.globalWorkScale[1];

    status |= vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t));

    return VX_SUCCESS;
  }

  vx_status UserNodes::vxEqualizeHistStep1OutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (index == 3)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 3);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if ((dist))
      {
        vx_size numBins = 0, rangs = 0;
        vx_uint32 windows;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxQueryDistribution(dist, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxQueryDistribution(dist, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));

        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 4)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 4);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if ((dist))
      {
        vx_size numBins = 0, rangs = 0;
        vx_uint32 windows;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxQueryDistribution(dist, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxQueryDistribution(dist, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));

        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 5)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 5);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if ((dist))
      {
        vx_size numBins = 0, rangs = 0;
        vx_uint32 windows;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxQueryDistribution(dist, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxQueryDistribution(dist, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));

        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));

        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 6)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 6);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if ((dist))
      {
        vx_size numBins = 0, rangs = 0;
        vx_uint32 windows;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxQueryDistribution(dist, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxQueryDistribution(dist, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));

        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 7)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 7);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if ((dist))
      {
        vx_size numBins = 0, rangs = 0;
        vx_uint32 windows;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxQueryDistribution(dist, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxQueryDistribution(dist, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));

        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_WINDOW, &windows, sizeof(windows));
        vxSetMetaFormatAttribute(metaObj, VX_DISTRIBUTION_RANGE, &rangs, sizeof(rangs));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    return status;
  }


  vx_node UserNodes::nxpWhiteBalanceNode(vx_graph graph, vx_image in, vx_distribution alpha, vx_distribution beta, vx_image out)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;

    context = vxGetContext((vx_reference)graph);
    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_WHITEBALANCE);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, 0, (vx_reference)in);
    status |= vxSetParameterByIndex(node, 1, (vx_reference)alpha);
    status |= vxSetParameterByIndex(node, 2, (vx_reference)beta);
    status |= vxSetParameterByIndex(node, 3, (vx_reference)out);

    if (status != VX_SUCCESS)
    {
      status = VX_ERROR_INVALID_PARAMETERS;
      vxReleaseNode(&node);
    }
    vxReleaseKernel(&kernel);

    return node;
  }


  vx_status UserNodes::vxWhiteBalanceInputValidator(vx_node nodeObj, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_df_image imgFmt = 0;

    if (index == 0)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));

          if (VX_DF_IMAGE_U8 == imgFmt)
            status = VX_SUCCESS;
          else
            status = VX_ERROR_INVALID_VALUE;

          vxReleaseImage(&imgObj);
        }

        vxReleaseParameter(&paramObj);
      }
    }
    else if (index == 1)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 1);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if (dist)
      {
        vx_size numBins = 0;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 2)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 2);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));

      if (dist)
      {
        vx_size numBins = 0;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }

    return status;
  }


  vx_status UserNodes::vxWhiteBalanceOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_uint32 width = 0;
    vx_uint32 height = 0;
    vx_df_image imgFmt = VX_DF_IMAGE_U8;

    if (index == 3)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          status = vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxReleaseImage(&imgObj);
        }
        status |= vxReleaseParameter(&paramObj);
      }
    }
    return status;
  }


  vx_status UserNodes::vxWhiteBalanceValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_status status = VX_SUCCESS;
    vx_uint32 index = 0;
    for (index = 0; index < num; index++)
    {
      if (index < 3)
      {
        status |= vxWhiteBalanceInputValidator(node, index);
      }
      else
      {
        status |= vxWhiteBalanceOutputValidator(node, index, metas[index]);
      }
    }
    return status;
  }


  vx_status UserNodes::vxWhiteBalanceInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_int32 imgWid = 0, imgHei = 0;
    vx_image imgObj = (vx_image)paramObj[0];
    vx_distribution in_alpha = (vx_distribution)paramObj[1];

    vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32));
    vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 2;
    shaderParam.globalWorkScale[1] = 2;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] = gcmALIGN_NP2(imgWid, shaderParam.globalWorkScale[0]) / shaderParam.globalWorkScale[0];
    shaderParam.globalWorkSize[1] = gcmALIGN_NP2(imgHei, shaderParam.globalWorkScale[1]) / shaderParam.globalWorkScale[1];

    vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t));

    vx_int32 offset = 0;
    vx_uint32 range = 0;
    vx_size numBins = 0;
    vx_uint32 window_size = 0;
    vxQueryDistribution(in_alpha, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
    vxQueryDistribution(in_alpha, VX_DISTRIBUTION_RANGE, &range, sizeof(range));
    vxQueryDistribution(in_alpha, VX_DISTRIBUTION_OFFSET, &offset, sizeof(offset));
    vxQueryDistribution(in_alpha, VX_DISTRIBUTION_WINDOW, &window_size, sizeof(window_size));

    return VX_SUCCESS;
  }


  vx_node UserNodes::nxpEqualizeHistNode(vx_graph graph, vx_image in_image, vx_distribution distributionR, vx_distribution distributionG,
                                         vx_distribution distributionB, vx_image out_image)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;

    context = vxGetContext((vx_reference)graph);
    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_EQUALIZEHIST);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, 0, (vx_reference)in_image);
    status |= vxSetParameterByIndex(node, 1, (vx_reference)distributionR);
    status |= vxSetParameterByIndex(node, 2, (vx_reference)distributionG);
    status |= vxSetParameterByIndex(node, 3, (vx_reference)distributionB);
    status |= vxSetParameterByIndex(node, 4, (vx_reference)out_image);

    if (status != VX_SUCCESS)
    {
      status = VX_ERROR_INVALID_PARAMETERS;
      vxReleaseNode(&node);
    }
    vxReleaseKernel(&kernel);

    return node;
  }


  vx_status UserNodes::vxEqualizeHistInputValidator(vx_node nodeObj, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_df_image imgFmt = 0;

    if (index == 0)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));

          if (VX_DF_IMAGE_U8 == imgFmt)
            status = VX_SUCCESS;
          else
            status = VX_ERROR_INVALID_VALUE;

          vxReleaseImage(&imgObj);
        }

        vxReleaseParameter(&paramObj);
      }
    }
    else if (index == 1)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 1);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if (dist)
      {
        vx_size numBins = 0;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 2)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 2);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));

      if (dist)
      {
        vx_size numBins = 0;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }
    else if (index == 3)
    {
      vx_parameter dst_param = vxGetParameterByIndex(nodeObj, 3);
      vx_distribution dist;
      vxQueryParameter(dst_param, VX_PARAMETER_REF, &dist, sizeof(dist));
      if (dist)
      {
        vx_size numBins = 0;
        vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
        status = VX_SUCCESS;
        vxReleaseDistribution(&dist);
      }
      vxReleaseParameter(&dst_param);
    }

    return status;
  }


  vx_status UserNodes::vxEqualizeHistOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_uint32 width = 0;
    vx_uint32 height = 0;
    vx_df_image imgFmt = VX_DF_IMAGE_U8;

    if (index == 4)
    {
      paramObj = vxGetParameterByIndex(nodeObj, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          status = vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxReleaseImage(&imgObj);
        }
        status |= vxReleaseParameter(&paramObj);
      }
    }
    return status;
  }


  vx_status UserNodes::vxEqualizeHistValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_status status = VX_SUCCESS;
    vx_uint32 index = 0;
    for (index = 0; index < num; index++)
    {
      if (index < 4)
      {
        status |= vxEqualizeHistInputValidator(node, index);
      }
      else
      {
        status |= vxEqualizeHistOutputValidator(node, index, metas[index]);
      }
    }
    return status;
  }


  vx_status UserNodes::vxEqualizeHistInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    vx_status status = VX_SUCCESS;
    vx_kernel_execution_parameters_t shaderParam = {
      2, {0, 0, 0}, {1, 1, 0}, {1, 1, 0}, {0, 0, 0},
    };
    vx_uint32 width = 0;
    vx_uint32 height = 0;
    vx_image src = (vx_image)paramObj[0];
    vx_distribution distributionR = (vx_distribution)paramObj[1];

    vxQueryImage(src, VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
    vxQueryImage(src, VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
    shaderParam.workDim = 2;
    shaderParam.globalWorkScale[0] = 2;
    shaderParam.globalWorkScale[1] = 2;

    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;

    shaderParam.globalWorkSize[0] = gcmALIGN_NP2(width, shaderParam.globalWorkScale[0]) / shaderParam.globalWorkScale[0];
    shaderParam.globalWorkSize[1] = gcmALIGN_NP2(height, shaderParam.globalWorkScale[1]) / shaderParam.globalWorkScale[1];

    status |= vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t));

    vx_int32 offset = 0;
    vx_uint32 range = 0;
    vx_size numBins = 0;
    vx_uint32 window_size = 0;
    vxQueryDistribution(distributionR, VX_DISTRIBUTION_BINS, &numBins, sizeof(numBins));
    vxQueryDistribution(distributionR, VX_DISTRIBUTION_RANGE, &range, sizeof(range));
    vxQueryDistribution(distributionR, VX_DISTRIBUTION_OFFSET, &offset, sizeof(offset));
    vxQueryDistribution(distributionR, VX_DISTRIBUTION_WINDOW, &window_size, sizeof(window_size));

    return VX_SUCCESS;
  }


  vx_node UserNodes::nxpDebayerNode(vx_graph graph, vx_image in, vx_image out)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_int32 index = 0;
    context = vxGetContext((vx_reference)graph);
    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_DEBAYER);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, index++, (vx_reference)in);
    status |= vxSetParameterByIndex(node, index++, (vx_reference)out);

    if (status != VX_SUCCESS)
    {
      status = VX_ERROR_INVALID_PARAMETERS;
      vxReleaseNode(&node);
    }
    vxReleaseKernel(&kernel);

    return node;
  }


  vx_status UserNodes::vxDebayerInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_int32 imgWid = 0, imgHei = 0;
    vx_image imgObj = (vx_image)paramObj[0];

    vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32));
    vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 4;
    shaderParam.globalWorkScale[1] = 4;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      (((imgWid + 3) / 4 + shaderParam.localWorkSize[0] - 1) / shaderParam.localWorkSize[0]) * shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1) / shaderParam.globalWorkScale[1];

    vx_uint32 uniform1[16] = {0};
    vx_uint32 uniform2[16] = {0};
    vx_uint32 uniform4[16] = {0};
    vx_uint32 uniform5[16] = {0};

    /* DP4x8, acc[j] = sum((src * const[i] )) i = 0 ... 7*/
    uniform1[0] = 0x9a291606;  /* config */
    uniform1[1] = 0x9a291606;  /* config */
    uniform1[2] = 0xe2200242;  /*  bin */
    uniform1[3] = 0x30524304;  /*  bin */
    uniform1[4] = 0x0284a4e4;  /*  bin */
    uniform1[5] = 0x85056640;  /*  bin */
    uniform1[6] = 0xb568505a;  /*  bin */
    uniform1[7] = 0x00000700;  /* accumulate format */
    uniform1[8] = 0x00000402;  /* const */
    uniform1[9] = 0x00040403;  /* const */
    uniform1[10] = 0x00020201; /* const */
    uniform1[11] = 0x02080202; /* const */
    uniform1[12] = 0x00000402; /* const */
    uniform1[13] = 0x00040403; /* const */
    uniform1[14] = 0x00020201; /* const */
    uniform1[15] = 0x02080202; /* const */
    /* DP4x8, acc[j] = sum((src * const[i] )) i = 0 ... 7*/
    uniform2[0] = 0x37962696;  /* config */
    uniform2[1] = 0x37962696;  /* config */
    uniform2[2] = 0x04020c20;  /*  bin */
    uniform2[3] = 0x12904101;  /*  bin */
    uniform2[4] = 0x14620146;  /*  bin */
    uniform2[5] = 0x83018823;  /*  bin */
    uniform2[6] = 0x01ca3398;  /*  bin */
    uniform2[7] = 0x00007700;  /* accumulate format */
    uniform2[8] = 0x02040402;  /* const */
    uniform2[9] = 0x00030c03;  /* const */
    uniform2[10] = 0x02080802; /* const */
    uniform2[11] = 0x00000a00; /* const */
    uniform2[12] = 0x02040402; /* const */
    uniform2[13] = 0x00030c03; /* const */
    uniform2[14] = 0x02080802; /* const */
    uniform2[15] = 0x00000a00; /* const */


    /* DP4x8, acc[j] = sum((src * const[i] )) i = 0 ... 7*/
    uniform4[0] = 0x06162b9a;  /* config */
    uniform4[1] = 0x06162b9a;  /* config */
    uniform4[2] = 0xe229ca22;  /*  bin */
    uniform4[3] = 0x30524304;  /*  bin */
    uniform4[4] = 0xd2640026;  /*  bin */
    uniform4[5] = 0x8505664a;  /*  bin */
    uniform4[6] = 0x002a505a;  /*  bin */
    uniform4[7] = 0x00007700;  /* accumulate format */
    uniform4[8] = 0x02080202;  /* const */
    uniform4[9] = 0x00020200;  /* const */
    uniform4[10] = 0x00040403; /* const */
    uniform4[11] = 0x00000402; /* const */
    uniform4[12] = 0x02080202; /* const */
    uniform4[13] = 0x00020200; /* const */
    uniform4[14] = 0x00040403; /* const */
    uniform4[15] = 0x00000402; /* const */
    /* DP4x8, acc[j] = sum((src * const[i] )) i = 0 ... 7*/
    uniform5[0] = 0x96269637;  /* config */
    uniform5[1] = 0x96269637;  /* config */
    uniform5[2] = 0xc2001040;  /*  bin */
    uniform5[3] = 0x13146120;  /*  bin */
    uniform5[4] = 0x18822904;  /*  bin */
    uniform5[5] = 0xa3314620;  /*  bin */
    uniform5[6] = 0x3988301c;  /*  bin */
    uniform5[7] = 0x00007700;  /* accumulate format */
    uniform5[8] = 0x00000a00;  /* const */
    uniform5[9] = 0x02080802;  /* const */
    uniform5[10] = 0x00030c03; /* const */
    uniform5[11] = 0x02040402; /* const */
    uniform5[12] = 0x00000a00; /* const */
    uniform5[13] = 0x02080802; /* const */
    uniform5[14] = 0x00030c03; /* const */
    uniform5[15] = 0x02040402; /* const */

    vx_uint32 uniform3[16] = {
      0x73713173, 0x00007131,                                                                           // TCfg
      0xc1290001, 0x59881300, 0x1c14a100, 0x00a98150, 0x00000000,                                       // BinSelect
      0x00003404,                                                                                       // AccumType, ConstantType, and PostShift
      0x08000000, 0x00000010, 0x0a000010, 0x08000000, 0x00000010, 0x0a000010, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uniform6[16] = {
      0x17371317, 0x00003713,                                                                           // TCfg
      0xc0004a41, 0x500a6304, 0x54040528, 0x0001aa70, 0x00000000,                                       // BinSelect
      0x00003404,                                                                                       // AccumType, ConstantType, and PostShift
      0x00100a00, 0x00100000, 0x00000800, 0x00100a00, 0x00100000, 0x00000800, 0x00000000, 0x00000000    // Constant
    };
    vx_uint32 uniform7[16] = {
      0x13331333, 0x13331333,                                                                           // TCfg
      0x00200400, 0x50100304, 0x1c060440, 0x09048080, 0x04c0b028,                                       // BinSelect
      0x00007700,                                                                                       // AccumType, ConstantType, and PostShift
      0x00000000, 0x00ff0000, 0x00000000, 0x00ff0000, 0x00000000, 0x00ff0000, 0x00000000, 0x00ff0000    // Constant
    };
    vxSetNodeUniform(nodObj, "EE_EO_AccTwoLines", 1, uniform1);
    vxSetNodeUniform(nodObj, "EE_EO_AccOneLine", 1, uniform2);
    vxSetNodeUniform(nodObj, "EE_EO_PackBGR", 1, uniform3);

    vxSetNodeUniform(nodObj, "EO_OO_AccTwoLines", 1, uniform4);
    vxSetNodeUniform(nodObj, "EO_OO_AccOneLine", 1, uniform5);
    vxSetNodeUniform(nodObj, "EO_OO_PackBGR", 1, uniform6);
    vxSetNodeUniform(nodObj, "BGR_2_BGRA", 1, uniform7);

    vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t));

    return VX_SUCCESS;
  }


  vx_node UserNodes::nxpRGBA2YUVNode(vx_graph graph, vx_image in_image, vx_image out_image, vx_image out_image_y)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_int32 index = 0;

    context = vxGetContext((vx_reference)graph);
    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_RGBA2YUV);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, index++, (vx_reference)in_image);
    status |= vxSetParameterByIndex(node, index++, (vx_reference)out_image);
    status |= vxSetParameterByIndex(node, index++, (vx_reference)out_image_y);

    vxReleaseKernel(&kernel);

    return node;
  }


  vx_status UserNodes::vxRGBA2YUVInputValidator(vx_node node, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_df_image imgFmt = 0;

    if (index == 0)
    {
      paramObj = vxGetParameterByIndex(node, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));

          if (VX_DF_IMAGE_U8 == imgFmt)
            status = VX_SUCCESS;
          else
            status = VX_ERROR_INVALID_VALUE;

          vxReleaseImage(&imgObj);
        }

        vxReleaseParameter(&paramObj);
      }
    }

    return status;
  }

  vx_status UserNodes::vxRGBA2YUVOutputValidator(vx_node node, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_uint32 width = 0;
    vx_uint32 height = 0;
    vx_df_image imgFmt = VX_DF_IMAGE_U8;

    if (index == 1)
    {
      paramObj = vxGetParameterByIndex(node, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          status = vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxReleaseImage(&imgObj);
        }
        status |= vxReleaseParameter(&paramObj);
      }
    }
    else if (index == 2)
    {
      paramObj = vxGetParameterByIndex(node, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          status = vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxReleaseImage(&imgObj);
        }
        status |= vxReleaseParameter(&paramObj);
      }
    }
    return status;
  }


  vx_status UserNodes::vxRGBA2YUVValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_status status = VX_SUCCESS;
    vx_uint32 index = 0;
    for (index = 0; index < num; index++)
    {
      if (index < 1)
      {
        status |= vxRGBA2YUVInputValidator(node, index);
      }
      else
      {
        status |= vxRGBA2YUVOutputValidator(node, index, metas[index]);
      }
    }
    return status;
  }


  vx_status UserNodes::vxRGBA2YUVInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_int32 imgWid = 0, imgHei = 0;
    vx_image imgObj = (vx_image)paramObj[2];

    vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32));
    vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 4;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      (((imgWid + shaderParam.globalWorkScale[0] - 1) / shaderParam.globalWorkScale[0] + shaderParam.localWorkSize[0] - 1) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1) / shaderParam.globalWorkScale[1];

    vx_uint32 rgba2yuv[16] = {
      0x01695a55,                                                                                       // TCfg
      0x00000000,                                                                                       // ASelt
      0x30123012, 0x00033012,                                                                           // ABin
      0x02aaaaaa,                                                                                       // BSelt
      0x00000000, 0x00000000,                                                                           // BBin
      0x00002100,                                                                                       // AccumType, ConstantType, and PostShift
      0x3808341c, 0x2c082e45, 0x34a730bc, 0x38003706, 0x35e33706, 0x38002c8b, 0x00003c00, 0x00000000    // Constant
    };

    vxSetNodeUniform(nodObj, "RGBA2Y_UV", 1, rgba2yuv);

    vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t));

    return VX_SUCCESS;
  }


  vx_node UserNodes::nxpBilateralNode(vx_graph graph, vx_image in_image, vx_image out_image)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_int32 index = 0;

    context = vxGetContext((vx_reference)graph);
    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_BILATERAL);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, index++, (vx_reference)in_image);
    status |= vxSetParameterByIndex(node, index++, (vx_reference)out_image);

    vxReleaseKernel(&kernel);

    return node;
  }


  vx_status UserNodes::vxBilateralInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_int32 imgWid = 0, imgHei = 0;
    vx_image imgObj = (vx_image)paramObj[0];

    vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32));
    vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 8;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      (((imgWid + shaderParam.globalWorkScale[0] - 1) / shaderParam.globalWorkScale[0] + shaderParam.localWorkSize[0] - 1) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1) / shaderParam.globalWorkScale[1];

    vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t));

    return VX_SUCCESS;
  }


  vx_node UserNodes::nxpYUV2RGBANode(vx_graph graph, vx_image in_image, vx_image in_image_y, vx_image out_image)
  {
    vx_context context = nullptr;
    vx_kernel kernel = nullptr;
    vx_node node = nullptr;
    vx_int32 index = 0;

    context = vxGetContext((vx_reference)graph);
    kernel = vxGetKernelByName(context, VX_KERNEL_NAME_YUV2RGBA);
    node = vxCreateGenericNode(graph, kernel);

    vx_status status = vxSetParameterByIndex(node, index++, (vx_reference)in_image);
    status |= vxSetParameterByIndex(node, index++, (vx_reference)in_image_y);
    status |= vxSetParameterByIndex(node, index++, (vx_reference)out_image);

    return node;
  }

  vx_status UserNodes::vxYUV2RGBAInputValidator(vx_node node, vx_uint32 index)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_df_image imgFmt = 0;

    if (index == 0)
    {
      paramObj = vxGetParameterByIndex(node, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));

          if (VX_DF_IMAGE_U8 == imgFmt)
            status = VX_SUCCESS;
          else
            status = VX_ERROR_INVALID_VALUE;

          vxReleaseImage(&imgObj);
        }

        vxReleaseParameter(&paramObj);
      }
    }
    else if (index == 1)
    {
      paramObj = vxGetParameterByIndex(node, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          vxQueryImage(imgObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));

          if (VX_DF_IMAGE_U8 == imgFmt)
            status = VX_SUCCESS;
          else
            status = VX_ERROR_INVALID_VALUE;

          vxReleaseImage(&imgObj);
        }

        vxReleaseParameter(&paramObj);
      }
    }

    return status;
  }


  vx_status UserNodes::vxYUV2RGBAOutputValidator(vx_node node, vx_uint32 index, vx_meta_format metaObj)
  {
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_parameter paramObj = nullptr;
    vx_image imgObj = nullptr;
    vx_uint32 width = 0;
    vx_uint32 height = 0;
    vx_df_image imgFmt = VX_DF_IMAGE_U8;

    if (index == 2)
    {
      paramObj = vxGetParameterByIndex(node, index);
      if (paramObj != nullptr)
      {
        if ((VX_SUCCESS == vxQueryParameter(paramObj, VX_PARAMETER_REF, &imgObj, sizeof(vx_image))))
        {
          status = vxQueryImage(imgObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_FORMAT, &imgFmt, sizeof(imgFmt));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_WIDTH, &width, sizeof(width));
          status |= vxSetMetaFormatAttribute(metaObj, VX_IMAGE_HEIGHT, &height, sizeof(height));

          status |= vxReleaseImage(&imgObj);
        }
        status |= vxReleaseParameter(&paramObj);
      }
    }
    return status;
  }


  vx_status UserNodes::vxYUV2RGBAValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
  {
    vx_status status = VX_SUCCESS;
    vx_uint32 index = 0;
    for (index = 0; index < num; index++)
    {
      if (index < 2)
      {
        status |= vxYUV2RGBAInputValidator(node, index);
      }
      else
      {
        status |= vxYUV2RGBAOutputValidator(node, index, metas[index]);
      }
    }
    return status;
  }


  vx_status UserNodes::vxYUV2RGBAInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum)
  {
    vx_kernel_execution_parameters_t shaderParam = {2, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    vx_int32 imgWid = 0, imgHei = 0;
    vx_image imgObj = (vx_image)paramObj[1];

    vxQueryImage(imgObj, VX_IMAGE_WIDTH, &imgWid, sizeof(vx_uint32));
    vxQueryImage(imgObj, VX_IMAGE_HEIGHT, &imgHei, sizeof(vx_uint32));

    shaderParam.globalWorkOffset[0] = 0;
    shaderParam.globalWorkOffset[1] = 0;
    shaderParam.globalWorkScale[0] = 4;
    shaderParam.globalWorkScale[1] = 1;
    shaderParam.localWorkSize[0] = 32;
    shaderParam.localWorkSize[1] = 1;
    shaderParam.globalWorkSize[0] =
      (((imgWid + shaderParam.globalWorkScale[0] - 1) / shaderParam.globalWorkScale[0] + shaderParam.localWorkSize[0] - 1) /
       shaderParam.localWorkSize[0]) *
      shaderParam.localWorkSize[0];
    shaderParam.globalWorkSize[1] = (imgHei + shaderParam.globalWorkScale[1] - 1) / shaderParam.globalWorkScale[1];


    vx_uint32 yuv2rgb[16] = {
      0x01256925,                                                                                       // TCfg
      0x00010101,                                                                                       // ASelt
      0x21000200, 0x00020210,                                                                           // ABin
      0x022aaa2a,                                                                                       // BSelt
      0x00000000, 0x00000000,                                                                           // BBin
      0x00000100,                                                                                       // AccumType, ConstantType, and PostShift
      0x40083ca7, 0x00003c57, 0x36453ca7, 0x38413a81, 0x3e623ca7, 0x00003afd, 0x00003c00, 0x00000000    // Constant
    };

    vxSetNodeUniform(nodObj, "Y_UV2RGBA", 1, yuv2rgb);

    vxSetNodeAttribute(nodObj, VX_NODE_ATTRIBUTE_KERNEL_EXECUTION_PARAMETERS, &shaderParam, sizeof(vx_kernel_execution_parameters_t));

    return VX_SUCCESS;
  }


  vx_status UserNodes::vxPublishNodeKernels(vx_context ContextVX)
  {
    vx_param_description_t vxKernelParam[] = {{VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                              {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED}};
    vx_param_description_t vxhistogramKernelParam[] = {{VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                                       {VX_OUTPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
                                                       {VX_OUTPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
                                                       {VX_OUTPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED}};
    vx_param_description_t vxWhiteBalanceKernelParam[] = {{VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
                                                          {VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
                                                          {VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
                                                          {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED}};
    vx_param_description_t vxEqualizeHistStep1KernelParam[] = {
      {VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},  {VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
      {VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},  {VX_OUTPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
      {VX_OUTPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED}, {VX_OUTPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
      {VX_OUTPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED}, {VX_OUTPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
    };
    vx_param_description_t vxEqualizeHistKernelParam[] = {
      {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},        {VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
      {VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED}, {VX_INPUT, VX_TYPE_DISTRIBUTION, VX_PARAMETER_STATE_REQUIRED},
      {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
    };
    vx_param_description_t vxRGBA2YUVKernelParam[] = {
      {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
      {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
      {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
    };
    vx_param_description_t vxYUV2RGBAKernelParam[] = {
      {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
      {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
      {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
    };

    vx_kernel_description_t vxBadPixelKernelVXCInfo = {VX_KERNEL_ENUM_BADPIXEL,
                                                       VX_KERNEL_NAME_BADPIXEL,
                                                       nullptr,
                                                       vxKernelParam,
                                                       (sizeof(vxKernelParam) / sizeof(vxKernelParam[0])),
                                                       vxValidator,
                                                       nullptr,
                                                       nullptr,
                                                       vxBadPixelDetectInitializer,
                                                       nullptr};

    vx_kernel_description_t vxhistogramKernelVXCInfo = {VX_KERNEL_ENUM_HISTOGRAM,
                                                        VX_KERNEL_NAME_HISTOGRAM,
                                                        nullptr,
                                                        vxhistogramKernelParam,
                                                        (sizeof(vxhistogramKernelParam) / sizeof(vxhistogramKernelParam[0])),
                                                        vxhistogramValidator,
                                                        nullptr,
                                                        nullptr,
                                                        vxhistogramInitializer,
                                                        nullptr};
    vx_kernel_description_t vxWhiteBalanceKernelVXCInfo = {VX_KERNEL_ENUM_WHITEBALANCE,
                                                           VX_KERNEL_NAME_WHITEBALANCE,
                                                           nullptr,
                                                           vxWhiteBalanceKernelParam,
                                                           (sizeof(vxWhiteBalanceKernelParam) / sizeof(vxWhiteBalanceKernelParam[0])),
                                                           vxWhiteBalanceValidator,
                                                           nullptr,
                                                           nullptr,
                                                           vxWhiteBalanceInitializer,
                                                           nullptr};
    vx_kernel_description_t vxDebayerKernelVXCInfo = {VX_KERNEL_ENUM_DEBAYER,
                                                      VX_KERNEL_NAME_DEBAYER,
                                                      nullptr,
                                                      vxKernelParam,
                                                      (sizeof(vxKernelParam) / sizeof(vxKernelParam[0])),
                                                      vxValidator,
                                                      nullptr,
                                                      nullptr,
                                                      vxDebayerInitializer,
                                                      nullptr};
    vx_kernel_description_t vxEqualizeHistStep1KernelVXCInfo = {VX_KERNEL_ENUM_EQUALIZEHIST0,
                                                                VX_KERNEL_NAME_EQUALIZEHIST0,
                                                                nullptr,
                                                                vxEqualizeHistStep1KernelParam,
                                                                (sizeof(vxEqualizeHistStep1KernelParam) / sizeof(vxEqualizeHistStep1KernelParam[0])),
                                                                vxEqualizeHistStep1Validator,
                                                                nullptr,
                                                                nullptr,
                                                                vxEqualizeHistStep1Initializer,
                                                                nullptr};
    vx_kernel_description_t vxEqualizeHistKernelVXCInfo = {VX_KERNEL_ENUM_EQUALIZEHIST,
                                                           VX_KERNEL_NAME_EQUALIZEHIST,
                                                           nullptr,
                                                           vxEqualizeHistKernelParam,
                                                           (sizeof(vxEqualizeHistKernelParam) / sizeof(vxEqualizeHistKernelParam[0])),
                                                           vxEqualizeHistValidator,
                                                           nullptr,
                                                           nullptr,
                                                           vxEqualizeHistInitializer,
                                                           nullptr};
    vx_kernel_description_t vxRGBA2YUVKernelVXCInfo = {VX_KERNEL_ENUM_RGBA2YUV,
                                                       VX_KERNEL_NAME_RGBA2YUV,
                                                       nullptr,
                                                       vxRGBA2YUVKernelParam,
                                                       (sizeof(vxRGBA2YUVKernelParam) / sizeof(vxRGBA2YUVKernelParam[0])),
                                                       vxRGBA2YUVValidator,
                                                       nullptr,
                                                       nullptr,
                                                       vxRGBA2YUVInitializer,
                                                       nullptr};
    vx_kernel_description_t vxBilateralKernelVXCInfo = {VX_KERNEL_ENUM_BILATERAL,
                                                        VX_KERNEL_NAME_BILATERAL,
                                                        nullptr,
                                                        vxKernelParam,
                                                        (sizeof(vxKernelParam) / sizeof(vxKernelParam[0])),
                                                        vxValidator,
                                                        nullptr,
                                                        nullptr,
                                                        vxBilateralInitializer,
                                                        nullptr};
    vx_kernel_description_t vxYUV2RGBAKernelVXCInfo = {VX_KERNEL_ENUM_YUV2RGBA,
                                                       VX_KERNEL_NAME_YUV2RGBA,
                                                       nullptr,
                                                       vxYUV2RGBAKernelParam,
                                                       (sizeof(vxYUV2RGBAKernelParam) / sizeof(vxYUV2RGBAKernelParam[0])),
                                                       vxYUV2RGBAValidator,
                                                       nullptr,
                                                       nullptr,
                                                       vxYUV2RGBAInitializer,
                                                       nullptr};

    vx_kernel_description_t* kernels[] = {
      &vxBadPixelKernelVXCInfo,     &vxhistogramKernelVXCInfo,    &vxEqualizeHistStep1KernelVXCInfo,
      &vxWhiteBalanceKernelVXCInfo, &vxEqualizeHistKernelVXCInfo, &vxDebayerKernelVXCInfo,
      &vxRGBA2YUVKernelVXCInfo,     &vxBilateralKernelVXCInfo,    &vxYUV2RGBAKernelVXCInfo,
    };

    vx_status status = VX_FAILURE;
    vx_kernel kernelObj = nullptr;

    vx_program programObj = nullptr;
    std::string programStr = m_contentManager->ReadAllText("kernels_vxc.vx");
    const vx_char* programSrc[] = {programStr.c_str()};
    vx_size programLen = programStr.size();

    programObj = vxCreateProgramWithSource(ContextVX, 1, programSrc, &programLen);
    RAPIDOPENVX_CHECK(vxGetStatus((vx_reference)programObj));

    IO::Path contentPath = m_contentManager->GetContentPath();
    std::string buildOptionStr = "-cl-viv-vx-extension -I ";
    buildOptionStr += contentPath.ToUTF8String();
    const vx_char* buildOptions = buildOptionStr.c_str();
    RAPIDOPENVX_CHECK(vxBuildProgram(programObj, buildOptions));

    std::size_t kernelNum = 0;
    if (m_denoiseEn)
    {
      kernelNum = sizeof(kernels) / sizeof(kernels[0]);
      FSLLOG("Please wait:");
      FSLLOG("Building noise reduction kernel will take several minutes!");
    }
    else
    {
      kernelNum = sizeof(kernels) / sizeof(kernels[0]) - 3;
    }
    FSLLOG("Please wait for compiling and building kernels...");
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
        FSLLOG("Succeeded to finalize kernel[" << i << "]= " << kernels[i]->name);
      }
      else
      {
        RAPIDOPENVX_CHECK(vxRemoveKernel(kernelObj));
      }
    }
    return status;
  }
}
