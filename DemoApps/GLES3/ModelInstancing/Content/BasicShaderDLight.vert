#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 View;
uniform mat4 Projection;

in vec3 VertexPosition;
in vec4 VertexColor;
in vec3 VertexNormal;
in vec2 VertexTexCoord;
in mat4 InstanceModelMatrix;
// in vec4 InstanceModelMatrix;

out vec4 v_Color;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
  mat4 worldView = View * InstanceModelMatrix;
  //  mat4 worldView = View;
  mat4 worldViewProj = Projection * worldView;
  mat3 normalMatrix = mat3(transpose(inverse(worldView)));

  //  v_Color = VertexColor* InstanceModelMatrix;
  v_Color = VertexColor;
  v_Normal = normalMatrix * VertexNormal;
  v_TexCoord = VertexTexCoord;
  gl_Position = worldViewProj * vec4(VertexPosition, 1.0);
}
