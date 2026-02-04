#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

out vec2 fragUV;

uniform mat4 uM;
uniform mat4 uV;
uniform mat4 uP;

void main()
{
    fragUV = inUV;
    gl_Position = uP * uV * uM * vec4(inPos, 1.0);
}
