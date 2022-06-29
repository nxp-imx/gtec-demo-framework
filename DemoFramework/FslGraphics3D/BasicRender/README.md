# FslGraphics3D.BasicRender

A very basic 3D rendering abstraction.

**FIX:** Work in progress.

## Basic shaders

The predefined basic shaders are defined by the BasicRender and useful for 2D UI rendering. The predefined shaders are  a basic vertex shader and two fragment shaders (one for normal rendering one one for sdf).  It is possible to create additional basic shaders by creating a specific implementation for the actual native rendering API by following the rules below.

```c++
constexpr VertexAttributeDescriptionArray<3> vertexAttrDesc = {
  VertexAttributeDescription(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
  VertexAttributeDescription(1, VertexElementFormat::Vector4, VertexElementUsage::Color, 0),
  VertexAttributeDescription(2, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0)};

ReadOnlySpan<uint8_t> vertShaderCodeSpan = ReadOnlySpanUtil::AsSpan(binaryVertShaderCode);
ReadOnlySpan<uint8_t> fragShaderCodeSpan = ReadOnlySpanUtil::AsSpan(binaryFragShaderCode);

BasicShaderCreateInfo vertShaderCreateInfo(BasicShaderStageFlag::Vertex, vertShaderCodeSpan, vertexAttrDesc.AsReadOnlySpan());
BasicShader vertShader = CreateShader(shaderCreateInfo);

BasicShaderCreateInfo fragShaderCreateInfo(fragShaderCodeSpan);
BasicShader fragShader = CreateShader(fragShaderCreateInfo);
```

These can then be used to create a new material that uses the basic shaders.

```c++
BasicCustomMaterialCreateInfo materialCreateInfo(BlendState::Opaque, VertexPositionColorTexture::AsVertexDeclarationSpan(), verteShader, fragShader);
BasicMaterial material = CreateMaterial(materialCreateInfo, texture);
```

### Api shader requirements

Each subsection defines the basic shader requirements by each underlaying API.

#### OpenGL ES 2

The vertex shader is expected to receive a ModelViewProj matrix via a uniform and it's expected that the vertex layout is defined like this

```glsl
uniform mat4 MatModelViewProj;

attribute vec4 VertexPosition;
attribute vec4 VertexColor;
attribute vec2 VertexTextureCoord;
```

The normal fragment shader is expected to receive a texture defined like this

```glsl
uniform sampler2D Texture;
```

The sdf fragment shader is expected to receive a texture and smoothing like this:

```glsl
uniform sampler2D Texture;
uniform float Smoothing;
```

#### OpenGL ES 3

The vertex shader is expected to receive a ModelViewProj matrix via a uniform and it's expected that the vertex layout is defined like this

```glsl
uniform mat4 MatModelViewProj;

layout(location = 0) in vec4 inVertexPosition;
layout(location = 1) in vec4 inVertexColor;
layout(location = 2) in vec2 inVertexTextureCoord;
```

The normal fragment shader is expected to receive a texture defined like this

```glsl
uniform sampler2D Texture;
```

The sdf fragment shader is expected to receive a texture and smoothing like this:

```glsl
uniform sampler2D Texture;
uniform float Smoothing;
```

##### Vulkan

The vertex shader is expected to receive a ModelViewProj matrix via a push constant and it's expected that the vertex layout is defined like this

```glsl
layout(push_constant) uniform LayerBlock
{
  mat4 ModelViewProj;
} g_pushConstant;

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec4 inVertexColor;
layout(location = 2) in vec2 inVertexTextureCoord;
```

The normal fragment shader is expected to receive a texture defined like this

```glsl
layout(set = 0, binding = 0) uniform sampler2D Texture;
```

The sdf fragment shader is expected to receive a texture and a push constant defining the smoothing like this:

```glsl
layout(set = 0, binding = 0) uniform sampler2D Texture;

layout(push_constant) uniform LayerBlock
{
  // Offset is because it comes after the ModelViewProj matrix used by the vertex shader
  layout(offset = 4 * 4 * 4) uniform float Smoothing;
} g_pushConstant;

```

## Basic custom shaders

More complex shaders can also be created using 'custom shaders'.


```c++
ReadOnlySpan<uint8_t> vertShaderCodeSpan = ReadOnlySpanUtil::AsSpan(binaryVertShaderCode);
ReadOnlySpan<uint8_t> fragShaderCodeSpan = ReadOnlySpanUtil::AsSpan(binaryFragShaderCode);

BasicCustomShaderCreateInfo vertShaderCreateInfo(BasicShaderStageFlag::Vertex, vertShaderCodeSpan);
BasicShader vertShader = CreateShader(shaderCreateInfo);

BasicShaderCreateInfo fragShaderCreateInfo(BasicShaderStageFlag::Fragment, fragShaderCodeSpan);
BasicShader fragShader = CreateShader(fragShaderCreateInfo);
```

These can then be used to create a new material that uses the basic shaders.

```c++
BasicCustomMaterialCreateInfo materialCreateInfo(BlendState::Opaque, VertexPositionColorTexture::AsVertexDeclarationSpan(), verteShader, fragShader);
BasicMaterial material = CreateMaterial(materialCreateInfo, texture);
```


Work in progress

