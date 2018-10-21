#version 400 core
layout (location = 0) in vec3 aPos;

uniform vec2 mx;

out float d;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    d = sqrt((aPos.x - mx.x) * (aPos.x - mx.x)
    + (aPos.y - mx.y) * (aPos.y - mx.y) + aPos.z * aPos.z);
}
