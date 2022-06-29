/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include "OpenCLGaussianFilter.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/TypeConverter.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenCL1_2/ContextEx.hpp>
#include <FslUtil/OpenCL1_2/OpenCLHelper.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <RapidOpenCL1/Buffer.hpp>
#include <RapidOpenCL1/Check.hpp>
#include <RapidOpenCL1/CommandQueue.hpp>
#include <RapidOpenCL1/DebugStrings.hpp>
#include <RapidOpenCL1/Kernel.hpp>
#include <RapidOpenCL1/Program.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <vector>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    void PrintInfo(const cl_platform_id rplatformId, const cl_device_id deviceId)
    {
      constexpr const int arrSize = 4;
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      const char* attributeNames[arrSize] = {"Name", "Vendor", "Version", "Profile"};
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      const cl_platform_info attributeTypes[arrSize] = {CL_PLATFORM_NAME, CL_PLATFORM_VENDOR, CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE};
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      const cl_device_info deviceAttributeTypes[arrSize] = {CL_DEVICE_NAME, CL_DEVICE_VENDOR, CL_DEVICE_VERSION, CL_DEVICE_PROFILE};

      FSLLOG3_INFO("\n-=-=-=- Platform and Device information -=-=-=-\n\n");

      for (int count = 0; count < arrSize; ++count)
      {
        const std::string info = OpenCL::OpenCLHelper::GetPlatformInfo<std::string>(rplatformId, attributeTypes[count]);
        const std::string deviceInfo = OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, deviceAttributeTypes[count]);

        FSLLOG3_INFO("Platform Attributes {}: {}", attributeNames[count], info);
        FSLLOG3_INFO("Device Attributes {}: {}", attributeNames[count], deviceInfo);
      }

      const auto deviceItems = OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
      FSLLOG3_INFO("Device Max Work Item Dimensions: {}-D", deviceItems);

      const auto deviceSize = OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE);
      FSLLOG3_INFO("Device Max Work Group Size: {}", deviceSize);

      FSLLOG3_INFO("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    }


    RapidOpenCL1::Program BuildProgram(const cl_context context, const cl_device_id deviceId, const std::string& kernelSrc)
    {
      const char* pszKernelSource = kernelSrc.c_str();
      const std::size_t kernelSize = kernelSrc.size();

      RapidOpenCL1::Program program(context, 1, &pszKernelSource, &kernelSize);

      const cl_int error = clBuildProgram(program.Get(), 1, &deviceId, "", nullptr, nullptr);
      if (error != CL_SUCCESS)
      {
        const auto buildInfo = OpenCL::OpenCLHelper::GetProgramBuildInfoString(program.Get(), deviceId, CL_PROGRAM_BUILD_LOG);
        FSLLOG3_INFO(buildInfo);
        RAPIDOPENCL_CHECK(error);
      }
      return program;
    }

    void ProcessBitmapUsingOpenCL(Bitmap& rBitmap, const std::string& strProgram)
    {
      OpenCL::ContextEx context;
      cl_device_id deviceId = nullptr;
      context.Reset(CL_DEVICE_TYPE_GPU, &deviceId);

      RapidOpenCL1::CommandQueue commandQueue(context.Get(), deviceId, 0);

      PrintInfo(context.GetPlatformId(), deviceId);

      RapidOpenCL1::Program program = BuildProgram(context.Get(), deviceId, strProgram);
      RapidOpenCL1::Kernel kernel(program.Get(), "gaussian_filter");

      const std::size_t size2d = rBitmap.Width() * rBitmap.Height();
      RapidOpenCL1::Buffer gaussMemInput(context.Get(), CL_MEM_READ_ONLY, size2d, nullptr);
      RapidOpenCL1::Buffer gaussMemOutput(context.Get(), CL_MEM_WRITE_ONLY, size2d, nullptr);

      const auto width = static_cast<cl_int>(rBitmap.Width());
      const auto height = static_cast<cl_int>(rBitmap.Height());
      const cl_int channels = 1;
      clSetKernelArg(kernel.Get(), 0, sizeof(cl_mem), gaussMemInput.GetPointer());     // input
      clSetKernelArg(kernel.Get(), 1, sizeof(cl_mem), gaussMemOutput.GetPointer());    // output
      clSetKernelArg(kernel.Get(), 2, sizeof(cl_int), &width);                         // width
      clSetKernelArg(kernel.Get(), 3, sizeof(cl_int), &height);                        // height
      clSetKernelArg(kernel.Get(), 4, sizeof(cl_int), &channels);                      // channels

      {
        RawBitmapEx rawBitmap;
        Bitmap::ScopedDirectAccess scopedAccess(rBitmap, rawBitmap);
        auto* pImgData = static_cast<uint8_t*>(rawBitmap.Content());

        constexpr const int dimension = 2;
        const std::array<std::size_t, dimension> global = {rawBitmap.Width(), rawBitmap.Height()};
        constexpr std::array<std::size_t, dimension> local = {16, 8};
        RAPIDOPENCL_CHECK(clEnqueueWriteBuffer(commandQueue.Get(), gaussMemInput.Get(), CL_TRUE, 0, size2d, pImgData, 0, nullptr, nullptr));
        RAPIDOPENCL_CHECK(
          clEnqueueNDRangeKernel(commandQueue.Get(), kernel.Get(), dimension, nullptr, global.data(), local.data(), 0, nullptr, nullptr));
        RAPIDOPENCL_CHECK(clEnqueueReadBuffer(commandQueue.Get(), gaussMemOutput.Get(), CL_TRUE, 0, size2d, pImgData, 0, nullptr, nullptr));
      }
    }


    // OPENGLES STUFF

    std::array<VertexPositionTexture, 24> g_vertices = {
      // Draw A Quad

      // Top Right Of The Quad (Top)
      VertexPositionTexture(Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0f, 0.0f)),
      // Top Left Of The Quad (Top)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Right Of The Quad (Top)
      VertexPositionTexture(Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Left Of The Quad (Top)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f)),
      // Top Right Of The Quad (Bottom)
      VertexPositionTexture(Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0f, 1.0f)),
      // Top Left Of The Quad (Bottom)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Right Of The Quad (Bottom)
      VertexPositionTexture(Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Left Of The Quad (Bottom)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 0.0f)),
      // Top Right Of The Quad (Front)
      VertexPositionTexture(Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f)),
      // Top Left Of The Quad (Front)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Right Of The Quad (Front)
      VertexPositionTexture(Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Left Of The Quad (Front)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 0.0f)),

      // Top Right Of The Quad (Back)
      VertexPositionTexture(Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0f, 0.0f)),
      // Top Left Of The Quad (Back)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Right Of The Quad (Back)
      VertexPositionTexture(Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Left Of The Quad (Back)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 1.0f)),

      // Top Right Of The Quad (Left)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f)),
      // Top Left Of The Quad (Left)
      VertexPositionTexture(Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Right Of The Quad (Left)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Left Of The Quad (Left)
      VertexPositionTexture(Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 0.0f)),
      // Top Right Of The Quad (Right)
      VertexPositionTexture(Vector3(1.0f, 1.0f, -1.0f), Vector2(1.0f, 1.0f)),
      // Top Left Of The Quad (Right)
      VertexPositionTexture(Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f)),
      // Bottom Right Of The Quad (Right)
      VertexPositionTexture(Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0f, 0.0f)),
      // Bottom Left Of The Quad (Right)
      VertexPositionTexture(Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0f, 0.0f))};
  }

  OpenCLGaussianFilter::OpenCLGaussianFilter(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();

    Bitmap bitmap;
    content->Read(bitmap, "Test.png", PixelFormat::EX_ALPHA8_UNORM);
    GetPersistentDataManager()->Write("Test_gray.bmp", bitmap);

    ProcessBitmapUsingOpenCL(bitmap, content->ReadAllText("gaussian_filter.cl"));

    // Write the result to a file so we can take a detailed look at it
    GetPersistentDataManager()->Write("Test_gauss.bmp", bitmap);

    // We also use it as a texture
    GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    m_resources.Texture.SetData(bitmap, params);

    // Open GL setup
    m_resources.Program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_resources.Program.Get();

    // Get uniform locations
    m_resources.ModelViewMatrixLoc = glGetUniformLocation(hProgram, "g_matModelView");
    m_resources.ProjMatrixLoc = glGetUniformLocation(hProgram, "g_matProj");
    m_resources.VertexBuffer.Reset(g_vertices, GL_STATIC_DRAW);

    // prepare attribute links
    constexpr auto vertexDecl = VertexPositionTexture::GetVertexDeclarationArray();
    m_attribLink[0] =
      GLVertexAttribLink(m_resources.Program.GetAttribLocation("g_vPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    m_attribLink[1] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("g_vTexCoord"),
                                         vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));

    PrepareMatrices(config.ScreenResolution);

    GL_CHECK_FOR_ERROR();
  }


  OpenCLGaussianFilter::~OpenCLGaussianFilter() = default;


  void OpenCLGaussianFilter::Update(const DemoTime& demoTime)
  {
    m_angle.X -= 0.60f * demoTime.DeltaTime;
    m_angle.Y -= 0.50f * demoTime.DeltaTime;
    m_angle.Z -= 0.40f * demoTime.DeltaTime;

    // Rotate and translate the model view matrix
    m_vertexUboData.MatModel =
      Matrix::CreateRotationX(m_angle.X) * Matrix::CreateRotationY(m_angle.Y) * Matrix::CreateRotationZ(m_angle.Z) * m_matTranslate;
  }


  void OpenCLGaussianFilter::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();
    glViewport(0, 0, currentSizePx.Width(), currentSizePx.Height());

    // Clear the color-buffer and depth-buffer
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If enabled, do depth comparisons and update the depth buffer
    glEnable(GL_DEPTH_TEST);
    // If enabled, cull polygons based on their winding in window coordinates
    glEnable(GL_CULL_FACE);
    // Culls back face
    glCullFace(GL_BACK);

    // Set the shader program
    glUseProgram(m_resources.Program.Get());

    // Load the matrices
    glUniformMatrix4fv(m_resources.ModelViewMatrixLoc, 1, 0, m_vertexUboData.MatModel.DirectAccess());
    glUniformMatrix4fv(m_resources.ProjMatrixLoc, 1, 0, m_vertexUboData.MatProj.DirectAccess());

    // Set up VBO Vertex Attribute information
    glBindBuffer(m_resources.VertexBuffer.GetTarget(), m_resources.VertexBuffer.Get());

    // Bind the vertex attributes
    m_resources.VertexBuffer.EnableAttribArrays(m_attribLink);

    // Select Our Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_resources.Texture.Get());

    // Drawing Using Triangle strips, draw triangle strips using 4 vertices
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

    // Cleanup
    m_resources.VertexBuffer.DisableAttribArrays();
  }


  void OpenCLGaussianFilter::PrepareMatrices(const Point2& currentSize)
  {
    const float aspectRatio = static_cast<float>(currentSize.X) / static_cast<float>(currentSize.Y);
    m_vertexUboData.MatProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(60.0f), aspectRatio, 1.0f, 1000.0f);
    m_matTranslate = Matrix::CreateTranslation(0.0f, 0.0f, -3.0f);
  }
}
