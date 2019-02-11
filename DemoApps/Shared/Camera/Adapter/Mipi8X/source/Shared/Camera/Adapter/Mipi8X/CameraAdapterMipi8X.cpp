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

#include <Shared/Camera/Adapter/Mipi8X/CameraAdapterMipi8X.hpp>
#include <FslBase/Log/Log.hpp>

// Required UNIX HEADERS
#include <linux/videodev2.h>
#include <malloc.h>
#include <pthread.h>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <stdexcept>

// MIPI defines
#define TFAIL -1
#define TPASS 0
#define CAMERA_NUM 1
#define TEST_BUFFER_NUM 3
#define MAX_V4L2_DEVICE_NR 64

namespace Fsl
{
  namespace Helios
  {
    namespace
    {
      struct testbuffer
      {
        unsigned char* start;
        size_t offset;
        unsigned int length;
      };

      struct video_channel
      {
        /* v4l2 info */
        int init;
        int on;
        int index;    // video channel index
        int out_width;
        int out_height;
        int cap_fmt;
        int frame_size;
        int v4l_dev;
        char v4l_dev_name[100];
        int mem_type;
        char save_file_name[100];
        FILE* pfile;
        int cur_buf_id;
        struct testbuffer buffers[TEST_BUFFER_NUM];
        int frame_num;

        /* fb info */
        int x_offset;
        int y_offset;
      };

      struct video_channel video_ch[8];

      int g_cam_max = 8; /* MAX support video channel */

      int g_cam = 0; /* video channel select */
      int g_cam_num = 0;
      int g_cap_fmt = V4L2_PIX_FMT_RGB24;
      int g_capture_mode = 0;
      int g_timeout = 10;
      int g_camera_framerate = 30; /* 30 fps */
      int g_loop = 0;
      int g_mem_type = V4L2_MEMORY_MMAP;

      char g_v4l_device[8][100] = {"/dev/video0", "/dev/video1", "/dev/video2", "/dev/video3",
                                   "/dev/video4", "/dev/video5", "/dev/video6", "/dev/video7"};

      static void print_pixelformat(char* prefix, int val)
      {
        printf("%s: %c%c%c%c\n", prefix != nullptr ? prefix : "pixelformat", val & 0xff, (val >> 8) & 0xff, (val >> 16) & 0xff, (val >> 24) & 0xff);
      }

      int init_video_channel(int ch_id, int cameraWidth, int cameraHeight)
      {
        video_ch[ch_id].init = 1;
        video_ch[ch_id].out_width = cameraWidth;
        video_ch[ch_id].out_height = cameraHeight;
        video_ch[ch_id].cap_fmt = g_cap_fmt;
        video_ch[ch_id].mem_type = g_mem_type;

        strcpy(video_ch[ch_id].v4l_dev_name, g_v4l_device[ch_id]);
        printf("%s, %s init %d\n", __func__, video_ch[ch_id].v4l_dev_name, ch_id);
        return 0;
      }

      int prepare_capturing(int ch_id)
      {
        unsigned int i;
        struct v4l2_buffer buf;
        struct v4l2_requestbuffers req;
        struct v4l2_plane planes = {0};
        int fd_v4l = video_ch[ch_id].v4l_dev;
        int mem_type = video_ch[ch_id].mem_type;

        memset(&req, 0, sizeof(req));
        req.count = TEST_BUFFER_NUM;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        req.memory = mem_type;

        if (ioctl(fd_v4l, VIDIOC_REQBUFS, &req) < 0)
        {
          printf("VIDIOC_REQBUFS failed\n");
          return -1;
        }

        if (mem_type == V4L2_MEMORY_MMAP)
        {
          for (i = 0; i < TEST_BUFFER_NUM; i++)
          {
            memset(&buf, 0, sizeof(buf));
            memset(&planes, 0, sizeof(planes));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
            buf.memory = mem_type;
            buf.m.planes = &planes;
            buf.length = 1; /* plane num */
            buf.index = i;
            if (ioctl(fd_v4l, VIDIOC_QUERYBUF, &buf) < 0)
            {
              printf("VIDIOC_QUERYBUF error\n");
              return -1;
            }

            video_ch[ch_id].buffers[i].length = buf.m.planes->length;
            video_ch[ch_id].buffers[i].offset = static_cast<size_t>(buf.m.planes->m.mem_offset);
            video_ch[ch_id].buffers[i].start = static_cast<unsigned char*>(
              mmap(nullptr, video_ch[ch_id].buffers[i].length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_v4l, video_ch[ch_id].buffers[i].offset));
            memset(video_ch[ch_id].buffers[i].start, 0xFF, video_ch[ch_id].buffers[i].length);
            printf("buffer[%d] startAddr=0x%x, offset=0x%x, buf_size=%d\n", i, reinterpret_cast<unsigned int*>(video_ch[ch_id].buffers[i].start),
                   video_ch[ch_id].buffers[i].offset, video_ch[ch_id].buffers[i].length);
          }
        }

        for (i = 0; i < TEST_BUFFER_NUM; i++)
        {
          memset(&buf, 0, sizeof(buf));
          memset(&planes, 0, sizeof(planes));
          buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
          buf.memory = mem_type;
          buf.m.planes = &planes;
          buf.index = i;
          buf.m.planes->length = video_ch[ch_id].buffers[i].length;
          buf.length = 1;
          if (mem_type == V4L2_MEMORY_USERPTR)
          {
            buf.m.planes->m.userptr = reinterpret_cast<unsigned long>(video_ch[ch_id].buffers[i].start);
          }
          else
          {
            buf.m.planes->m.mem_offset = video_ch[ch_id].buffers[i].offset;
          }

          if (ioctl(fd_v4l, VIDIOC_QBUF, &buf) < 0)
          {
            printf("VIDIOC_QBUF error\n");
            return -1;
          }
        }
        return 0;
      }

      int start_capturing(int ch_id)
      {
        enum v4l2_buf_type type;
        int fd_v4l = video_ch[ch_id].v4l_dev;
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        if (ioctl(fd_v4l, VIDIOC_STREAMON, &type) < 0)
        {
          printf("VIDIOC_STREAMON error\n");
          return -1;
        }
        printf("%s channel=%d, v4l_dev=0x%x\n", __func__, ch_id, fd_v4l);
        return 0;
      }

      int stop_capturing(int ch_id)
      {
        enum v4l2_buf_type type;
        int fd_v4l = video_ch[ch_id].v4l_dev;
        int nframe = video_ch[ch_id].frame_num;
        printf("%s channel=%d, v4l_dev=0x%x, frames=%d\n", __func__, ch_id, fd_v4l, nframe);
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        return ioctl(fd_v4l, VIDIOC_STREAMOFF, &type);
      }

      int v4l_capture_setup(int ch_id)
      {
        struct v4l2_format fmt;
        struct v4l2_streamparm parm;
        struct v4l2_fmtdesc fmtdesc;
        struct v4l2_capability cap;
        int fd_v4l;
        int i;

        printf("Try to open device %s\n", video_ch[ch_id].v4l_dev_name);
        if ((fd_v4l = open(video_ch[ch_id].v4l_dev_name, O_RDWR, 0)) < 0)
        {
          printf("unable to open v4l2 %s for capture device.\n", video_ch[ch_id].v4l_dev_name);
          return -1;
        }

        if (ioctl(fd_v4l, VIDIOC_QUERYCAP, &cap) == 0)
        {
          printf("cap=0x%x\n", cap.capabilities);
          if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) == 0u)
          {
            printf("%s not support v4l2 capture device.\n", video_ch[ch_id].v4l_dev_name);
            return -1;
          }
        }
        else
        {
          close(fd_v4l);
          return -1;
        }

        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        /* enum channels fmt */
        for (i = 0; i <= ch_id; i++)
        {
          fmtdesc.index = i;
          if (ioctl(fd_v4l, VIDIOC_ENUM_FMT, &fmtdesc) < 0)
          {
            printf("VIDIOC ENUM FMT failed, index=%d \n", i);
            break;
          }
          printf("index=%d\n", fmtdesc.index);
          print_pixelformat("pixelformat (output by camera)", fmtdesc.pixelformat);
        }

        memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        fmt.fmt.pix_mp.pixelformat = video_ch[ch_id].cap_fmt;
        fmt.fmt.pix_mp.width = video_ch[ch_id].out_width;
        fmt.fmt.pix_mp.height = video_ch[ch_id].out_height;
        fmt.fmt.pix_mp.num_planes = 1; /* RGB */
        if (ioctl(fd_v4l, VIDIOC_S_FMT, &fmt) < 0)
        {
          printf("Set format failed\n");
          goto fail;
        }
        else
        {
          printf("Set format Success\n");
        }

        memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        if (ioctl(fd_v4l, VIDIOC_G_FMT, &fmt) < 0)
        {
          printf("get format failed\n");
          goto fail;
        }
        printf("video_ch=%d, width=%d, height=%d, \n", ch_id, fmt.fmt.pix_mp.width, fmt.fmt.pix_mp.height);
        print_pixelformat("Pixelformat from V4L", fmt.fmt.pix_mp.pixelformat);

        memset(&parm, 0, sizeof(parm));
        parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        if (ioctl(fd_v4l, VIDIOC_G_PARM, &parm) < 0)
        {
          printf("VIDIOC_G_PARM failed\n");
          parm.parm.capture.timeperframe.denominator = g_camera_framerate;
        }

        printf("\t WxH@fps = %dx%d@%d", fmt.fmt.pix_mp.width, fmt.fmt.pix_mp.height, parm.parm.capture.timeperframe.denominator);
        printf("\t Image size = %d\n", fmt.fmt.pix_mp.plane_fmt[0].sizeimage);

        video_ch[ch_id].frame_size = fmt.fmt.pix_mp.plane_fmt[0].sizeimage;

        video_ch[ch_id].v4l_dev = fd_v4l;
        video_ch[ch_id].on = 1;

        printf("%s, Open v4l_dev=0x%x, channel=%d\n", __func__, video_ch[ch_id].v4l_dev, ch_id);
        return 0;
      fail:
        close(fd_v4l);
        return -1;
      }

      int get_video_channel_buffer(int ch_id)
      {
        int fd_v4l = video_ch[ch_id].v4l_dev;
        struct v4l2_buffer buf;
        struct v4l2_plane planes = {0};

        memset(&buf, 0, sizeof(buf));
        memset(&planes, 0, sizeof(planes));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = video_ch[ch_id].mem_type;
        buf.m.planes = &planes;
        buf.length = 1;
        if (ioctl(fd_v4l, VIDIOC_DQBUF, &buf) < 0)
        {
          printf("VIDIOC_DQBUF failed.\n");
          return -1;
        }

        video_ch[ch_id].frame_num++;
        video_ch[ch_id].cur_buf_id = buf.index;
        return 0;
      }

      int put_video_channel_buffer(int ch_id)
      {
        int fd_v4l = video_ch[ch_id].v4l_dev;
        struct v4l2_buffer buf;
        struct v4l2_plane planes = {0};

        int buf_id = video_ch[ch_id].cur_buf_id;

        memset(&buf, 0, sizeof(buf));
        memset(&planes, 0, sizeof(planes));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = g_mem_type;
        buf.m.planes = &planes;
        buf.index = buf_id;
        buf.m.planes->length = video_ch[ch_id].buffers[buf_id].length;
        buf.length = 1;

        if (ioctl(fd_v4l, VIDIOC_QBUF, &buf) < 0)
        {
          printf("VIDIOC_QBUF failed, video=%d\n", ch_id);
          return -1;
        }
        return 0;
      }
    }

    CameraAdapterMipi8X::CameraAdapterMipi8X(const CameraAdapterAllocateInfo& allocateInfo)
      : m_extent(allocateInfo.Extent)
      , m_pixelFormat(PixelFormat::R8G8B8A8_UNORM)
      , m_cam_num(1)
      , m_frameId(0)
    {
      if (!allocateInfo.Flags.IsEnabled(CameraAdapterAllocateFlags::CustomExtent))
      {
        FSLLOG("FIX: Using fixed default size");
        m_extent = Extent2D(640, 480);
      }

      int ret;

      g_cam_num = m_cam_num;

      // Only 1 camera will be initialized
      init_video_channel(0, m_extent.Width, m_extent.Height);
      ret = v4l_capture_setup(0);


      if (ret < 0)
      {
        close(video_ch[0].v4l_dev);
        throw std::runtime_error("v4l_capture_setup failed");
      }

      if (video_ch[0].on != 0)
      {
        if (prepare_capturing(0) < 0)
        {
          close(video_ch[0].v4l_dev);
          throw std::runtime_error("prepare_capturing failed");
        }
        if (start_capturing(0) < 0)
        {
          stop_capturing(0);
          close(video_ch[0].v4l_dev);
          throw std::runtime_error("start_capturing failed");
        }
      }
    }

    CameraAdapterMipi8X::~CameraAdapterMipi8X()
    {
      FSLLOG_WARNING("FIX: add camera shutdown code here");
    }

    CameraAdapterConfig CameraAdapterMipi8X::GetConfig() const
    {
      return CameraAdapterConfig(m_extent, m_pixelFormat);
    }

    bool CameraAdapterMipi8X::TryRender(RawBitmapEx& rTargetBitmap, uint32_t& rFrameId)
    {
      // The camera class will have checked these so we just have asserts here.
      assert(rTargetBitmap.GetExtent() == m_extent);
      assert(rTargetBitmap.Stride() == PixelFormatUtil::CalcMinimumStride(m_extent.Width, m_pixelFormat));
      assert(rTargetBitmap.GetPixelFormat() == m_pixelFormat);
      rFrameId = 0;

      int ret;
      for (int i = 0; i < 8; i++)
      {
        if (video_ch[i].on != 0)
        {
          ret = get_video_channel_buffer(i);
          if (ret < 0)
          {
            // FIX: Consider if this error is critical and should shutdown the app, or if we just return false and let the user try again
            //      The new method description recommends to return false and let the user retry.

            // FIX: When we add the 'Destructor' shutdown code we might not need do the close here
            close(video_ch[i].v4l_dev);
            throw std::runtime_error("get_video_channel_buffer failed");
          }
          int buf_id = video_ch[i].cur_buf_id;
          memcpy(rTargetBitmap.Content(), reinterpret_cast<void*>(video_ch[i].buffers[buf_id].start),
                 rTargetBitmap.Height() * rTargetBitmap.Stride());
          put_video_channel_buffer(i);
        }
      }
      ++m_frameId;
      if (m_frameId == 0)
      {
        ++m_frameId;
      }
      rFrameId = m_frameId;
      return true;
    }
  }
}
