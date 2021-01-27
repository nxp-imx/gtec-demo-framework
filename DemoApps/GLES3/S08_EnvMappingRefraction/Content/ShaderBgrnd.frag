#version 300 es
precision highp float;

in vec3 texCoord0;
in vec3 texCoord1;    // view vector (eye - pixel position)
uniform samplerCube samplerCb;
out vec4 out_color;

void main(void)
{
  out_color = texture(samplerCb, texCoord0);
}
