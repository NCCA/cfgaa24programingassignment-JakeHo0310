#version 410 core

layout(location=0) out vec4 fragColour;
in vec3 particleColour;

uniform sampler2D tex;

void main()
{
    vec4 colour=texture(tex,gl_PointCoord);
    if(colour.a<0.2)
        discard;
    else
    fragColour.rgb=particleColour; //texture(tex,gl_PointCoord).rgb;
}