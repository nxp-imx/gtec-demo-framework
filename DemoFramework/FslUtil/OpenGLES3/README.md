# FslUtil.OpenGLES3

A RAII based helper class library for OpenGL ES 3.
All operations are fully error checked and throw exceptions on errors.

# Table of contents
<!-- #AG_TOC_BEGIN# -->
* [Classes](#classes)
  * [GLBuffer](#glbuffer)
  * [GLBufferArray](#glbufferarray)
  * [GLIndexBuffer](#glindexbuffer)
  * [GLIndexBufferArray](#glindexbufferarray)
  * [GLVertexBuffer](#glvertexbuffer)
  * [GLVertexBufferArray](#glvertexbufferarray)
  * [GLVertexElement](#glvertexelement)
  * [GLFrameBuffer](#glframebuffer)
  * [GLRenderBuffer](#glrenderbuffer)
  * [GLProgram](#glprogram)
  * [GLShader](#glshader)
  * [GLTexture](#gltexture)
  * [GLTextureInfo](#gltextureinfo)
  * [GLTextureParameters](#gltextureparameters)
  * [GLRawBitmapUtil](#glrawbitmaputil)
  * [GLValues](#glvalues)
  * [GLUtil](#glutil)
  * [GLBatch2D](#glbatch2d)
  * [NativeBatch2D](#nativebatch2d)
<!-- #AG_TOC_END# -->

# Classes


## GLBuffer
A RAII encapsulation of a OpenGL ES buffer.


## GLBufferArray
A RAII encapsulation of a OpenGL ES buffer array


### GLBufferArrayEntry.
A entry in the GLBufferArray


## GLIndexBuffer
A RAII encapsulation of a OpenGL ES index buffer.
Has simple utility methods for filling the buffer from a source vector or pointer to the original data.

```C++
// Create from a vector
std::vector<uint8_t> indices = {0,1,2,3};
GLIndexBuffer indexBuffer(indices, GL_STATIC_DRAW);

// Create from a pointer and length
std::array<uint8_t, 4> indices = {0,1,2,3};
GLIndexBuffer indexBuffer(indices.data(), indices.size(), GL_STATIC_DRAW);
```

## GLIndexBufferArray
A RAII encapsulation of a OpenGL ES index buffers.
Has simple utility methods for filling the buffers from a source vector or pointer to the original data.

```C++
const std::size_t bufferCount = 4;
GLIndexBufferArray indexBuffers(bufferCount, GL_UNSIGNED_SHORT);

std::vector<uint8_t> indexVector0 = {0,1,2,3};
std::vector<uint8_t> indexVector1 = {0,1,2};
std::vector<uint8_t> indexVector2 = {0,2,3};
std::vector<uint8_t> indexVector3 = {1,2,3,4,5};

indexBuffers.Reset(0, indexVector0, GL_STATIC_DRAW);
indexBuffers.Reset(1, indexVector1, GL_STATIC_DRAW);
indexBuffers.Reset(2, indexVector2, GL_STATIC_DRAW);
indexBuffers.Reset(3, indexVector3, GL_STATIC_DRAW);
```


## GLVertexBuffer
A RAII encapsulation of a OpenGL ES vertex buffer.
Has simple utility methods for filling the buffer using API independent vertex types.

```C++
std::vector<VertexPosition> vertices = {VertexPosition(-10,-10,0), VertexPosition(0, 10, 0), VertexPosition(10, -10, 0)};
GLVertexBuffer vertexBuffer(vertices, GL_STATIC_DRAW);
```


## GLVertexBufferArray
A RAII encapsulation of a OpenGL ES vertex buffer array.
Has simple utility methods for filling the buffers using API independent vertex types.

```C++
const std::size_t bufferCount = 4;
GLVertexBufferArray vertexBuffers(bufferCount, VertexPosition::GetVertexDeclaration());

std::vector<VertexPosition> vertexVector0 = {VertexPosition(-10,-10,0), VertexPosition(0, 10, 0), VertexPosition(10, -10, 0)};
std::vector<VertexPosition> vertexVector1 = {VertexPosition(-10,-10,0), VertexPosition(0, 10, 0), VertexPosition(10, -10, 0), VertexPosition(50, -10, 0)};;
std::vector<VertexPosition> vertexVector2 = {VertexPosition(-10,-10,0), VertexPosition(0, 22, 0), VertexPosition(20, -10, 0)};
std::vector<VertexPosition> vertexVector3 = {VertexPosition(-50,-20,0), VertexPosition(0, 10, 0), VertexPosition(10, -10, 0), VertexPosition(50, -50, 0)};;

vertexBuffers.Reset(0, vertexVector0, GL_STATIC_DRAW);
vertexBuffers.Reset(1, vertexVector1, GL_STATIC_DRAW);
vertexBuffers.Reset(2, vertexVector2, GL_STATIC_DRAW);
vertexBuffers.Reset(3, vertexVector3, GL_STATIC_DRAW);
```


## GLVertexElement
A Open GL ES native representation of the elements from a VertexDeclaration.
Used by the GLVertexBuffers.


## GLFrameBuffer
A RAII encapsulation of a framebuffer, this includes the color, depth and possible scencil buffers.

```C++
GLFrameBuffer frameBuffer(Point2(16, 16), GLTextureParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE), GL_RGB565)
```


## GLRenderBuffer
A RAII encapsulation of a OpenGL ES render buffer.

```C++
GLRenderBuffer colorRenderBuffer(Point2(16,16), GL_RGB565);
```



## GLProgram
A RAII encapsulation of a OpenGL ES program.
Has utility methods for compiling text based vertex and fragment shaders in to into a program and any errors are nicely logged.

```C++
GLProgram program(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));
```


## GLShader
A RAII encapsulation of a OpenGL ES shader.
Has utility methods for compiling the text based shader into a proper shader and any errors are nicely logged.

```C++
GLShader vertexShader(GL_VERTEX_SHADER, content->ReadAllText("Shader.vert"));
```


## GLTexture
A RAII encapsulation of a OpenGL ES texture.
Has utility methods for creating the texture based on the platform independent Bitmap and Texture classes.

```C++
// Create a GLTexture from a Bitmap
Bitmap bitmap = content->ReadBitmap("Texture.png", PixelFormat::R8G8B8_UNORM);
GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
GLTexture tex(bitmap, params);

// Create a GLTexture from a Texture
Texture bitmap = content->ReadTexture("Texture.ktx", PixelFormat::R8G8B8_UNORM);
GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
GLTexture tex(bitmap, params);
```


## GLTextureInfo
Contains key information about a texture, this is used by the GLBatch2D/NativeBatch2D classes.

```C++
GLTextureInfo(textureHandle, textureSize)
```

## GLTextureParameters
Encapsulate common texture parameters.


```C++
GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
```


## GLRawBitmapUtil
Given a pixelFormat or RawBitmap get the OpenGL ES data representation suitable for use with glTexImage2D.


## GLValues
Contains more explicit definitions for some Open GL ES magic values.

Name | Usage
--- | ---
INVALID_HANDLE | The invalid handle is used for buffers, programs, shaders, textures
INVALID_LOCATION | Invalid handle like returned by glGetUniformLocation and glGetAttribLocation


## GLUtil
A collection of commen utility functionality.
- A simpler way to check if a extension is present.
- Capture a screenshot into a Bitmap.
- Convert the api independent PrimitiveType to its OpenGL ES 3 native type.


## GLBatch2D
A simple specialization of the GenericBatch2D for OpenGL ES 3.
This is basically a simple immediate mode quad rendering utility.


## NativeBatch2D

Extends the GLBatch2D with extra OpenGL ES 3 specific rendering methods. It also implements the INativeBatch2D service interface that apps can use for 'API' independent rendering.

```C++
// Draw using a OpenGL ES 3 native texture handle
GLTexture texture; // This needs to contain a valid texture :)
nativeBatch->Draw(GLTextureInfo(texture.Get(), texture.GetSize()), Rectangle(0, 0, 256, 256), Color::White());

// Draw using the GLTexture object
GLTexture texture; // This needs to contain a valid texture :)
nativeBatch->Draw(texture, Vector2(0, 0), Color::White());
nativeBatch->Draw(texture, Rectangle(0, 0, 120, 100), Color::White());

// Draw using a API independent texture
Texture2D texture2D; // This needs to contain a valid texture :)
nativeBatch->Draw(texture2D, Vector2(0, 0), Color::White());
nativeBatch->Draw(texture2D, Rectangle(0, 0, 120, 100), Color::White());

```
