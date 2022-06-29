#ifndef OPENVX_SOFTISP_USERNODES_HPP
#define OPENVX_SOFTISP_USERNODES_HPP
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

#include <FslDemoApp/OpenVX/DemoAppOpenVX.hpp>
#include <RapidOpenVX/Context.hpp>
#include <RapidOpenVX/Distribution.hpp>
#include <RapidOpenVX/Graph.hpp>
#include <RapidOpenVX/Image.hpp>
#include <VX/vx.h>
#include <VX/vx_ext_program.h>
#include <VX/vx_helper.h>
#include <VX/vxu.h>
#include <iostream>

namespace Fsl
{
  class UserNodes
  {
    std::shared_ptr<IContentManager> m_contentManager;

  public:
    bool m_denoiseEn;
    UserNodes(const std::shared_ptr<IContentManager>& contentManager);
    ~UserNodes();
    vx_status vxPublishNodeKernels(vx_context context);
    vx_node nxpDebayerNode(vx_graph graph, vx_image in, vx_image out);
    vx_node nxpBadPixelDetectNode(vx_graph graph, vx_image in, vx_image out);
    vx_node nxpHistogramNode(vx_graph graph, vx_image in, vx_distribution distR, vx_distribution distG, vx_distribution distB);
    vx_node nxpEqualizeHistStep1Node(vx_graph graph, vx_distribution distR, vx_distribution distG, vx_distribution distB, vx_distribution outDistR,
                                     vx_distribution outDistG, vx_distribution outDistB, vx_distribution alpha, vx_distribution beta);
    vx_node nxpWhiteBalanceNode(vx_graph graph, vx_image in, vx_distribution alpha, vx_distribution beta, vx_image out);
    vx_node nxpEqualizeHistNode(vx_graph graph, vx_image in, vx_distribution distR, vx_distribution distG, vx_distribution distB, vx_image out);
    vx_node nxpRGBA2YUVNode(vx_graph graph, vx_image in, vx_image outUV, vx_image outY);
    vx_node nxpBilateralNode(vx_graph graph, vx_image in, vx_image out);
    vx_node nxpYUV2RGBANode(vx_graph graph, vx_image in, vx_image inY, vx_image out);

  protected:
    static vx_status vxInputValidator(vx_node nodeObj, vx_uint32 index);
    static vx_status vxOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj);
    static vx_status vxValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status vxBadPixelDetectInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status vxhistogramInputValidator(vx_node nodeObj, vx_uint32 index);
    static vx_status vxhistogramOutputValidator(vx_node node, vx_uint32 index, vx_meta_format metaObj);
    static vx_status vxhistogramValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status vxhistogramInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status vxWhiteBalanceInputValidator(vx_node nodeObj, vx_uint32 index);
    static vx_status vxWhiteBalanceOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj);
    static vx_status vxWhiteBalanceValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status vxWhiteBalanceInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status vxEqualizeHistStep1InputValidator(vx_node nodeObj, vx_uint32 index);
    static vx_status vxEqualizeHistStep1OutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj);
    static vx_status vxEqualizeHistStep1Validator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status vxEqualizeHistStep1Initializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status vxEqualizeHistInputValidator(vx_node nodeObj, vx_uint32 index);
    static vx_status vxEqualizeHistOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj);
    static vx_status vxEqualizeHistValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status vxEqualizeHistInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status vxDebayerInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status vxRGBA2YUVInputValidator(vx_node node, vx_uint32 index);
    static vx_status vxRGBA2YUVOutputValidator(vx_node node, vx_uint32 index, vx_meta_format metaObj);
    static vx_status vxRGBA2YUVValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status vxRGBA2YUVInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status vxBilateralInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status vxYUV2RGBAInputValidator(vx_node node, vx_uint32 index);
    static vx_status vxYUV2RGBAOutputValidator(vx_node node, vx_uint32 index, vx_meta_format metaObj);
    static vx_status vxYUV2RGBAValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status vxYUV2RGBAInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);
  };
}
#endif
