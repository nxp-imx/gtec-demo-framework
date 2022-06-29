#ifndef OPENVX_STEREO_USERNODES_HPP
#define OPENVX_STEREO_USERNODES_HPP
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
    UserNodes(const std::shared_ptr<IContentManager>& contentManager);
    ~UserNodes();
    vx_status VXPublishNodeKernels(vx_context contextVX);
    vx_node NXPCensusNode(vx_graph graph, vx_image in, vx_image out);
    vx_node NXPMatchLRNode(vx_graph graph, vx_image inputCensusL, vx_image inputCensusR, vx_image outputDisparityLeft, const vx_int32 minDisparity,
                           const vx_int32 maxDisparity);
    vx_node NXPMatchRRNode(vx_graph graph, vx_image inputCensusL, vx_image inputCensusR, vx_image outputDisparityRight, const vx_int32 minDisparity,
                           const vx_int32 maxDisparity);
    vx_node NXPOutliersDetectionNode(vx_graph graph, vx_image disparityInput, vx_image disparytyOutput);
    vx_node NXPLeftRightConsistencyNode(vx_graph graph, vx_image inL, vx_image inR, vx_image outL, vx_image outR);
    vx_node NXPDisparityRefinementNode(vx_graph graph, vx_image imageIn, vx_image imageOut);
    vx_node NXPMedianFilterNode(vx_graph graph, vx_image imageIn, vx_image imageOut);

    vx_node NXPMatchFinerLRNode(vx_graph graph, const vx_image censusL, const vx_image censusR, vx_image outputLeft, const vx_int32 minDis,
                                const vx_int32 maxDis, const vx_image estimateLeft, const vx_image estimateRight, const vx_int32 rangeVariation);

    vx_node NXPMatchFinerRRNode(vx_graph graph, const vx_image censusL, const vx_image censusR, vx_image outputRight, const vx_int32 minDis,
                                const vx_int32 maxDis, const vx_image estimateLeft, const vx_image estimateRight, const vx_int32 rangeVariation);

  protected:
    static vx_status VXInputValidator(vx_node nodeObj, vx_uint32 index);
    static vx_status VXOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj);
    static vx_status VXValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status VXCensusInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status VXMatchInputValidator(vx_node nodeObj, vx_uint32 index);
    static vx_status VXMatchOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj);
    static vx_status VXMatchValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status VXMatchInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status VXOutliersInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status VXConsistencyInputValidator(vx_node nodeObj, vx_uint32 index);
    static vx_status VXConsistencyOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj);
    static vx_status VXConsistencyValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status VXConsistencyInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status VXDisparityInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status VXMedianInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);

    static vx_status VXMatchFinerInputValidator(vx_node nodeObj, vx_uint32 index);
    static vx_status VXMatchFinerOutputValidator(vx_node nodeObj, vx_uint32 index, vx_meta_format metaObj);
    static vx_status VXMatchFinerValidator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[]);
    static vx_status VXMatchFinerInitializer(vx_node nodObj, const vx_reference* paramObj, vx_uint32 paraNum);
  };
}
#endif
