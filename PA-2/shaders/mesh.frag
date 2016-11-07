#version 430 core

out vec4 color;

in vec3 N;
in vec3 V;
in vec3 L;

in vec2 UV;

uniform sampler2D image;
uniform float r;


void main(void)
{    
    
    float D = pow(abs(dot(N,V)), r);
    float X = dot(N,L); 
    if(X < 0)
    {
		X = 0.01;
	}
    vec2 uv = vec2(X, D);
    
    vec4 colorFromImage = texture(image, uv);
	color = colorFromImage;
    
    
    
}
