#version 310 es
#extension GL_EXT_tessellation_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldView;
uniform mat4 WorldViewProjection;
uniform mat3 NormalMatrix;

// Camera space
uniform vec3 LightDirection;
uniform float DisplacementFactor;
uniform float DisplacementMod;

uniform sampler2D TextureDisplacement;

layout(triangles, fractional_odd_spacing, ccw) in;

// Defining gl_PerVertex is not really necessary according to the standard (and maybe even wrong)
// But it makes it work under most emulators and no target its been tested on complained so far.
in gl_PerVertex
{
  vec4 gl_Position;
}
gl_in[];


in block
{
  vec3 Normal;
  vec3 Tangent;
  vec2 TexCoord;
}
In[];


out gl_PerVertex
{
  vec4 gl_Position;
};


out block
{
  vec3 LightVec;
  vec3 HalfVec;
  vec2 TexCoord;
}
Out;


vec2 interpolate(vec2 a, vec2 b, vec2 c)
{
  vec2 p0 = vec2(gl_TessCoord.x) * a;
  vec2 p1 = vec2(gl_TessCoord.y) * b;
  vec2 p2 = vec2(gl_TessCoord.z) * c;
  return p0 + p1 + p2;
}


vec3 interpolate(vec3 a, vec3 b, vec3 c)
{
  vec3 p0 = vec3(gl_TessCoord.x) * a;
  vec3 p1 = vec3(gl_TessCoord.y) * b;
  vec3 p2 = vec3(gl_TessCoord.z) * c;
  return p0 + p1 + p2;
}

vec4 interpolate(vec4 a, vec4 b, vec4 c)
{
  vec4 p0 = vec4(gl_TessCoord.x) * a;
  vec4 p1 = vec4(gl_TessCoord.y) * b;
  vec4 p2 = vec4(gl_TessCoord.z) * c;
  return p0 + p1 + p2;
}

void main()
{
  vec4 tePosition = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
  vec2 texCoord = interpolate(In[0].TexCoord, In[1].TexCoord, In[2].TexCoord);
  vec3 displacementNormal = normalize(interpolate(In[0].Normal, In[1].Normal, In[2].Normal));
  vec3 tangent = normalize(interpolate(In[0].Tangent, In[1].Tangent, In[2].Tangent));

  float displace = ((texture(TextureDisplacement, texCoord).x - 0.5) * 2.0) + DisplacementMod;
  tePosition += vec4(displacementNormal.xyz * (displace * DisplacementFactor), 0);

  // Build a 'matrix' to convert from 'Eye Space' to 'Tangent Space'
  vec3 n = normalize(NormalMatrix * displacementNormal);
  vec3 t = normalize(NormalMatrix * tangent);
  vec3 b = cross(n, t);    // binormal

  // Transform light from eye to tangent space
  vec3 lightDir;
  lightDir.x = dot(LightDirection, t);
  lightDir.y = dot(LightDirection, b);
  lightDir.z = dot(LightDirection, n);
  Out.LightVec = normalize(lightDir);

  // Transform halfVec from eye to tangent space
  vec3 vertexPosition = normalize(-(WorldView * tePosition).xyz);
  vec3 halfVector = normalize(vertexPosition + LightDirection);
  vec3 transformedHalfVec;
  transformedHalfVec.x = dot(halfVector, t);
  transformedHalfVec.y = dot(halfVector, b);
  transformedHalfVec.z = dot(halfVector, n);
  // We dont normalize as t,b,n are normal vectors
  Out.HalfVec = transformedHalfVec;

  Out.TexCoord = texCoord;
  gl_Position = WorldViewProjection * tePosition;
}
