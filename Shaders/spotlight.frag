#version 460 core


in vec3 lightColour;

out vec4 finalColour;

in vec3 fragPos;
in vec3 fragmentNormal;

uniform float lerpAmount;
uniform float linear;
uniform float quad;
uniform float cutOff;
uniform float outerCutOff;
uniform vec3 direction;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform vec3 cameraDelta;
uniform mat4 invVP;
uniform mat4 invP;
uniform mat4 invV;
uniform mat4 view;
uniform mat4 proj;
uniform vec2 invViewPort;
uniform mat4 lightMat;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D pbr;
uniform sampler2D depth;
uniform sampler2D lightLerp;
uniform sampler2D shadowMap;

vec3 trueNormal;
vec3 trueAlbedo;
float metallic;
float roughness;

vec4 fragInLight;
vec3 viewDirection;
vec3 F0;
vec3 worldPos;
vec3 posToLight;
vec2 texCoords;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 Radiance(
    vec3 lightDir, float distanceToLight, 
    vec3 normal, float constant, 
    float linear, float quadratic, vec3 diffuse
);
vec3 CalcDirectionalLight(vec3 lightDirection, vec3 normal);
vec3 specularIBL(vec3 trueNormal);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight);
float ShadowCalculation();
float ScreenSpaceShadows();
float InterleavedGradientNoise(vec2 position_screen);

float sssMaxRayDistance = 50.0f;
float sssThickness = 0.05f;
int sssSteps = 8;

#define PI 3.1415926535

const float MAX_REFLECTION_LOD = 4.0;


void main()
{

    texCoords = gl_FragCoord.xy;
    texCoords *= invViewPort;
	vec4 albedo4 = texture(albedo, texCoords);
	vec4 normal4 = texture(normal, texCoords);
	vec4 pbr4 = texture(pbr, texCoords);

	trueNormal = normal4.xyz;
	trueAlbedo = albedo4.xyz;
	metallic = pbr4.r;
	roughness = pbr4.g;

	float depthValue = texture(depth, texCoords).r;

    vec4 NDC = vec4(texCoords * 2.0 - 1.0, depthValue * 2.0 - 1.0, 1.0);
    vec4 clipPos = invP * NDC;
    vec3 screenPosition = clipPos.xyz / clipPos.w;
    worldPos = (invV * vec4(screenPosition, 1.0)).xyz;

    fragInLight = lightMat * vec4((invV * vec4(screenPosition, 1.0)));
	posToLight = lightPos - worldPos;

	viewDirection = -normalize(worldPos - (camPos));
	F0 = vec3(0.04); 
	F0 = mix(F0, trueAlbedo, metallic);

	vec3 Lo = clamp(Radiance(
	normalize(posToLight),
	length(posToLight) / 100.0,
	trueNormal, 1, linear, quad, lightColour
	), 0, 1);
    Lo *= texture(lightLerp, lerpAmount.rr).rgb;
    Lo *= ShadowCalculation();
	finalColour = vec4(Lo, 1.0);
	return;
}

vec3 Radiance(
    vec3 lightDir, float distanceToLight, 
    vec3 normal, float constant, 
    float linear, float quadratic, vec3 diffuse
)
{
    // calculate per-light radiance
    vec3 H = normalize(viewDirection + lightDir);
    float attenuation = CalcAttenuation(constant, linear, quadratic, distanceToLight);
    float theta = dot(lightDir, -normalize(direction));
	float epsilon = cutOff - outerCutOff;
	float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
    vec3 radiance     = diffuse * attenuation * intensity;        
        
    // cook-torrance brdf
    float NDF = DistributionGGX(normal, H, roughness);        
    float G   = GeometrySmith(normal, viewDirection, lightDir, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, viewDirection), 0.0), F0);       
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
            
    // add to outgoing radiance Lo
    float NdotL = max(dot(normal, lightDir), 0.0);                
    return (kD * trueAlbedo / PI + specular) * radiance * NdotL; 
}

float CalcAttenuation(float constant, float linear, float quadratic, float distanceToLight) {
    return 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float ShadowCalculation()
{
    // perform perspective divide
    vec3 projCoords = fragInLight.xyz / fragInLight.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 n = normalize(trueNormal);
    vec3 lightDir = normalize(posToLight);
    float bias = 0.00005;

    // check whether current frag pos is in shadow
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0) {
        shadow = 1.0;
    }

    return 1.0 - shadow;
}

float ScreenSpaceShadows()
{
    // Compute ray position and direction (in view-space)

    vec3 ray_pos = worldPos;
    vec3 ray_dir = normalize(posToLight).xyz;
    // Compute ray step
    vec3 ray_step = ray_dir * sssMaxRayDistance / sssSteps;
	
    // Ray march towards the light
    float occlusion = 0.0;
    vec2 ray_uv   = vec2(0.0);
    float offset = InterleavedGradientNoise(gl_FragCoord.xy);
    ray_pos += ray_step * offset;
    float startingDepth = (view * vec4(worldPos, 1.0)).z;
    

    for (uint i = 0; i < sssSteps; i++)
    {
        // Step the ray
        if(length(ray_pos - worldPos) > length(posToLight)) break;
        vec3 viewSpace = (view * vec4(ray_pos, 1.0)).xyz;
        vec4 clipSpace = proj * vec4(viewSpace,1.0);

        ray_uv  = (clipSpace.xy / clipSpace.w) * 0.5 + 0.5;

        // Ensure the UV coordinates are inside the screen
        if (ray_uv.x <= 1.0 && ray_uv.x >= 0.0 && ray_uv.y <= 1.0 && ray_uv.y >= 0.0)
        {
            // Compute the difference between the ray's and the camera's depth
            float depth_z     = texture(depth, ray_uv).r;
            vec4 sampleNDC = vec4(ray_uv.xy * 2.0 - 1.0, depth_z * 2.0 - 1.0, 1.0);
            vec4 sampleClipPos = invP * sampleNDC;
            vec3 sampleScreenPos = sampleClipPos.xyz /sampleClipPos.w;
            float depthDelta = sampleScreenPos.z - viewSpace.z;

            // Check if the camera can't "see" the ray (ray depth must be larger than the camera depth, so positive depth_delta)
            bool onCam = (depthDelta > 0.0 && depthDelta < sssThickness);
            if (onCam)
            {
                // Mark as occluded
                occlusion = cos((i/sssSteps * PI) / 2);
                break;
            }
        }
        ray_pos += ray_step;
    }

    // Convert to visibility
    return 1.0 - occlusion;
}

float InterleavedGradientNoise(vec2 screen_pos)
{
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(screen_pos, magic.xy)));
}