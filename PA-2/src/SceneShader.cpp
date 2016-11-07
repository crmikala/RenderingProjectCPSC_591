/*
 * SceneShader.cpp
 *
 *  Created on: Nov 17, 2015
 *      Author: acarocha
 */

#include "SceneShader.h"
#include "texture.h"
#include "lodepng.h"
static float PI = 3.14159265359;


SceneShader::SceneShader(): Shader()
{
	_programPlane = 0;
	_programMesh = 0;
	_planeVertexArray = -1;
	_planeVertexArray = -1;
	_mvUniform = -1;
	_projUniform = -1;
	_zTranslation = 1.0;
	_aspectRatio = 1.0;
	_xRot = 0.0;
	_yRot = 0.0;
	lightPosition = glm::vec3(0.5, 0.5, 0.5);
	
	size = 16;
	texture_pointer = 0;
	texture_names[0] = "textures/fig-7b.png";
	texture_names[1] = "textures/fig-7c.png";
	texture_names[2] = "textures/fig-7d.png";
	texture_names[3] = "textures/fig-8a.png";
	texture_names[4] = "textures/fig-8b.png";
	texture_names[5] = "textures/fig-9b.png";
	texture_names[6] = "textures/fig-9c.png";
	texture_names[7] = "textures/fig-9e.png";
	texture_names[8] = "textures/fig-9f.png";
	texture_names[9] = "textures/fig-10b.png";
	texture_names[10] = "textures/fig-10c.png";
	texture_names[11] = "textures/fig-10d.png";
	texture_names[12] = "textures/fig-11b.png";
	texture_names[13] = "textures/fig-11c.png";
	texture_names[14] = "textures/fig-11d.png";
	texture_names[15] = "textures/texture_6.png"; // chalk texture by Claire Mikalauskas
	
	r = 2.0;
	
	model_type = 1;

}

void SceneShader::calculateCylindricalUVCoordinates()
{
	for(unsigned int i = 0; i < _mesh->vertices.size(); i++)
	{
		glm::vec3 vertex(_mesh->vertices[i][0],
						_mesh->vertices[i][1],
						_mesh->vertices[i][2]);
						
		// Note that the coordinates of this model have height along the y-axis
		float theta = glm::atan(vertex.z, vertex.x)/PI; 
		float h = vertex.y * 15.0; // height scaled by 4
		
		_uvs.push_back(glm::vec2(theta, h));
	}
}

void SceneShader::readMesh( std::string filename )
{
	_mesh = trimesh::TriMesh::read(filename);

	_mesh->need_bbox();
	_mesh->need_faces();
	_mesh->need_normals();
	_mesh->need_bsphere();

	_triangleIndices.clear();
	for(unsigned int i = 0; i < _mesh->faces.size(); i++)
	{
   	  _triangleIndices.push_back(_mesh->faces[i][0]);
	  _triangleIndices.push_back(_mesh->faces[i][1]);
	  _triangleIndices.push_back(_mesh->faces[i][2]);
	}
}



void SceneShader::createVertexBuffer()
{
	//create texture 1d
	std::vector<GLubyte> colormap;
	
	//red
	colormap.push_back(255); //r
	colormap.push_back(0); //g
	colormap.push_back(0);//b
	colormap.push_back(255); //alpha
	
	//blue
	colormap.push_back(0); //r
	colormap.push_back(0); //g
	colormap.push_back(255);//b
	colormap.push_back(255); //alpha
	
	_textureID = _texture.create1DTexture(colormap);
	
	//create plane geometry
	static const GLfloat quadData[] =
	{
                        -1.0f, 0.0f, -1.0f,
                        -1.0f, 0.0f, 1.0f,
                        1.0f, 0.0f, -1.0f,
                        1.0f, 0.0f, 1.0f,
	};
	
	//added in tutorial
	//plane texture coordinates
	//triangle strip
	static const GLfloat uvsPlane[] =
	{
                        0.0f, 0.0f,
                        0.0f, 1.0f,
                        1.0f, 0.0f,
                        1.0f, 1.0f,
	};

	//passing model attributes to the GPU
	//plane
	glGenVertexArrays(1, &_planeVertexArray);
	glBindVertexArray(_planeVertexArray);

	glGenBuffers(1, &_planeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _planeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof (quadData), quadData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	
	//connected uvsplane coordinates to GPU
	glGenBuffers(1, &_planeTextureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _planeTextureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof (uvsPlane), uvsPlane, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	//read and create mesh geometry
	//readMesh("./models/bunny.ply"); //normalized vertices coordinates
	
	// code reuse from PA-1
	switch (model_type)
	{
	case (1):
			readMesh("./models/bunny.ply");
			model_scale = 4.0;
			model_y_position = -0.42;
	break;
	case (2):
			readMesh("./models/dragon.ply");
			model_scale = 1.0;
			model_y_position = 0.02;
	break;
	case (3):
			readMesh("./models/bug.obj");
			model_scale = 0.01;
			model_y_position = -0.05;
	break;
	case (4):
			readMesh("./models/cat.obj");
			model_scale = 0.002;
			model_y_position = 0.01;
	break;
	default:
			readMesh("./models/bunny.ply");
			model_scale = 4.0;
			model_y_position = -0.42;
	break;
	}
	
	//loading image
	for(int i = 0; i < size; i++)
	{
		filenames[i] = (texture_names[i]);
		//reads an image and stores in the buffer and also returns width and height
		unsigned int error = lodepng::decode( _image, _imageWidth, _imageHeight, filenames[i].c_str());
	
		if(error)
		{ 
			std::cout << "error" << error << ":" << lodepng_error_text(error) << std::endl;
		}
	
		//creating 2D texture
		textures[i] = _texture.create2DTexture(_image, _imageWidth, _imageHeight);
		_image.clear();
	}
	_texture2Did = textures[texture_pointer];
	
	calculateCylindricalUVCoordinates();
	
	//triangle mesh
	glGenVertexArrays(1, &_meshVertexArray);
	glBindVertexArray(_meshVertexArray);

	glGenBuffers(1, &_meshVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,  _mesh->vertices.size() * sizeof (trimesh::point), _mesh->vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &_meshNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, _mesh->normals.size() * sizeof(trimesh::vec), _mesh->normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	
	glGenBuffers(1, &_meshTextureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshTextureBuffer);
	glBufferData(GL_ARRAY_BUFFER, _uvs.size() * sizeof(glm::vec2), _uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &_meshIndicesBuffer );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshIndicesBuffer );
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _triangleIndices.size()*sizeof(unsigned int), _triangleIndices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

}


void SceneShader::startup()
{
	_programPlane = compile_shaders("./shaders/plane.vert", "./shaders/plane.frag");

	_programMesh = compile_shaders("./shaders/mesh.vert", "./shaders/mesh.frag");

	_programLight = compile_shaders("./shaders/light.vert", "./shaders/light.frag");

	createVertexBuffer();

}

void SceneShader::renderPlane()
{
	
	glBindVertexArray(_planeVertexArray);
	
	// added in tutorial on textures
	glUseProgram(_programPlane);
	_texture.bind1DTexture(_programPlane, _textureID, std::string("colormap"));
	_texture.bind2DTexture(_programPlane, _texture2Did, std::string("image"));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glUseProgram(_programPlane);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f,0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	glm::mat4 identity(1.0f);
	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        _modelview *=  rotationX;
	
	//Uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programPlane, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	// added in tutorial on textures
	_texture.unbind1DTexture();
	_texture.unbind2DTexture();

	glBindVertexArray(0);
	glDisable(GL_BLEND);
}

void SceneShader::renderMesh()
{
	glBindVertexArray(_meshVertexArray);

	glUseProgram(_programMesh);
	
	_texture2Did = textures[texture_pointer];
	
	// binding the 2D texture
	_texture.bind2DTexture(_programMesh, _texture2Did, std::string("image"));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 identity(1.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        _modelview *=  rotationX;

	//uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programMesh, "lightPosition"), 1, glm::value_ptr(lightPosition) );
	
	glUniform1f(glGetUniformLocation(_programMesh, "r"), r);
	
	//scale and y position of model
	glUniform1f(glGetUniformLocation(_programMesh, "model_scale"), model_scale);
	glUniform1f(glGetUniformLocation(_programMesh, "model_y_position"), model_y_position);

	glDrawElements( GL_TRIANGLES, _mesh->faces.size()*3, GL_UNSIGNED_INT, 0 );

	_texture.unbind2DTexture();
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}


void SceneShader::renderLight()
{
	glUseProgram(_programLight);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 identity(1.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        _modelview *=  rotationX;

	//uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programLight, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	glPointSize(30.0f);
	glDrawArrays( GL_POINTS, 0, 1);

}

void SceneShader::render()
{
	renderPlane();
	renderMesh();
	renderLight();
}

void SceneShader::setZTranslation(float z)
{
	_zTranslation = z;
}

void SceneShader::setAspectRatio(float ratio)
{
	_aspectRatio = ratio;
}

void SceneShader::setRotationX( float x )
{
	_xRot = x;
}

void SceneShader::setRotationY( float y )
{
	_yRot = y;
}

void SceneShader::shutdown()
{
	glDeleteBuffers(1, &_meshVertexBuffer);
	glDeleteBuffers(1, &_meshNormalBuffer);
	glDeleteBuffers(1, &_meshIndicesBuffer );
	glDeleteVertexArrays(1, &_meshVertexArray);
	glDeleteVertexArrays(1, &_planeVertexArray);
}

void SceneShader::updateLightPositionX(float x)
{
	lightPosition.x += x;
}

void SceneShader::updateLightPositionY(float y)
{
	lightPosition.y += y;
}

void SceneShader::updateLightPositionZ(float z)
{
	lightPosition.z += z;
}

void SceneShader::changeTexture()
{
	if(texture_pointer < (size - 1))
	{
		texture_pointer++;
	}
	else
	{
		texture_pointer = 0;
	}
}

void SceneShader::increaseR(float inc)
{
	if(r < 8.0)
	{
		r = r + inc;
	}
	else
	{
		r = 8.0;
	}
	std::cout << "Increase. r = " << r << "\n";
}

void SceneShader::decreaseR(float inc)
{
	if(r > 1.0)
	{
		r = r - inc;
	}
	else
	{
		r = 1.0;
	}
	std::cout << "Decrease. r = " << r << "\n";
}

void SceneShader::changeModel(int num)
{
	model_type = num;
	createVertexBuffer();
}

SceneShader::~SceneShader()
{
	shutdown();
}
