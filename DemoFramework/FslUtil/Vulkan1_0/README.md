# FslUtil.Vulkan1_0

A higher level RAII based helper class library for Vulkan.
All operations are fully error checked and throw exceptions on errors.

## Source sections

### Batch

Very basic quad batch draw functionality. 
This is later used to implement the NativeBatch service for Vulkan.

### Draft

Some functionality that is still in development and that will change.

### Extend

Takes RapidVulkan low level classes and extends them slightly with commonly used functionality and variables.
The resulting classes are still very low level in functionality.

### SafeType

Classes that allow you to safely store a few of the Vulkan types.

### Util

Common utility functions grouped by functionality.
These are often used to implement more high level functionality in the VU classes.