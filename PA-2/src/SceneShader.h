/*
 * SceneShader.h
 *
 *  Created on: Nov 17, 2015
 *      Author: acarocha
 */

#ifndef SCENESHADER_H_
#define SCENESHADER_H_

#include "Shader.h"
#include <vector>

#define GLM_FORCE_RADIANS

#include "TriMesh.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture.h"

class SceneShader : public Shader
{
public:

	SceneShader();
	~SceneShader();

	void startup ();
	void shutdown ();
	void render();

	void renderPlane();
	void renderMesh();
	void renderLight();

	void setAspectRatio( float ratio );
	void setZTranslation(float z);
	void setRotationX( float x );
	void setRotationY( float y );

	void updateLightPositionX( float x );
	void updateLightPositionY( float y );
	void updateLightPositionZ( float z );
	
	/* texture */
	void changeTexture();

	/* user control*/
	void increaseR(float inc);
	void decreaseR(float inc);
	
	void changeModel(int num);

private:

	/*methods*/

	void calculateCylindricalUVCoordinates();
	void readMesh( std::string filename );
	void createVertexBuffer();


	/*variables*/
	GLuint _programLight;
	GLuint _programPlane;
	GLuint _programMesh;


	GLuint _planeVertexArray;
	GLuint _planeVertexBuffer;
	
	GLuint _planeTextureBuffer;

	GLuint _meshVertexArray;
	GLuint _meshVertexBuffer;
	GLuint _meshNormalBuffer;
	GLuint _meshIndicesBuffer;
	GLuint _meshTextureBuffer;

        GLint _mvUniform, _projUniform;

	/* Matrices */
	glm::mat4 _modelview;
	glm::mat4 _projection;

	float _zTranslation;
	float _xRot;
	float _yRot;
	float _aspectRatio;

	/* Textures */
	GLuint _textureID;
	GLuint _texture2Did;
	
	Texture _texture;
	
	int size;
	int texture_pointer;
	std::string texture_names[16];
	GLuint textures[16];
	std::string filenames[16];
	
	// user control 
	float r;
	
	//define model to be used 
	int model_type;
	
	//scale and y_position of model
	float model_scale;
	float model_y_position;

	/*vertices*/
	std::vector<float> _vertices;
	std::vector<glm::vec2> _uvs;
	std::vector<unsigned char> _image;
	unsigned int _imageWidth;
	unsigned int _imageHeight;

	trimesh::TriMesh* _mesh;
	std::vector<unsigned int> _triangleIndices;

	glm::vec3 lightPosition;

};

#endif /* SCENESHADER_H_ */
