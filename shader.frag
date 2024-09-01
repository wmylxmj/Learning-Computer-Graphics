#version 430 core

out vec4 FragColor;

struct Ray {
    vec3 origin;
    vec3 direction;
};

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}