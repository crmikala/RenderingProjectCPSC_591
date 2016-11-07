#version 430 core

uniform mat4 modelview;
uniform mat4 projection;
uniform vec3 lightPosition;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

//attributes in camera coordinates
out vec3 N; //normal
out vec3 V; //view vector
out vec3 L; //light

out vec2 UV;

uniform float model_scale;
uniform float model_y_position;

void main(void)
{
	UV = uv;
	
    vec4 lightCameraSpace = modelview * vec4(lightPosition, 1.0);
    lightCameraSpace /= lightCameraSpace.w;

    mat3 normalMatrix = mat3(transpose(inverse(modelview)));
    N = normalize(normalMatrix * normal);

    float scale = model_scale;    
    vec3 positionModelSpace = vertex*scale + vec3(0.0, model_y_position, 0.0);
    vec4 positionCameraSpace = modelview * vec4(positionModelSpace, 1.0);

    vec3 P = positionCameraSpace.xyz/positionCameraSpace.w;
   
    L = normalize(lightCameraSpace.xyz - P);
    V = normalize(-P);

    gl_Position = projection * positionCameraSpace;    
}
