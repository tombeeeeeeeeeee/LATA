#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 Tangent;
out vec3 BiTangent;

uniform mat4 model;
uniform mat4 vp; // View projection

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  

//    mat3 normalMatrix = transpose(inverse(mat3(model)));
//    vec3 T = normalize(normalMatrix * aTangent);
//    vec3 N = normalize(normalMatrix * aNormal);
//    T = normalize(T - dot(T, N) * N);
//    vec3 B = cross(N, T);
//    
//    mat3 TBN = transpose(mat3(T, B, N));
//    vs_out.TangentLightPos = TBN * lightPos;
//    vs_out.TangentViewPos  = TBN * viewPos;
//
//
//    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    TexCoords = aTexCoords;
    Tangent = aTangent;
    BiTangent = aBiTangent;

    gl_Position = vp * vec4(FragPos, 1.0);
}