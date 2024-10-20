#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colourTexture;

uniform sampler2D colourGradeKey1;
uniform sampler2D colourGradeKey2;

uniform float colourGradeInterpolation;

void main()
{
    vec3 colourTap = texture2D(colourTexture, TexCoords).rgb;
    
    colourTap = clamp(colourTap, 0.0, 1.0);

    colourTap = colourTap * (30.0 / 32.0) + (1.0/32.0);
    
    float blueUV1 = floor(colourTap.b * 32.0) / 32.0;
    float blueUV2 = blueUV1 + (1.0/32.0);
    float blueUVBlend = fract(colourTap.b * 32.0);
    vec2 gradeKeyUVs1 = vec2(colourTap.r / 32.0 + blueUV1, colourTap.g);
    vec2 gradeKeyUVs2 = vec2(colourTap.r / 32.0 + blueUV2, colourTap.g);
    
    vec4 firstColour = mix(texture2D(colourGradeKey1, gradeKeyUVs1), texture2D(colourGradeKey1, gradeKeyUVs2), blueUVBlend);
    vec4 secondColour = mix(texture2D(colourGradeKey2, gradeKeyUVs1), texture2D(colourGradeKey2, gradeKeyUVs2), blueUVBlend);
    
    FragColor = mix(firstColour, secondColour, colourGradeInterpolation);
//    FragColor = vec4(mix(colourTap, firstColour.xyz, colourGradeInterpolation), 1.0);
    //FragColor = vec4(gradeKeyUVs1, 0.0, 1.0);
} 