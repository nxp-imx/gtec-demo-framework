#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_CAMERA_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_CAMERA_HPP
/*
 * Basic camera class
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Fsl
{
  namespace Willems
  {
    class Camera
    {
    private:
      float Fov;
      float ZNear;
      float ZFar;

      void UpdateViewMatrix()
      {
        glm::mat4 rotM = glm::mat4(1.0f);
        glm::mat4 transM;

        rotM = glm::rotate(rotM, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        rotM = glm::rotate(rotM, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotM = glm::rotate(rotM, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        transM = glm::translate(glm::mat4(1.0f), Position);

        if (Type == CameraType::FirstPerson)
        {
          Matrices.View = rotM * transM;
        }
        else
        {
          Matrices.View = transM * rotM;
        }
      };

    public:
      enum CameraType
      {
        LookAt,
        FirstPerson
      };

      CameraType Type = CameraType::LookAt;

      glm::vec3 Rotation = glm::vec3();
      glm::vec3 Position = glm::vec3();

      float RotationSpeed = 1.0f;
      float MovementSpeed = 1.0f;

      bool updated = false;

      struct
      {
        glm::mat4 Perspective;
        glm::mat4 View;
      } Matrices;

      struct KeysInfo
      {
        bool Left{false};
        bool Right{false};
        bool Up{false};
        bool Down{false};

        KeysInfo() = default;
      } Keys;

      bool Moving()
      {
        return Keys.Left || Keys.Right || Keys.Up || Keys.Down;
      }

      float GetNearClip() const
      {
        return ZNear;
      }

      float GetFarClip() const
      {
        return ZFar;
      }

      void SetPerspective(const float fov, const float aspect, const float znear, const float zfar)
      {
        Fov = fov;
        ZNear = znear;
        ZFar = zfar;
        Matrices.Perspective = glm::perspective(glm::radians(Fov), aspect, znear, zfar);
      };

      void UpdateAspectRatio(float aspect)
      {
        Matrices.Perspective = glm::perspective(glm::radians(Fov), aspect, ZNear, ZFar);
      }

      void SetPosition(const glm::vec3& position)
      {
        Position = position;
        UpdateViewMatrix();
      }

      void SetRotation(const glm::vec3& rotation)
      {
        Rotation = rotation;
        UpdateViewMatrix();
      };

      void Rotate(const glm::vec3& delta)
      {
        Rotation += delta;
        UpdateViewMatrix();
      }

      void SetTranslation(const glm::vec3& translation)
      {
        Position = translation;
        UpdateViewMatrix();
      };

      void Translate(const glm::vec3& delta)
      {
        Position += delta;
        UpdateViewMatrix();
      }

      void Update(const float deltaTime)
      {
        updated = false;
        if (Type == CameraType::FirstPerson)
        {
          if (Moving())
          {
            glm::vec3 camFront;
            camFront.x = -cos(glm::radians(Rotation.x)) * sin(glm::radians(Rotation.y));
            camFront.y = sin(glm::radians(Rotation.x));
            camFront.z = cos(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
            camFront = glm::normalize(camFront);

            float moveSpeed = deltaTime * MovementSpeed;

            if (Keys.Up)
            {
              Position += camFront * moveSpeed;
            }
            if (Keys.Down)
            {
              Position -= camFront * moveSpeed;
            }
            if (Keys.Left)
            {
              Position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
            }
            if (Keys.Right)
            {
              Position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
            }

            UpdateViewMatrix();
          }
        }
      };

      // Update camera passing separate axis data (gamepad)
      // Returns true if view or position has been changed
      bool UpdatePad(const glm::vec2& axisLeft, const glm::vec2& axisRight, const float deltaTime)
      {
        bool retVal = false;

        if (Type == CameraType::FirstPerson)
        {
          // Use the common console thumbstick layout
          // Left = view, right = move

          const float deadZone = 0.0015f;
          const float range = 1.0f - deadZone;

          glm::vec3 camFront;
          camFront.x = -cos(glm::radians(Rotation.x)) * sin(glm::radians(Rotation.y));
          camFront.y = sin(glm::radians(Rotation.x));
          camFront.z = cos(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
          camFront = glm::normalize(camFront);

          float moveSpeed = deltaTime * MovementSpeed * 2.0f;
          float rotSpeed = deltaTime * RotationSpeed * 50.0f;

          // Move
          if (std::fabs(axisLeft.y) > deadZone)
          {
            float pos = (fabsf(axisLeft.y) - deadZone) / range;
            Position -= camFront * pos * ((axisLeft.y < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
            retVal = true;
          }
          if (std::fabs(axisLeft.x) > deadZone)
          {
            float pos = (fabsf(axisLeft.x) - deadZone) / range;
            Position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * pos * ((axisLeft.x < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
            retVal = true;
          }

          // Rotate
          if (std::fabs(axisRight.x) > deadZone)
          {
            float pos = (fabsf(axisRight.x) - deadZone) / range;
            Rotation.y += pos * ((axisRight.x < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
            retVal = true;
          }
          if (std::fabs(axisRight.y) > deadZone)
          {
            float pos = (fabsf(axisRight.y) - deadZone) / range;
            Rotation.x -= pos * ((axisRight.y < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
            retVal = true;
          }
        }
        else
        {
          // todo: move code from example base class for look-at
        }

        if (retVal)
        {
          UpdateViewMatrix();
        }

        return retVal;
      }
    };
  }
}


#endif
