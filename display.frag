#version 430 core

uniform sampler2D frame;
in vec2 pixel;
out vec4 FragColor;

void main() {
    FragColor = vec4(texture2D(frame, pixel.xy * 0.5 + 0.5).rgb, 1.0);
}
