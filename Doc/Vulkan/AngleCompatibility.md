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

Name                             | Status 2020                  | Status feb 2022
---------------------------------|------------------------------|-----------------
Bloom                            | Works                        | Works
Blur                             | Fails with GL_INVALID_VALUE  | Works
DeBayer                          | NA                           | NA
DirectMultiSamplingVideoYUV      | NA                           | NA
EightLayerBlend                  | Works                        | Works
FractalShader                    | Works                        | Works
LineBuilder101                   | Works                        | Works
ModelLoaderBasics                | Works                        | Entrypoint Cr_z_compressBound
ModelViewer                      | Works                        | Entrypoint Cr_z_compressBound
OpenCV101                        | Works                        | Works
S01_SimpleTriangle               | Works                        | Works
S02_ColoredTriangle              | Works                        | Works
S03_Transform                    | Works                        | Works
S04_Projection                   | Works                        | Works
S05_PrecompiledShader            | NA                           | NA
S06_Texturing                    | Works                        | Works
S07_EnvironmentMapping           | Works                        | Works
S08_EnvironmentMappingRefraction | Works                        | Works
S09_VIV_direct_texture           | NA                           | NA
SdfFonts                         | Did not exist                | Works
Stats                            | Works                        | Works
System.BasicRenderModelLoader    | Did not exist                | Entrypoint Cr_z_compressBound
System.DevBasicRender            | Did not exist                | Rendering error
System.DevNativeTexture2D        | Did not exist                | Works
System.DFGraphicsBasic2D         | Works                        | Works
System.DFNativeBatch2D           | Works                        | Works
System.InputEvents               | Works                        | Works
System.OnDemandRendering         | Did not exist                | Works
T3DStressTest                    | Works                        | Works
TextureCompression               | Works                        | Works
UI.Benchmark                     | Did not exist                | Rendering error (depth buffer issue)
UI.SimpleUI100                   | Works                        | Works
UI.SimpleUI101                   | Works                        | Works
UI.SmoothScroll                  | Did not exist                | Works
UI.ThemeBasicUI                  | Did not exist                | Works
VIVDirectTextureMultiSampling    | NA                           | NA

### OpenGL ES3

Name                             | Status 2020                                         | Status feb 2022
---------------------------------|-----------------------------------------------------|---------------------
AssimpDoubleTexture              | Mesh is corrupt when run with --Stats               | entrypoint Cr_z_compressBound
Bloom                            | Works                                               | Works
CameraDemo                       | Crashes when camera is enabled                      |
ColorspaceInfo                   | Works                                               | Works
D1_1_VBOs                        | Works                                               | Works
D1_2_VAOs                        | Render corruption with --Stats                      | Works
DFNativeBatchCamera              | Crashes when camera is enabled                      |
DirectMultiSamplingVideoYUV      | NA                                                  | NA
E1_1_VBOs                        | Works                                               | Works
E1_2_VAOs                        | Render corruption with --Stats                      | Works
E2_1_CopyBuffer                  | Works                                               | Works
E3_0_InstancingSimple            | Rendering issue with --stats (stats text not shown) | Rendering issue with --stats (stats text not shown)
E4_0_PRestart                    | Rendering issue with --stats                        | Works
E6_0_MultipleRenderTargets       | Works???                                            | Works???
E7_0_ParticleSystem              | Works                                               | Works
EquirectangularToCubemap         | Rendering issue with --stats                        | Works
FractalShader                    | Works                                               | Works
FurShellRendering                | Works                                               | Works
GammaCorrection                  | Crashes on startup                                  | Works
HDR01_BasicToneMapping           | Rendering issue                                     | Works
HDR02_FBBasicToneMapping         | Rendering issue                                     | Works
HDR03_SkyboxToneMapping          | Rendering issue                                     | Works
HDR04_HDRFramebuffer             | Rendering issue                                     | Works
LineBuilder101                   | Works                                               | Works
ModelLoaderBasics                | Works                                               | entrypoint Cr_z_compressBound
ModelViewer                      | Works                                               | entrypoint Cr_z_compressBound
MultipleViewportsFractalShader   | Works                                               | Works
ObjectSelection                  | Works                                               | Works
OpenCL101                        | Works                                               | Works
OpenCLGaussianFilter             | Works                                               | Works
OpenCV101                        | Works                                               | Works
OpenCVMatToNativeBatch           | Works                                               | Works
OpenCVMatToUI                    | Works                                               | Works
OpenVX101                        | Works                                               | Works
ParticleSystem                   | Works but GL_EXT_geometry_shader not available.     | Works but GL_EXT_geometry_shader not available.
RenderToTexture                  | Did not exist                                       | Works
S01_SimpleTriangle               | Works                                               | Works
S02_ColoredTriangle              | Works                                               | Works
S03_Transform                    | Works                                               | Works
S04_Projection                   | Works                                               | Works
S05_PrecompiledShader            | NA                                                  | NA
S06_Texturing                    | Works                                               | Works
S07_EnvironmentMapping           | Works                                               | Works
S08_EnvironmentMappingRefraction | Works                                               | Works
S09_VIV_direct_texture           | NA                                                  | NA
Scissor101                       | Works                                               | Works
SdfFonts                         | Did not exist                                       | Works
Skybox                           | Rendering issue with --Stats                        | Works
SpringBackground                 | Shader creation failure                             | Shader creation failure
SRGBFramebuffer                  | Assertion failure                                   | Internal error
System.BasicRenderModelLoader    | Did not exist                                       | entrypoint Cr_z_compressBound
System.DevBasicRender            | Did not exist                                       | Rendering error
System.DevNativeTexture2D        | Did not exist                                       | Works
System.DFGraphicsBasic2D         | Works                                               | Works
System.DFNativeBatch2D           | Works                                               | Works
System.OnDemandRendering         | Did not exist                                       | Works
T3DStressTest                    | Shader compilation issue                            | Works
Tessellation101                  | GL_EXT_geometry_shader not available                | GL_EXT_geometry_shader not available
TessellationSample               | GL_EXT_geometry_shader not available                | entrypoint Cr_z_compressBound
TextureCompression               | Works                                               | Works
UI.Benchmark                     | Did not exist                                       | Rendering error (depth buffer issue)
UI.DpiScale                      | Did not exist                                       | Works
UI.PixelPerfect                  | Did not exist                                       | Works
UI.SimpleUI101                   | Works                                               | Works
UI.SimpleUI100                   | Works                                               | Works
UI.SmoothScroll                  | Did not exist                                       | Works
UI.ThemeBasicUI                  | Did not exist                                       | Works
VerletIntegration101             | Works                                               | Works
