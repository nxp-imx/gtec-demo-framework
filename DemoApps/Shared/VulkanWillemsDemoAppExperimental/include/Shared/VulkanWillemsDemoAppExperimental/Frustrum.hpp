#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_FRUSTRUM_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_FRUSTRUM_HPP
/*
 * View frustum culling class
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <vulkan/vulkan.h>
#include <array>
#include <cmath>
#include <glm/glm.hpp>

namespace Fsl
{
  namespace Willems
  {
    class Frustum
    {
    public:
      enum Side
      {
        LEFT = 0,
        RIGHT = 1,
        TOP = 2,
        BOTTOM = 3,
        BACK = 4,
        FRONT = 5
      };
      std::array<glm::vec4, 6> Planes;

      void Update(const glm::mat4& matrix)
      {
        Planes[LEFT].x = matrix[0].w + matrix[0].x;
        Planes[LEFT].y = matrix[1].w + matrix[1].x;
        Planes[LEFT].z = matrix[2].w + matrix[2].x;
        Planes[LEFT].w = matrix[3].w + matrix[3].x;

        Planes[RIGHT].x = matrix[0].w - matrix[0].x;
        Planes[RIGHT].y = matrix[1].w - matrix[1].x;
        Planes[RIGHT].z = matrix[2].w - matrix[2].x;
        Planes[RIGHT].w = matrix[3].w - matrix[3].x;

        Planes[TOP].x = matrix[0].w - matrix[0].y;
        Planes[TOP].y = matrix[1].w - matrix[1].y;
        Planes[TOP].z = matrix[2].w - matrix[2].y;
        Planes[TOP].w = matrix[3].w - matrix[3].y;

        Planes[BOTTOM].x = matrix[0].w + matrix[0].y;
        Planes[BOTTOM].y = matrix[1].w + matrix[1].y;
        Planes[BOTTOM].z = matrix[2].w + matrix[2].y;
        Planes[BOTTOM].w = matrix[3].w + matrix[3].y;

        Planes[BACK].x = matrix[0].w + matrix[0].z;
        Planes[BACK].y = matrix[1].w + matrix[1].z;
        Planes[BACK].z = matrix[2].w + matrix[2].z;
        Planes[BACK].w = matrix[3].w + matrix[3].z;

        Planes[FRONT].x = matrix[0].w - matrix[0].z;
        Planes[FRONT].y = matrix[1].w - matrix[1].z;
        Planes[FRONT].z = matrix[2].w - matrix[2].z;
        Planes[FRONT].w = matrix[3].w - matrix[3].z;

        for (std::size_t i = 0; i < Planes.size(); ++i)
        {
          const float length = std::sqrt(Planes[i].x * Planes[i].x + Planes[i].y * Planes[i].y + Planes[i].z * Planes[i].z);
          Planes[i] /= length;
        }
      }

      bool checkSphere(glm::vec3 pos, float radius)
      {
        for (std::size_t i = 0; i < Planes.size(); ++i)
        {
          if ((Planes[i].x * pos.x) + (Planes[i].y * pos.y) + (Planes[i].z * pos.z) + Planes[i].w <= -radius)
          {
            return false;
          }
        }
        return true;
      }
    };
  }
}

#endif
