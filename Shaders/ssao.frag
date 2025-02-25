#version 460 core
out float FragColor;

in vec2 texCoords;

uniform sampler2D depth;
uniform sampler2D normalColour;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
uniform int kernelSize = 64;
uniform float radius = 0.5;
uniform float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform vec2 noiseScale = vec2(1.0/4.0, 1.0/4.0); 

uniform mat4 projection;
uniform mat4 invP;
uniform mat4 invV;
uniform mat4 view;

void main()
{
    // get input for SSAO algorithm
    float depthValue = texture(depth, texCoords).r;
    vec4 NDC = vec4(texCoords * 2.0 - 1.0, depthValue * 2.0 - 1.0, 1.0);
    vec4 clipPos = invP * NDC;
    vec3 screenPos = clipPos.xyz / clipPos.w;
    vec3 fragPos = (invV * vec4(screenPos, 1.0)).xyz;

    vec3 randomVec = normalize(texture(texNoise, texCoords * noiseScale).xyz);

    vec3 normal = normalize(texture(normalColour, texCoords).rgb);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        vec4 offset = view * vec4(samplePos, 1.0);

        samplePos = offset.xyz;
        // project sample position (to sample texture) (to get position on screen/texture)
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(depth, offset.xy).r; // get depth value of kernel sample
        vec4 sampleNDC = vec4(offset.xy * 2.0 - 1.0, sampleDepth * 2.0 - 1.0, 1.0);
        vec4 sampleClipPos = invP * sampleNDC;
        vec3 sampleScreenPos = sampleClipPos.xyz /sampleClipPos.w;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(screenPos.z - sampleScreenPos.z));
        occlusion += (sampleScreenPos.z >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
 }