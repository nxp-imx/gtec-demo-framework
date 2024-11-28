#version 310 es
precision mediump float;

// A single attribute for the vertex position in normalized device coordinates
layout(location = 0) in vec2 VertexPosition;

// Output to the fragment shader
layout(location = 0) out vec2 v_TexCoord;

void main()
{
  // Pass through the vertex position and map it to the full screen
  // Map NDC [-1, 1] -> Texture coordinates [0, 1]
  v_TexCoord = (VertexPosition + vec2(1.0)) * 0.5;

  // Output the final position (clip-space coordinates)
  gl_Position = vec4(VertexPosition, 0.0, 1.0);
}