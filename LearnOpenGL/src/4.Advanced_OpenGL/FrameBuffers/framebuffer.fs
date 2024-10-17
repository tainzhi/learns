#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

const float offset = 1.0 / 300.0;



void main()
{    
    // generate kernel effects
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset), vec2(0.0f, offset), vec2(offset, offset),
        vec2(-offset, 0.0f), vec2(0.0f, 0.0f), vec2(offset, 0.0f),
        vec2(-offset, -offset), vec2(0.0f, -offset), vec2(offset, -offset)
    );
    float kernel[9] = float[](
        -1, -1, -1,
        -1, 9, -1,
        -1, -1, -1
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(texture1, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * kernel[i];
    }
    FragColor = vec4(col, 1.0);
    // make grey effects
    // FragColor = texture(texture1, TexCoords);
    // float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    // FragColor = vec4(average, average, average, 1.0);
}