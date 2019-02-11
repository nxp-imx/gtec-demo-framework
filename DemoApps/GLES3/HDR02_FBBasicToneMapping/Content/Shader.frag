#version 300 es
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

in vec3 v_FragPosition;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec3 g_lightPositions[4];
uniform vec3 g_lightColors[4];

uniform sampler2D g_diffuseTexture;

out vec4 FragColor;

void main()
{
  vec3 color = texture(g_diffuseTexture, v_TexCoord).rgb;
  vec3 normal = normalize(v_Normal);
  // ambient
  vec3 ambient = 0.0 * color;
  // lighting
  vec3 lighting = vec3(0.0);
  for (int i = 0; i < 4; i++)
  {
    // diffuse
    vec3 lightDir = normalize(g_lightPositions[i] - v_FragPosition);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = g_lightColors[i] * diff * color;
    vec3 result = diffuse;
    // attenuation (use quadratic as we have gamma correction)
    float distance = length(v_FragPosition - g_lightPositions[i]);
    result *= 1.0 / (distance * distance);
    lighting += result;
  }

  FragColor = vec4(ambient + lighting, 1.0);
}