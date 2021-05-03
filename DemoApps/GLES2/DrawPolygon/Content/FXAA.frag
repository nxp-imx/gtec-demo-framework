#ifdef GL_FRAGMENT_PRECISION_HIGH
  precision highp float;
#else
  precision mediump float;
#endif


#define LUMA_THRESHOLD 	0.2
#define MUL_REDUCE 		(1.0/8.0)
#define MIN_REDUCE 		(1.0/128.0)
#define MAX_SPAN		8.0

// Binding 0
uniform sampler2D u_texSampler;
varying vec2 v_texCoords;
uniform float u_grd_c_tw;
uniform float u_grd_c_th;

// see FXAA
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
// http://iryoku.com/aacourse/downloads/09-FXAA-3.11-in-15-Slides.pdf
// http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA

void main()
{
    vec4 rgba = texture2D(u_texSampler, v_texCoords).rgba;
    vec2 u_texelStep = vec2(1.0/u_grd_c_tw,1.0/u_grd_c_th);
    
    if (rgba.a == 0.0)
    {
        discard;
    }
    
    vec3 rgbM = rgba.rgb;
    
    // Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates.
    vec3 rgbNW = texture2D(u_texSampler, v_texCoords + vec2(-1, 1)*u_texelStep).rgb;
    vec3 rgbNE = texture2D(u_texSampler, v_texCoords + vec2(1, 1)*u_texelStep).rgb;
    vec3 rgbSW = texture2D(u_texSampler, v_texCoords + vec2(-1, -1)*u_texelStep).rgb;
    vec3 rgbSE = texture2D(u_texSampler, v_texCoords + vec2(1, -1)*u_texelStep).rgb;
    
    // see http://en.wikipedia.org/wiki/Grayscale
    const vec3 toLuma = vec3(0.299, 0.587, 0.114);
    
    // Convert from RGB to luma.
    float lumaNW = dot(rgbNW, toLuma);
    float lumaNE = dot(rgbNE, toLuma);
    float lumaSW = dot(rgbSW, toLuma);
    float lumaSE = dot(rgbSE, toLuma);
    float lumaM = dot(rgbM, toLuma);
    
    // Gather minimum and maximum luma.
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    
    // If contrast is lower than a maximum threshold ...
    if (lumaMax - lumaMin < lumaMax * LUMA_THRESHOLD)
    {
        // ... do no AA and return.
        gl_FragColor = vec4(rgbM, 1.0);
        
        return;
    }
    
    // Sampling is done along the gradient.
    vec2 samplingDirection;
    samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.
    float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * MUL_REDUCE, MIN_REDUCE);
    
    // Factor for norming the sampling direction plus adding the brightness influence.
    float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size.
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-MAX_SPAN, -MAX_SPAN), vec2(MAX_SPAN, MAX_SPAN)) * u_texelStep;
    
    // Inner samples on the tab.
    vec3 rgbSampleNeg = texture2D(u_texSampler, v_texCoords + samplingDirection * (1.0/3.0 - 0.5)).rgb;
    vec3 rgbSamplePos = texture2D(u_texSampler, v_texCoords + samplingDirection * (2.0/3.0 - 0.5)).rgb;
    
    vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;
    
    // Outer samples on the tab.
    vec3 rgbSampleNegOuter = texture2D(u_texSampler, v_texCoords + samplingDirection * (0.0/3.0 - 0.5)).rgb;
    vec3 rgbSamplePosOuter = texture2D(u_texSampler, v_texCoords + samplingDirection * (3.0/3.0 - 0.5)).rgb;
    
    vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;
    
    // Calculate luma for checking against the minimum and maximum value.
    float lumaFourTab = dot(rgbFourTab, toLuma);
    
    // Are outer samples of the tab beyond the edge ...
    if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
    {
        // ... yes, so use only two samples.
        gl_FragColor = vec4(rgbTwoTab, 1.0);
    }
    else
    {
        // ... no, so use four samples. 
        gl_FragColor = vec4(rgbFourTab, 1.0);
    }
}
