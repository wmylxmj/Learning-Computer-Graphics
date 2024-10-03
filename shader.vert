#version 430 core

layout (location = 0) in vec3 vertex_position;

out vec2 pixel;

void main() {
    gl_Position = vec4(vertex_position, 1.0);
    pixel = vertex_position.xy;
}
