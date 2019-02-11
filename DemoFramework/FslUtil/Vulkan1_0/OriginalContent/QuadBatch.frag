#version 450 core

layout (set = 1, binding = 0) uniform sampler2D Texture;

layout (location = 0) in vec4 inFragColor;
layout (location = 1) in vec2 inFragTextureCoord;

layout (location = 0) out vec4 outFragColor;

void main()
{
  outFragColor = texture(Texture, inFragTextureCoord) * inFragColor;
  //outFragColor = inFragColor;
}
