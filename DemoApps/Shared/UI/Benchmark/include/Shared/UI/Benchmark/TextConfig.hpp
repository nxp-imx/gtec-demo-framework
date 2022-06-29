#ifndef SHARED_UI_BENCHMARK_TEXTCONFIG_HPP
#define SHARED_UI_BENCHMARK_TEXTCONFIG_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/String/StringViewLite.hpp>

namespace Fsl::TextConfig
{
  constexpr StringViewLite HeaderSettings("Settings:");
  constexpr StringViewLite HeaderOptions("Options:");
  constexpr StringViewLite HeaderRenderMethod("Render method:");
  constexpr StringViewLite HeaderRenderOptions("Render options:");

  constexpr StringViewLite GpuTimestamps("GPU timestamps");
  constexpr StringViewLite NoOpaqueMaterials("No opaque materials");
  constexpr StringViewLite UseSdfFonts("Use SDF fonts");
  constexpr StringViewLite OnDemandRendering("On demand");

  constexpr StringViewLite Batch("Batch");
  constexpr StringViewLite FillBuffers("Fill buffers");
  constexpr StringViewLite DepthBuffer("Depth buffer");
  constexpr StringViewLite DrawReorder("Draw reorder");
  constexpr StringViewLite MeshCaching("Mesh caching");
  constexpr StringViewLite PreferFastReorder("Prefer fast reorder");

}

#endif
