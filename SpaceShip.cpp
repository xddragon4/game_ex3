#include "SpaceShip.h"
#include "globals.h"
#include <glm/gtc/matrix_transform.hpp>

SpaceShip::SpaceShip(const glm::vec3 pos, const glm::vec4 & color, const char* vShaderFile, const char* fShaderFile, 
					const char* textureIMG, const char*  meshPath) :
	OpenMeshObject(vShaderFile, fShaderFile, pos, color, meshPath, textureIMG), _angleYaw(-90.f), _anglePitch(0.f)
{
	_updateModel();

	for (int comm = 0; comm != NUM_OF_COMMANDS; comm++) {
		Commands command = static_cast<Commands>(comm);
		_commands[command] = false;
	}
}

SpaceShip::~SpaceShip()
{
}

void SpaceShip::draw(const glm::mat4 & projection, glm::vec3 lightPos, glm::vec4 lightColor)
{
	BEGIN_OPENGL;
	{
		_useMVP(projection, _camera->getViewMatrix());
		setWorldUniforms(_camera->getPosition(), lightPos, lightColor);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(_vao);

		// Get a handle for our "gMaterialColor" uniform
		GLuint materialID = glGetUniformLocation(_programID, MATERIAL_COLOR);
		glUniform4f(materialID, _color.r, _color.g, _color.b, _color.a);

		GLuint textureSamplerID = glGetUniformLocation(_programID, TEXTURE_SAMPLER);
		glUniform1i(textureSamplerID, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _textureID);

		//draw
		//size_t n_faces = _mesh.n_faces();
		glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
		//glDrawArrays(GL_TRIANGLES, 0, n_faces * 3);

		// Unbind the Vertex Array object
		glBindVertexArray(0);
	}
	END_OPENGL;
}

void SpaceShip::init()
{
	OpenMeshObject::init();

	// Initialize vertices buffer and transfer it to OpenGL
	{
		// Create and bind the Mesh Vertex Array Object
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// Create and load vertex data into a Vertex Buffer Object
		{
			glGenBuffers(1, &_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBufferData(GL_ARRAY_BUFFER,
				sizeof(glm::vec4) * _vertices.size(),
				&_vertices[0],
				GL_STATIC_DRAW);

			// Obtain attribute handles:
			GLint _posAttr = glGetAttribLocation(_programID, "position");
			glEnableVertexAttribArray(_posAttr);
			glVertexAttribPointer(_posAttr, // attribute handle
				4,          // number of scalars per vertex
				GL_FLOAT,   // scalar type
				GL_FALSE,
				sizeof(glm::vec4) * 2,
				0);

			GLint _normAttr = glGetAttribLocation(_programID, "norm");
			glEnableVertexAttribArray(_normAttr);
			glVertexAttribPointer(_normAttr, // attribute handle
				4,          // number of scalars per vertex
				GL_FLOAT,   // scalar type
				GL_FALSE,
				sizeof(glm::vec4) * 2,
				(GLvoid*)(sizeof(glm::vec4)));

			// Unbind vertex array:
			glBindVertexArray(0);
		}
	}

	if (0 < _textureImg.size())
	{
		_textureID = initTexture(_textureImg.c_str());
	}
}

void SpaceShip::update()
{
	if (_commands[TURN_UP]) { _pitchLogic(TURN_SPEED, TURN_UP); }
	if (_commands[TURN_DOWN]) { _pitchLogic(-TURN_SPEED, TURN_DOWN); }
	if (_commands[TURN_RIGHT]) { _yawLogic(-TURN_SPEED, TURN_RIGHT); }
	if (_commands[TURN_LEFT]) { _yawLogic(TURN_SPEED, TURN_LEFT); }
	if (_commands[MOVE_FORWORD]) { _moveForwordLogic(); }
	_updateModel();
}

void SpaceShip::_updateModel() {
	_model = glm::translate(glm::mat4(1), _position);
	_model = glm::rotate(_model, _angleYaw, _camera->getUp());
	_model = glm::rotate(_model, _anglePitch, _camera->getPitchAxis());
}

void SpaceShip::_pitchLogic(float speed, Commands comm) {
	_commands[comm] = false;
	//update position
	glm::mat4 rotation = glm::rotate(glm::mat4(1), speed, _camera->getPitchAxis());
	_position = glm::vec3(rotation*glm::vec4(_position, 1));
	//update pitch angle
	_anglePitch += speed;
}

void SpaceShip::_yawLogic(float speed, Commands comm) {
	_commands[comm] = false;
	//update position
	glm::mat4 rotation = glm::rotate(glm::mat4(1), speed, _camera->getUp());
	_position = glm::vec3(rotation*glm::vec4(_position, 1));
	//update pitch angle
	_angleYaw += speed;
}

void SpaceShip::_moveForwordLogic()
{
	_commands[MOVE_FORWORD] = false;
	//TODO: move position by speed*direction
	//_calculateViewMatrix();
}

void SpaceShip::turnUp()		{ _commands[TURN_UP] = true; }
void SpaceShip::turnDown()		{ _commands[TURN_DOWN] = true; }
void SpaceShip::turnLeft()		{ _commands[TURN_LEFT] = true; }
void SpaceShip::turnRight()		{ _commands[TURN_RIGHT] = true; }
void SpaceShip::moveForword()	{ _commands[MOVE_FORWORD] = true; }
