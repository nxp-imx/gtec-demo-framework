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
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif


#include <FslGraphicsGLES3/Exceptions.hpp>
#include <FslGraphicsGLES3/GLCheck.hpp>
#include "DirectMultiSamplingVideoYUV.hpp"
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <iostream>
#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>
#include <gst/app/gstappsink.h>
#include <gst/gstallocator.h>


namespace Fsl
{
  namespace
  {
    const GLfloat g_vertices[][2] =
    {
      { -1.0f, 0.0f },
      { 0.0f, 0.0f },
      { -1.0f, 1.0f },
      { 0.0f, 1.0f }
    };

    const GLfloat g_texcoords[][2] = {
      { 0.0f, 1.0f },
      { 1.0f, 1.0f },
      { 0.0f, 0.0f },
      { 1.0f, 0.0f }
    };

    GLfloat g_transformMatrix[16] =
    {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
    };

    const char*const g_pszShaderAttributeArray[] =
    {
      "my_Vertex",
      "my_Texcoor",
      nullptr
    };

#define WIDTH 320
#define HEIGHT 240
#define GST_COMMAND   "filesrc location=./Content/Video.mp4 ! video/quicktime ! aiurdemux ! queue ! vpudec output-format=1 name=mydec ! queue  ! intervideosink name=mysink"
    pthread_mutex_t gstThreadMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t gstCON = PTHREAD_COND_INITIALIZER;
    GstElement *pipeLine;
    guint busWatchID;
    uint32_t yAddr;
    uint32_t  uvAddr;
    uint32_t *vYaddr;
    uint32_t *vUVaddr;

    //extracting vpu physical memory from gstreamer pipeline
    typedef struct {
      guint8 *vaddr;
      guint8 *paddr;
      gsize size;
      gpointer *user_data;
    } PhyMemBlock;

    typedef struct {
      GstMemory mem;
      guint8 *vaddr;
      guint8 *paddr;
      PhyMemBlock block;
    } GstMemoryPhy;

    PhyMemBlock *gst_buffer_query_phymem_block(GstBuffer *buffer)
    {
      GstMemory *mem;
      GstMemoryPhy *memphy;
      PhyMemBlock *memblk;
      mem = gst_buffer_get_memory(buffer, 0);
      if (mem == NULL) {
        GST_ERROR("Not get memory from buffer.\n");
        return NULL;
      }

      memphy = (GstMemoryPhy*)mem;
      memblk = &memphy->block;

      gst_memory_unref(mem);

      return memblk;
    }

    //gstreamer thread
    void* gstLibThread(void* tharg)
    {
      GMainLoop *loop = (GMainLoop*)tharg;
      g_main_loop_run(loop);

      /* clean up */
      gst_element_set_state(pipeLine, GST_STATE_NULL);

      gst_object_unref(GST_OBJECT(pipeLine));
      g_source_remove(busWatchID);
      g_main_loop_unref(loop);

      return NULL;
    }

    gboolean busCall(GstBus  *bus, GstMessage *msg, gpointer data)
    {

      GMainLoop *loop = (GMainLoop *)data;

      switch (GST_MESSAGE_TYPE(msg)) {

      case GST_MESSAGE_EOS:
        /* restart playback if at end */
        if (!gst_element_seek(pipeLine,
          1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
          GST_SEEK_TYPE_SET, 0,
          GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
          g_main_loop_quit(loop);
        }
        break;

      case GST_MESSAGE_ERROR: {
        gchar  *debug;
        GError *error;

        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);
        g_error_free(error);

        g_main_loop_quit(loop);
        break;
      }
      default:
        break;
      }

      return TRUE;
    }
    void Invalid(uint32_t *phy, uint32_t * logical)
    {
      pthread_mutex_lock(&gstThreadMutex);
      {
        yAddr = *((int*)(&phy));
        uvAddr = yAddr + WIDTH*HEIGHT;
        vYaddr = logical;
        vUVaddr = vYaddr + WIDTH*HEIGHT;
        pthread_cond_signal(&gstCON);
      }
      pthread_mutex_unlock(&gstThreadMutex);
    }

    //vpudec pad probe call back function
    GstPadProbeReturn VpudecOutputBufferProbe(GstPad * pad, GstPadProbeInfo * info, gpointer unused)
    {
      GstBuffer *gstbuffer = GST_PAD_PROBE_INFO_BUFFER(info);
      PhyMemBlock * memblk;
      memblk = gst_buffer_query_phymem_block(gstbuffer);
      Invalid((uint32_t *)(memblk->paddr), (uint32_t *)(memblk->vaddr));
      return GST_PAD_PROBE_OK;
    }

    void InitGstreamer()
    {
      GstElement * vpuDec = NULL;
      pthread_t thread;
      gst_init(NULL, NULL);

      GMainLoop *loop;
      GError* error = NULL;
      GstBus *bus;
      GstPad *sinkPad;

      loop = g_main_loop_new(NULL, FALSE);

      pipeLine = gst_parse_launch(GST_COMMAND, &error);

      if (!pipeLine) {
        g_error_free(error);
      }
      vpuDec = gst_bin_get_by_name(GST_BIN(pipeLine), "mydec");
      /* we add a message handler */
      bus = gst_pipeline_get_bus(GST_PIPELINE(pipeLine));
      busWatchID = gst_bus_add_watch(bus, busCall, loop);
      sinkPad = gst_element_get_static_pad(vpuDec, "src");
      gst_pad_add_probe(sinkPad, GST_PAD_PROBE_TYPE_BUFFER, VpudecOutputBufferProbe, NULL, NULL);
      gst_object_unref(sinkPad);

      gst_object_unref(bus);
      gst_element_set_state(pipeLine, GST_STATE_PLAYING);
      pthread_create(&thread, NULL, gstLibThread, (void*)loop);

    }

  }


  DirectMultiSamplingVideoYUV::DirectMultiSamplingVideoYUV(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_program()
    , m_yTex(0)
    , m_uvTex(0)
    , m_locVertices(0)
    , m_locTexCoord(0)
    , m_locTransformMat(0)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"), g_pszShaderAttributeArray);

    const GLuint hProgram = m_program.Get();
    // Grab location of shader attributes.
    GL_CHECK(m_locVertices = glGetAttribLocation(hProgram, "my_Vertex"));
    GL_CHECK(m_locTexCoord = glGetAttribLocation(hProgram, "my_Texcoor"));
    // Transform Matrix is uniform for all vertices here.
    GL_CHECK(m_locTransformMat = glGetUniformLocation(hProgram, "my_TransMatrix"));
    GL_CHECK(glUseProgram(hProgram));

    m_locSampler[0] = glGetUniformLocation(hProgram, "my_Texture0");
    m_locSampler[1] = glGetUniformLocation(hProgram, "my_Texture1");

    InitYUVTextures();
    InitGstreamer();

    GL_CHECK(glEnableVertexAttribArray(m_locVertices));
    GL_CHECK(glEnableVertexAttribArray(m_locTexCoord));

    // set data in the arrays
    GL_CHECK(glVertexAttribPointer(m_locVertices, 2, GL_FLOAT, GL_FALSE, 0, &g_vertices[0][0]));
    GL_CHECK(glVertexAttribPointer(m_locTexCoord, 2, GL_FLOAT, GL_FALSE, 0, &g_texcoords[0][0]));
    GL_CHECK(glUniformMatrix4fv(m_locTransformMat, 1, GL_FALSE, g_transformMatrix));
  }


  DirectMultiSamplingVideoYUV::~DirectMultiSamplingVideoYUV()
  {
    glDisableVertexAttribArray(m_locVertices);
    glDisableVertexAttribArray(m_locTexCoord);
    glDeleteTextures(1, &m_yTex);
    glDeleteTextures(1, &m_uvTex);
  }

  void DirectMultiSamplingVideoYUV::InitYUVTextures()
  {
    // init y texture
    GL_CHECK(glGenTextures(1, &m_yTex));
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_yTex));
    GL_CHECK(glUniform1i(m_locSampler[0], 0));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // init uv texture
    GL_CHECK(glGenTextures(1, &m_uvTex));
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + 1));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_uvTex));
    GL_CHECK(glUniform1i(m_locSampler[1], 1));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  }

  void DirectMultiSamplingVideoYUV::Update(const DemoTime& demoTime)
  {
  }


  void DirectMultiSamplingVideoYUV::Draw(const DemoTime& demoTime)
  {
    pthread_mutex_lock(&gstThreadMutex);
    pthread_cond_wait(&gstCON, &gstThreadMutex);
    {
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      void* logical = (void*)vYaddr;
      glActiveTexture(GL_TEXTURE0);
      glUniform1i(m_locSampler[0], 0);
      glBindTexture(GL_TEXTURE_2D, m_yTex);
      glTexDirectVIVMap(GL_TEXTURE_2D, WIDTH, HEIGHT, GL_ALPHA, &logical, &yAddr);
      glTexDirectInvalidateVIV(GL_TEXTURE_2D);

      logical = (void*)vUVaddr;
      glActiveTexture(GL_TEXTURE1);
      glUniform1i(m_locSampler[1], 1);
      glBindTexture(GL_TEXTURE_2D, m_uvTex);
      glTexDirectVIVMap(GL_TEXTURE_2D, WIDTH / 2, HEIGHT / 2, GL_LUMINANCE8_ALPHA8_EXT, &logical, &uvAddr);
      glTexDirectInvalidateVIV(GL_TEXTURE_2D);

      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    pthread_mutex_unlock(&gstThreadMutex);

    // flush all commands.
    glFlush();
  }

}
