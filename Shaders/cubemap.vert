#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 vp; // View Projection

void main()
{
        mat4 inversePV = inverse(vp);
        vec2 pos = vec2( (gl_VertexID & 2)>>1, 1 - (gl_VertexID & 1)) * 2.0 - 1.0;
        vec4 front = inversePV * vec4(pos, -1.0, 1.0);
        vec4 back = inversePV * vec4(pos,  1.0, 1.0);

        TexCoord = back.xyz / back.w - front.xyz / front.w;
        gl_Position = vec4(pos, 1.0, 1.0);
}