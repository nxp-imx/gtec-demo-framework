/*
* Vulkan Example base class
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in VulkanExampleBase to make it easier
// to port samples.

#include <Shared/VulkanWillemsDemoAppExperimental/SwapchainBuffers.hpp>
#include <RapidVulkan/Check.hpp>
#include <utility>

using namespace RapidVulkan;

namespace Fsl
{
  namespace Willems
  {
    //! @brief Move assignment operator
    SwapchainBuffers& SwapchainBuffers::operator = (SwapchainBuffers&& other)
    {
      if (this != &other)
      {
        // Claim ownership here
        Image = other.Image;
        View = std::move(other.View);

        // Remove the data from other
        other.Image = VK_NULL_HANDLE;
      }
      return *this;
    }


    SwapchainBuffers::SwapchainBuffers(SwapchainBuffers&& other)
      : Image(other.Image)
      , View(std::move(other.View))
    {
      // Remove the data from other
      other.Image = VK_NULL_HANDLE;
    }

    SwapchainBuffers::SwapchainBuffers()
      : Image()
      , View()
    {
    }
  }
}
