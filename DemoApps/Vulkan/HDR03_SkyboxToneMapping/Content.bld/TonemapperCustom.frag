#version 310 es
/****************************************************************************************************************************************************
 *This is free and unencumbered software released into the public domain.
 *
 *Anyone is free to copy, modify, publish, use, compile, sell, or
 *distribute this software, either in source code form or as a compiled
 *binary, for any purpose, commercial or non-commercial, and by any
 *means.
 *
 *In jurisdictions that recognize copyright laws, the author or authors
 *of this software dedicate any and all copyright interest in the
 *software to the public domain. We make this dedication for the benefit
 *of the public at large and to the detriment of our heirs and
 *successors. We intend this dedication to be an overt act of
 *relinquishment in perpetuity of all present and future rights to this
 *software under copyright law.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *OTHER DEALINGS IN THE SOFTWARE.
 *
 *For more information, please refer to <http://unlicense.org>
 ****************************************************************************************************************************************************/

// ShaderCode based on the OpenGL Tutorial https://learnopengl.com/Advanced-Lighting/HDR

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(std140, binding = 0) uniform UBO
{
  // Tonemapping
  uniform float Exposure;
}
g_ubo;

mediump layout(input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput g_hdrColorAttachment;

layout(location = 0) out vec4 FragColor;

void main()
{
  vec3 hdrColor = subpassLoad(g_hdrColorAttachment).rgb;
  // Exposure
  hdrColor *= g_ubo.Exposure;


  hdrColor = vec3(1.0) - exp(-hdrColor);

  // Gamma correction
  hdrColor = pow(hdrColor, vec3(1.0 / 2.2));
  FragColor = vec4(hdrColor, 1.0);
}