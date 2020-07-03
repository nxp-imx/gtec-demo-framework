#ifndef SHARED_VULKANWILLEMSMESHDEMOAPPEXPERIMENTAL_VULKANWILLEMSMESHDEMOAPP_HPP
#define SHARED_VULKANWILLEMSMESHDEMOAPPEXPERIMENTAL_VULKANWILLEMSMESHDEMOAPP_HPP
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

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <vulkan/vulkan.h>


namespace Fsl
{
  namespace Willems
  {
    class VulkanWillemsMeshDemoApp : public VulkanWillemsDemoApp
    {
    public:
      explicit VulkanWillemsMeshDemoApp(const DemoAppConfig& demoAppConfig);
    };
  }
}

#endif
