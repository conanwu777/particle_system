#version 400 core
layout (location = 0) in vec3 aPos;

uniform float mx;
uniform float my;

out float d;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    d = sqrt((aPos.x - mx) * (aPos.x - mx)
    + (aPos.y - my) * (aPos.y - my) + aPos.z * aPos.z);
}
