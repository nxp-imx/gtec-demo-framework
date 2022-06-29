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

#include <Shared/VulkanWillemsMeshDemoAppExperimental/VulkanMeshLoaderAssimp.hpp>
#include <Shared/VulkanWillemsMeshDemoAppExperimental/VulkanWillemsMeshDemoApp.hpp>

namespace Fsl::Willems
{
  namespace
  {
    std::unique_ptr<VulkanMeshLoader> Allocate(const std::shared_ptr<IContentManager>& contentManager)
    {
      return std::unique_ptr<VulkanMeshLoader>(new VulkanMeshLoaderAssimp(contentManager));
    }
  }


  VulkanWillemsMeshDemoApp::VulkanWillemsMeshDemoApp(const DemoAppConfig& demoAppConfig)
    : VulkanWillemsDemoApp(demoAppConfig, Allocate)
  {
  }
}
