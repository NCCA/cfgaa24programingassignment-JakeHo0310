#version 330

in vec2 UV;
out vec4 fragColor;

void main()
{
    // Interpolate colors from blue to black based on UV coordinates
    fragColor = mix(vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), UV.y);
}