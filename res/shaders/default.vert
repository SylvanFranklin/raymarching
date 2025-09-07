#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float aspect;

out vec2 uv;

void main() {
    gl_Position = vec4(aPos, 1.0);
    uv = vec2(aPos.x * aspect, aPos.y);
}
