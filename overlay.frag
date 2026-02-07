#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 uColor;          
uniform sampler2D uTexture;   
uniform bool useTexture;      
uniform float uAlpha = 1.0;

void main() {
    vec4 baseColor = useTexture ? texture(uTexture, TexCoord) : vec4(uColor, 1.0);

    baseColor.a *= uAlpha; // primeni providnost
    FragColor = baseColor;
}