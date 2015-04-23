
#include "P_base.h"

#include <iostream>
#include <fstream>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include "CheckShader.h"


// This does not belong here - should really have a nice shader class etc for sorting all this stuff out!
// Useful little function to just check for compiler errors


P_base::P_base(int i, std::string filename)
{
	
	// Initialise variables
	_VAO = 0;
	_program = 0;
	_shaderModelMatLocation = _shaderViewMatLocation = _shaderProjMatLocation = 0;
	_numVertices = 0;

	mesh = new ModelLoader();
	mesh->Load(filename);

	// Create the model
	InitialiseVAO();
	// Create the shaders
	InitialiseShaders();

	
	size = i;


	_position.x = (0.0);
	_position.y = (0.0);
	_position.z = (0.0);
	force.x = (0.0);
	force.y = (0.0);
	force.z = (0.0);
	_rotation.x = (0.0);
	_rotation.y = (0.0);
	_rotation.z = (0.0);
	accel.x = (0.0);
	accel.y = (0.0);
	accel.z = (0.0);
	velocity.x = (0.0);
	velocity.y = (0.0);
	velocity.z = (0.0);

	//create a new asteroid 


	drag = 0.5f;
	mass = 2.0f;
	//_modelMatrix;
	type = 1; //Runner Particle, will repel others when Ai activated
	U_id = 1; // Unit ID is set to 1
	deleteme = false; // This bool is used to delete the specific 

}

P_base::~P_base()
{
	// TODO: destroy VAO, shaders etc
}

void P_base::InitialiseVAO()
{
	// Creates one VAO
	glGenVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);
	_numVertices = mesh->GetMeshVertices().size();

	// stores data in the position buffer
	positionBuffer = 0;
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, _numVertices * sizeof(glm::vec3), &mesh->GetMeshVertices()[0], GL_STATIC_DRAW);

	// stores data in the normal buffer
	normalBuffer = 0;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, _numVertices * sizeof(glm::vec3), &mesh->GetMeshNormals()[0], GL_STATIC_DRAW);

	// stores data in the texture Coordinate buffer
	texCoordBuffer = 0;
	glGenBuffers(1, &texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, _numVertices * sizeof(glm::vec2), &mesh->GetMeshTexCoords()[0], GL_STATIC_DRAW);

	// Unbind for neatness, it just makes life easier
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Technically we can do this, because the enabled / disabled state is stored in the VAO
	glDisableVertexAttribArray(0);
}

std::string ReadFile(std::string fileName) {

	std::string result;

	std::ifstream stream(fileName, std::ios::in);
	if (stream.is_open()){
		std::string Line = "";
		while (getline(stream, Line))
			result += "\n" + Line;
		stream.close();
	}

	return result;
}

void P_base::InitialiseShaders()
{  // This is the vertex shader being loaded from file
	std::string VertexShaderCode = ReadFile("basic.vert");

	// This is the fragment shader
	std::string FragShaderCode = ReadFile("basic.frag");

	// The 'program' stores the shaders
	_program = glCreateProgram();

	// Create the vertex shader
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	// Give GL the source for it
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vShader, 1, &VertexSourcePointer, NULL);
	// Compile the shader
	glCompileShader(vShader);
	// Check it compiled and give useful output if it didn't work!
	if (!CheckShaderCompiled(vShader))
	{
		return;
	}
	// This links the shader to the program
	glAttachShader(_program, vShader);

	// Same for the fragment shader
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const* FragSourcePointer = FragShaderCode.c_str();
	glShaderSource(fShader, 1, &FragSourcePointer, NULL);
	glCompileShader(fShader);
	if (!CheckShaderCompiled(fShader))
	{
		return;
	}
	glAttachShader(_program, fShader);

	// This makes sure the vertex and fragment shaders connect together
	glLinkProgram(_program);
	// Check this worked
	GLint linked;
	glGetProgramiv(_program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLsizei len;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetProgramInfoLog(_program, len, &len, log);
		std::cerr << "ERROR: Shader linking failed: " << log << std::endl;
		delete[] log;

		return;
	}

	// We need to get the location of the uniforms in the shaders
	// This is so that we can send the values to them from the application
	// We do this in the following way:
	_shaderModelMatLocation = glGetUniformLocation(_program, "modelMat");
	_shaderViewMatLocation = glGetUniformLocation(_program, "viewMat");
	_shaderProjMatLocation = glGetUniformLocation(_program, "projMat");

}

void P_base::Update(float deltaTs, float mx, float my, float mz)
{
	// update the rotation angle of our cube
	for (int i = 0; i < size; i++)
	{
		//The following lines make the sphere follow the mouse position for debugging purposes
		//target.x = mx;
		//target.y = my;
		//target.z = mz;


		_rotation.x += deltaTs * 0.5f;
		_rotation.y += deltaTs * 0.5f;
		_rotation.z += deltaTs * 0.5f;
		while (_rotation.x >(3.14159265358979323846 * 2.0))
		{
			_rotation.x -= (3.14159265358979323846 * 2.0);
		}
		while (_rotation.y >(3.14159265358979323846 * 2.0))
		{
			_rotation.y -= (3.14159265358979323846 * 2.0);
		}
		while (_rotation.z >(3.14159265358979323846 * 2.0))
		{
			_rotation.z -= (3.14159265358979323846 * 2.0);
		}

		// Build the model matrix!
		// First we start with an identity matrix
		// This is created with the command: glm::mat4(1.0f)
		// Next, we translate this matrix according to the object's _position vector:
		_modelMatrix = glm::translate(glm::mat4(1.0f), _position);
		// Next, we rotate this matrix in the y-axis by the object's y-rotation:
		_modelMatrix = glm::rotate(_modelMatrix, _rotation.x, glm::vec3(1, 0, 0));
		_modelMatrix = glm::rotate(_modelMatrix, _rotation.y, glm::vec3(0, 1, 0));
		_modelMatrix = glm::rotate(_modelMatrix, _rotation.z, glm::vec3(0, 0, 1));
		// And there we go, model matrix is ready!

	}
}

void P_base::Draw(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, unsigned int texID)
{
	// Ok, here I like to indent drawing calls - it's just a personal style, you may not like it and that's fine ;)
	// Generally you will need to be activating and deactivating OpenGL states
	// I just find it visually easier if the activations / deactivations happen at different tab depths
	// This can help when things get more complex

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	
	// Activate the shader program
	glUseProgram(_program);
	
	// Activate the VAO
	glBindVertexArray(_VAO);
	
	for (int i = 0; i < size; i++)
	{
		// Send matrices to the shader as uniforms like this:
		glUniformMatrix4fv(_shaderModelMatLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
		glUniformMatrix4fv(_shaderViewMatLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(_shaderProjMatLocation, 1, GL_FALSE, glm::value_ptr(projMatrix));
	
		// Tell OpenGL to use the texture //send the diffuse texture to the shader
	
		int diffuseSampler = glGetUniformLocation(_program, "texSampler");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		glUniform1i(diffuseSampler, 0);
	
	
		//vertex position data read in from obj file
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
		//vertex normal data read in from obj file
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
		//vertex texture coordinate data read in from obj file
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
		// Tell OpenGL to draw it
		// Must specify the type of geometry to draw and the number of vertices
		glDrawArrays(GL_LINES, 0, _numVertices);
	
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
	
	// Unbind VAO
	glBindVertexArray(0);
	
	// Technically we can do this, but it makes no real sense because we must always have a valid shader program to draw geometry
	glUseProgram(0);
}

void P_base::setForce(float X, float Y, float Z)
{
	force.x = X;
	force.y = Y;
	force.z = Z;
}
void P_base::travelTime(float deltaTs)
{
	//This function is my physics calculator
	//For detailed explaination and information see my game report
	for (int i = 0; i < size; i++)
	{

		float dragX = (-velocity.x * drag);
		float dragY = (-velocity.y * drag);
		float dragZ = (-velocity.z * drag);

		accel.x = (force.x + dragX) / mass;
		accel.y = (force.y + dragY) / mass;
		accel.z = (force.z + dragZ) / mass;

		_position.x = _position.x + (velocity.x*deltaTs);
		_position.y = _position.y + (velocity.y*deltaTs);
		_position.z = _position.z + (velocity.z*deltaTs);

		velocity.x = velocity.x + (accel.x*deltaTs);
		velocity.y = velocity.y + (accel.y*deltaTs);
		velocity.z = velocity.z + (accel.z*deltaTs);

		force.x = force.y = force.z = 0.0f;
	}
}


void P_base::AI(void)
{

	//This function makes my particles orrient and revolve around a Target point, set by the mouse position
	if (_position.x < target.x) //is the x position of the particle less than the target X
	{
		force.x += 15.0f; // If so add to the X force of the particle
	}
	if (_position.x > target.x)
	{
		force.x += -15.0f;
	}
	if (_position.y < target.y)
	{
		force.y += 15.0f;
	}
	if (_position.y > target.y)
	{
		force.y += -15.0f;
	}
	if (_position.z < target.z)
	{
		force.z += 15.0f;
	}
	if (_position.z > target.z)
	{
		force.z += -15.0f;
	}

}
