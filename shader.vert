#version 430 core
layout (location = 0) in vec3 vertexPosition;

uniform mat4 matModel;
uniform mat4 matCamera;
uniform mat4 matPerspective;

void main()
{
    gl_Position = matPerspective * matCamera * matModel * vec4(vertexPosition, 1.0);
}