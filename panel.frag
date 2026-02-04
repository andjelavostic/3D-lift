#version 330 core
out vec4 FragColor;

in vec2 fragUV;

uniform sampler2D uDiffMap1;

void main()
{
    FragColor = texture(uDiffMap1, fragUV);
}
