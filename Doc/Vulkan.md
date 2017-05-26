Vulkan 
======

The support in the demo framework is still in development so this release is marked as early access.
Early access means everything is subject to changes and still in development. 

Beware:
- The Vulkan demo host has not been finalized, so creating projects using it is not recommended.
  Instead create a project using the WindowVulkan or WillemsVulkan temporary project types.
- Ubuntu X11 is in theory supported but has not been tested yet.
- Yocto, Android and Qnx support is disabled.
  
The WindowVulkan project type is basically a freeform project that utilize the vulkan native window code 
to create the window, this is the recommended way to create new Vulkan samples at the moment.

The WillemsVulkan project type exist to make it easier to port a existing sample that uses Sascha Willems
framework. (https://github.com/SaschaWillems/Vulkan)
