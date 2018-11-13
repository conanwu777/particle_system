#version 400 core
layout (location = 0) in vec3 aPos;

uniform mat4 p;
uniform float mx;
uniform float my;

out float d;

void main()
{
    gl_Position = p * vec4(aPos, 1.0);
    d = length(aPos - vec3(mx, my, 0.0));
}
