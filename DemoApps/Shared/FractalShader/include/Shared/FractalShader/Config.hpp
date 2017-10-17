#ifndef SHARED_FRACTALSHADER_CONFIG_HPP
#define SHARED_FRACTALSHADER_CONFIG_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Point2.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{

  struct Quality
  {
    enum Enum
    {
      Low,
      Medium,
      High,
      COUNT
    };
  };

  struct RenderMode
  {
    enum Enum
    {
      Gray,
      Col,
      Smooth,
      //Tex,
      COUNT
    };
  };


  struct AnimationMode
  {
    enum Enum
    {
      ConstantLoad = 0,
      ConstantLoad2 = 1,
      ConstantLoad3 = 2,
      ComplexMove = 3,
      SlowAndComplex = 4,
      DSin = 5,
      Stopped = 6,
      COUNT
    };
  };


  class Config
  {
  public:
    Quality::Enum TheQuality;
    RenderMode::Enum TheRenderMode;
    int32_t Scene;
    int32_t Iterations;
    AnimationMode::Enum TheAnimationMode;
    bool ForceUnroll;
    bool Show;

    Config()
      : TheQuality(Quality::Medium)
      , TheRenderMode(RenderMode::Col)
      , Scene(0)
      , Iterations(20)
      , TheAnimationMode(AnimationMode::ComplexMove)
      , ForceUnroll(false)
      , Show(false)
    {

    }


    void SetIterations(const int32_t value)
    {
      if (value < 1)
        throw std::invalid_argument("invalid iteration limit");
      Iterations = value;
    }

    void SetScene(const int32_t value)
    {
      if (value < 0 || value >= 2)
        throw std::invalid_argument("Unknown scene");
      Scene = value;
    }

    void SetRenderMode(const RenderMode::Enum value)
    {
      if (value < 0 || value >= RenderMode::COUNT)
        throw std::invalid_argument("Unknown RenderMode");

      TheRenderMode = value;
    }

    void SetAnimationMode(const AnimationMode::Enum value)
    {
      if (value < 0 || value >= AnimationMode::COUNT)
        throw std::invalid_argument("Unknown AnimationMode");

      TheAnimationMode = value;
    }

    void SetQuality(const Quality::Enum value)
    {
      if (value < 0 || value >= Quality::COUNT)
        throw std::invalid_argument("Unknown Quality");

      TheQuality = value;
    }

    void SetForceUnroll(const bool value)
    {
      ForceUnroll = value;
    }

    void SetShow(const bool value)
    {
      Show = value;
    }

  };
}

#endif
