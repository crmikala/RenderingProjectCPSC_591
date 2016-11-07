#version 430 core

uniform sampler1D colormap;
uniform sampler2D image; // connected to line 170 in sceneshader to bind2dTexture

out vec4 color;

in vec3 N;
in vec3 P;
in vec3 L;

in vec2 UV;

void main (void)
{
    vec3 diffuse_albedo = vec3(1.0, 1.0, 1.0);
    float kd = 0.8;   
    
    vec4 colorFromImage = texture(image, UV);
    
    float diffuse = max( 0.0, dot( N, normalize(L - P)));
    vec4 C = texture(colormap,diffuse);
    //color = vec4(C); // passed to GPU and converted from 0-1
	//color = vec4(UV, 0.0, 1.0);
	color = colorFromImage;
}
