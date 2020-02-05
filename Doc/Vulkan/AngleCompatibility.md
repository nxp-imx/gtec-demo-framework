# Angle

## Summary

Provides a OpenGL ES implementation running on top of Vulkan.
Main benefit is that its a standard implementation used on many devices that lives in user-space so it can be easily updated. One of the main issues with Android for developers is its many OpenGL ES implementations that each has its own "quirks". Another benefit is that it can allow Android vendors to only supply a high quality Vulkan driver instead of both a Vulkan and OpenGL ES driver. Beware it is not expected to be able to compete with a native driver in performance.

## Presentations

* [ANGLE: Portable OpenGL ES](https://youtu.be/1fU4w2ZGxH4?t=10781) from July 31, SIGGRAPH 2019.
* [ANGLE: OpenGL on Vulkan](https://youtu.be/QrIKdjmpmaA) from 2018.

## State

API          | Vulkan      | Conformance
-------------|-------------|-------------
OpenGL ES 2.0| complete    | 99%
OpenGL ES 3.0| in progress | over 90%
OpenGL ES 3.1| in progress | over 80%
OpenGL ES 3.2| planned     | coming soon
EGL 1.4      | complete    | -

ANGLE v1.0.772 was certified compliant by passing the ES 2.0.3 conformance tests in October 2011.

**They intend to have 3.2 Conformance Q3 2020.** This is not a committed release date.

Fully supported on Windows, Linux and Android

From https://github.com/google/angle


## DemoFramework Compatibility

### OpenGL ES2

Name                             | Status
---------------------------------|-----------------
Bloom                            | Works
Blur                             | Fails with GL_INVALID_VALUE
DeBayer                          | NA
DFGraphicsBasic2D                | Works
DFNativeBatch2D                  | Works
DFSimpleUI100                    | Works
DFSimpleUI101                    | Works
DirectMultiSamplingVideoYUV      | NA
EightLayerBlend                  | Works
FractalShader                    | Works
InputEvents                      | Works
LineBuilder101                   | Works
ModelLoaderBasics                | Works
ModelViewer                      | Works
OpenCV101                        | Works
S01_SimpleTriangle               | Works
S02_ColoredTriangle              | Works
S03_Transform                    | Works
S04_Projection                   | Works
S05_PrecompiledShader            | NA
S06_Texturing                    | Works
S07_EnvironmentMapping           | Works
S08_EnvironmentMappingRefraction | Works
S09_VIV_direct_texture           | NA
Stats                            | Works
T3DStressTest                    | Works
TextureCompression               | Works
VIVDirectTextureMultiSampling    | NA

### OpenGL ES3

Name                             | Status
---------------------------------|-----------------
AssimpDoubleTexture              | Mesh is corrupt when run with --Stats
Bloom                            | Works
CameraDemo                       | Crashes when camera is enabled
ColorspaceInfo                   | Works
D1_1_VBOs                        | Works
D1_2_VAOs                        | Render corruption with --Stats
DFGraphicsBasic2D                | Works
DFNativeBatch2D                  | Works
DFNativeBatchCamera              | Crashes when camera is enabled
DFSimpleUI100                    | Works
DFSimpleUI101                    | Works
DirectMultiSamplingVideoYUV      | NA
E1_1_VBOs                        | Works
E1_2_VAOs                        | Render corruption with --Stats
E2_1_CopyBuffer                  | Works
E3_0_InstancingSimple            | Rendering issue with --stats (stats text not shown)
E4_0_PRestart                    | Rendering issue with --stats
E6_0_MultipleRenderTargets       | Works???
E7_0_ParticleSystem              | Works
EquirectangularToCubemap         | Rendering issue with --stats
FractalShader                    | Works
FurShellRendering                | Works
GammaCorrection                  | Crashes on startup
HDR01_BasicToneMapping           | Rendering issue
HDR02_FBBasicToneMapping         | Rendering issue
HDR03_SkyboxToneMapping          | Rendering issue
HDR04_HDRFramebuffer             | Rendering issue
LineBuilder101                   | Works
ModelLoaderBasics                | Works
ModelViewer                      | Works
MultipleViewportsFractalShader   | Works
ObjectSelection                  | Works
OpenCL101                        | Works
OpenCLGaussianFilter             | Works
OpenCV101                        | Works
OpenCVMatToNativeBatch           | Works
OpenCVMatToUI                    | Works
OpenVX101                        | Works
ParticleSystem                   | Works but GL_EXT_geometry_shader not available.
S01_SimpleTriangle               | Works
S02_ColoredTriangle              | Works
S03_Transform                    | Works
S04_Projection                   | Works
S05_PrecompiledShader            | NA
S06_Texturing                    | Works
S07_EnvironmentMapping           | Works
S08_EnvironmentMappingRefraction | Works
S09_VIV_direct_texture           | NA
Scissor101                       | Works
Skybox                           | Rendering issue with --Stats
SpringBackground                 | Shader creation failure
SRGBFramebuffer                  | Assertion failure
T3DStressTest                    | Shader compilation issue
Tessellation101                  | GL_EXT_geometry_shader not available
TessellationSample               | GL_EXT_geometry_shader not available
TextureCompression               | Works
VerletIntegration101             | Works
