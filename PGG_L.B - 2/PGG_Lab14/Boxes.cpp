
#include "Boxes.h"

#include <iostream>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include "CheckShader.h"


// This does not belong here - should really have a nice shader class etc for sorting all this stuff out!
// Useful little function to just check for compiler errors



Boxes::Boxes( int i)
{
	// Initialise variables
	_VAO = 0;
	_program = 0;
	_shaderModelMatLocation = _shaderViewMatLocation = _shaderProjMatLocation = 0;
	_numVertices = 0;
	 
	// Create the model
	InitialiseVAO();
	// Create the shaders
	InitialiseShaders();

	size = i;

	for (int l = 0; l < size; l++)
	{
		float newx = rand() % 400 - 210;
		float newy = rand() % 400 - 210;
		float newz = rand() % 2000 - 10000;
		newx = newx / 100;
		newy = newy / 100;
		newz = newz / 100;

		glm::vec3 newPosition(newx, newy, newz);
		glm::vec3 newRotation(0.0f, 0.0f, 0.0f);
		glm::vec3 newForce(0.0f, 0.0f, 0.0f);
		glm::vec3 newAccel(0.0f, 0.0f, 0.0f);
		glm::vec3 newTarget(0.0f, 0.0f, newz);
		glm::vec3 newVelocity(0.0f, 0.0f, 0.0f);
		bool newdeleteme = false;
		glm::mat4 new_modelMatrix;
		//create a new asteroid 

		accel.push_back(newAccel);
		target.push_back(newTarget);
		_rotation.push_back(newRotation);
		_position.push_back(newPosition);
		velocity.push_back(newVelocity);
		force.push_back(newForce);
		_modelMatrix.push_back(new_modelMatrix);
		deleteme.push_back(newdeleteme);
		
	}

	drag = 0.5f;
	mass = 2.0f;
	type = 1; //Runner Particle, will repel others when Ai activated
	U_id = 1; // Unit ID is set to 1


}

Boxes::~Boxes()
{
	// TODO: destroy VAO, shaders etc
}

int Boxes::Find(float x, float y, float z)
{
	int indxClosest = 0; 
	//Let find the closest piece of matter
	for (int i = 0; i < size; i++)
	{
		float tempx = (_position[i].x - x);
		float tempy = (_position[i].y - y);
		float tempz = (_position[i].z - z);

		float Closetempx = (_position[indxClosest].x -x);
		float Closetempy = (_position[indxClosest].y - y);
		float Closetempz = (_position[indxClosest].z - z);

		if ((tempx < Closetempx) && (tempy < Closetempy) && (tempz < Closetempz))
		{
			indxClosest = i;
		}
	}
	return indxClosest;
}

void Boxes::InitialiseVAO()
{
		// Creates one VAO
	glGenVertexArrays( 1, &_VAO );
	// 'Binding' something makes it the current one we are using
	// This is like activating it, so that subsequent function calls will work on this item
	glBindVertexArray( _VAO );




	// Simple vertex data for a cube
	// (actually this is only four sides of a cube, you will have to expand this code if you want a complete cube :P )
	float vertices[] = {

		-0.05f, -0.05f, 0.05f,
		0.05f, -0.05f, 0.05f,
		0.05f, -0.05f, -0.05f,
		-0.05f, -0.05f, -0.05f,
		-0.05f, 0.05f, 0.05f,
		0.05f, 0.05f, 0.05f,
		0.05f, 0.05f, -0.05f,
		-0.05f, 0.05f, -0.05f,

	};
	// Number of vertices in above data
	_numVertices = 8;

	
	// Variable for storing a VBO
	GLuint positionBuffer = 0;
	// Create a generic 'buffer'
	glGenBuffers(1, &positionBuffer);
	// Tell OpenGL that we want to activate the buffer and that it's a VBO
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	// With this buffer active, we can now send our data to OpenGL
	// We need to tell it how much data to send
	// We can also tell OpenGL how we intend to use this buffer - here we say GL_STATIC_DRAW because we're only writing it once
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _numVertices * 3, vertices, GL_STATIC_DRAW);

	// This tells OpenGL how we link the vertex data to the shader
	// (We will look at this properly in the lectures)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray(0);




	


	// Normal data for our incomplete cube
	// Each entry is the normal for the corresponding vertex in the position data above
	float normals[] = {
		 0.0f, 0.0f, 0.5f,
		 0.0f, 0.0f, 0.5f,
		 0.0f, 0.0f, 0.5f,

		 0.5f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 
		-0.5f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,

		 
		 0.0f, 0.0f,-0.5f,
		 0.0f, 0.0f,-0.5f,
		 0.0f, 0.0f,-0.5f
	};
	
	

	//for (int i = 0; i < 8; i++)
	//{
	//	glm::vec3 n(vertices[i * 3 + 0], vertices[i * 3 + 1], vertices[i * 3 + 2]);
	//
	//	n = glm::normalize(n);
	//
	//	normals[i * 3 + 0] = n.x;
	//	normals[i * 3 + 1] = n.y;
	//	normals[i * 3 + 2] = n.z;
	//}
	
	GLubyte Indices[] = {
		
		0, 3, 1,
		1, 3, 2,

		4, 0, 5,
		5, 0, 1,

		1, 2, 5,
		5, 2, 6,

		3, 0, 4,
		4, 7, 3,

		3, 7, 6,
		6, 2, 3,

		6, 7, 5,
		5, 7, 4


	};
	// Variable for storing a VBO
	GLuint normalBuffer = 0;
	// Create a generic 'buffer'
	glGenBuffers(1, &normalBuffer);
	// Tell OpenGL that we want to activate the buffer and that it's a VBO
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	// With this buffer active, we can now send our data to OpenGL
	// We need to tell it how much data to send
	// We can also tell OpenGL how we intend to use this buffer - here we say GL_STATIC_DRAW because we're only writing it once
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _numVertices * 3, normals, GL_STATIC_DRAW);

	// This tells OpenGL how we link the vertex data to the shader
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	

	// Unbind for neatness, it just makes life easier
	// As a general tip, especially as you're still learning, for each function that needs to do something specific try to return OpenGL in the state you found it in
	// This means you will need to set the states at the beginning of your function and set them back at the end
	// If you don't do this, your function could rely on states being set elsewhere and it's easy to lose track of this as your project grows
	// If you then change the code from elsewhere, your current code could mysteriously stop working properly!
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray( 0 );

	// Technically we can do this, because the enabled / disabled state is stored in the VAO
	//glDisableVertexAttribArray(0);

}

void Boxes::InitialiseShaders()
{
	// OpenGL doesn't provide any functions for loading shaders from file
	// I'm feeling lazy, so just hardcoding them here

	// This is the vertex shader
	const GLchar *vShaderText = "#version 430 core\n\
						 layout(location = 0) in vec4 vPosition;\n\
						 layout(location = 1) in vec3 vNormalIn;\n\
						 \n\
						 uniform mat4 modelMat;\n\
						 uniform mat4 invModelMat;\n\
						 uniform mat4 viewMat;\n\
						 uniform mat4 projMat;\n\
						 \n\
						 uniform vec4 worldSpaceLightPos = {1,0.8,1,1};\n\
						 \n\
						 out vec3 vNormalV;\n\
						 out vec3 lightDirV;\n\
						 \n\
						 void main()\n\
						 {\n\
								gl_Position = projMat * viewMat * modelMat * vPosition;\n\
								\n\
								vec4 eyeSpaceVertPos = viewMat * modelMat * vPosition;\n\
								vec4 eyeSpaceLightPos = viewMat * worldSpaceLightPos;\n\
								\n\
								lightDirV =  normalize( vec3(eyeSpaceLightPos) - vec3(eyeSpaceVertPos) );\n\
								\n\
								vNormalV = mat3(viewMat * modelMat) * vNormalIn;\n\
						 }";

	// This is the fragment shader
	const GLchar *fShaderText = "#version 430 core\n\
								in vec3 lightDirV;\n\
								in vec3 vNormalV;\n\
								\n\
								uniform vec3 lightColour = {1,1,1};\n\
								uniform vec3 emissiveColour = {0,0,0};\n\
								uniform vec3 ambientColour  = {0.1f,0.1f,0.2f};\n\
								uniform vec3 diffuseColour  = {0.2f,0.2f,1.0f};\n\
								uniform vec3 specularColour = {0.7f,0.3f,0.2f};\n\
								uniform float shininess     = 100.0f;\n\
								uniform float alpha         = 1.0f;\n\
								\n\
								out vec4 fragColour;\n\
								\n\
								void main()\n\
								{\n\
									vec3 lightDir = normalize( lightDirV );\n\
									vec3 vNormal = normalize( vNormalV );\n\
									\n\
										vec3 diffuse = diffuseColour * lightColour * max( dot( vNormal, lightDir ), 0);\n\
										\n\
										fragColour = vec4( emissiveColour + ambientColour + diffuse, alpha);\n\
								}";
						 // 
						 /*
									vec3 ambient = vec3();
									vec3 diffuse = vec3(1.0f,0.3f,0.3f) * max(dot( normalize(lightDir), normalize(vNormal) ),0);
									fColor = vec4( ambient + diffuse, 1.0f);
									*/

	// The 'program' stores the shaders
	_program = glCreateProgram();

	// Create the vertex shader
	GLuint vShader = glCreateShader( GL_VERTEX_SHADER );
	// Give GL the source for it
	glShaderSource( vShader, 1, &vShaderText, NULL );
	// Compile the shader
	glCompileShader( vShader );
	// Check it compiled and give useful output if it didn't work!
	if( !CheckShaderCompiled( vShader ) )
	{
		return;
	}
	// This links the shader to the program
	glAttachShader( _program, vShader );

	// Same for the fragment shader
	GLuint fShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fShader, 1, &fShaderText, NULL );
	glCompileShader( fShader );
	if( !CheckShaderCompiled( fShader ) )
	{
		return ;
	}
	glAttachShader( _program, fShader );

	// This makes sure the vertex and fragment shaders connect together
	glLinkProgram( _program );
	// Check this worked
	GLint linked;
	glGetProgramiv( _program, GL_LINK_STATUS, &linked );
	if ( !linked )
	{
		GLsizei len;
		glGetProgramiv( _program, GL_INFO_LOG_LENGTH, &len );

		GLchar* log = new GLchar[len+1];
		glGetProgramInfoLog( _program, len, &len, log );
		std::cerr << "ERROR: Shader linking failed: " << log << std::endl;
		delete [] log;

		return;
	}

	// We need to get the location of the uniforms in the shaders
	// This is so that we can send the values to them from the application
	// We do this in the following way: 
	_shaderModelMatLocation = glGetUniformLocation( _program, "modelMat" );
	_shaderViewMatLocation = glGetUniformLocation( _program, "viewMat" );
	_shaderProjMatLocation = glGetUniformLocation( _program, "projMat" );

}


void Boxes::Update(float deltaTs)
{

	// update the rotation angle of our cube
	for (int i = 0; i < size; i++)
	{
	


		_rotation[i].x += deltaTs * 0.5f;
		_rotation[i].y += deltaTs * 0.5f;
		_rotation[i].z += deltaTs * 0.5f;
		//_position[i].z += deltaTs * 0.9f;
		while (_rotation[i].x >(3.14159265358979323846 * 2.0))
		{
			_rotation[i].x -= (3.14159265358979323846 * 2.0);
		}
		while (_rotation[i].y >(3.14159265358979323846 * 2.0))
		{
			_rotation[i].y -= (3.14159265358979323846 * 2.0);
		}
		while (_rotation[i].z >(3.14159265358979323846 * 2.0))
		{
			_rotation[i].z -= (3.14159265358979323846 * 2.0);
		}

		

		// Build the model matrix!
		// First we start with an identity matrix
		// This is created with the command: glm::mat4(1.0f)
		// Next, we translate this matrix according to the object's _position vector:
		_modelMatrix[i] = glm::translate(glm::mat4(1.0f), _position[i]);
		// Next, we rotate this matrix in the y-axis by the object's y-rotation:
		_modelMatrix[i] = glm::rotate(_modelMatrix[i], _rotation[i].x, glm::vec3(1, 0, 0));
		_modelMatrix[i] = glm::rotate(_modelMatrix[i], _rotation[i].y, glm::vec3(0, 1, 0));
		_modelMatrix[i] = glm::rotate(_modelMatrix[i], _rotation[i].z, glm::vec3(0, 0, 1));
		// And there we go, model matrix is ready!
		/*
		if ((_position[i].x < -3.0f) || (_position[i].x > 3.0f))
		{
			resizeDown(i);
		}
		else if ((_position[i].y < -3.0f) || (_position[i].y > 3.0f))
		{
			resizeDown(i);
		}
		*/

	}
}

void Boxes::Draw(const glm::mat4 &viewMatrix, const glm::mat4 &projMatrix)
{
	// Ok, here I like to indent drawing calls - it's just a personal style, you may not like it and that's fine ;)
	// Generally you will need to be activating and deactivating OpenGL states
	// I just find it visually easier if the activations / deactivations happen at different tab depths
	// This can help when things get more complex

	// Activate the shader program
	glUseProgram( _program );

		// Activate the VAO
		glBindVertexArray( _VAO );

		for (int i = 0; i < size; i++)
		{
			// Send matrices to the shader as uniforms like this:
			glUniformMatrix4fv(_shaderModelMatLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix[i]));
			glUniformMatrix4fv(_shaderViewMatLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(_shaderProjMatLocation, 1, GL_FALSE, glm::value_ptr(projMatrix));


			// Tell OpenGL to draw it
			// Must specify the type of geometry to draw and the number of vertices

			if (type == 1)
			{
				glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid*)0);
			}
		}
			
		// Unbind VAO
		glBindVertexArray( 0 );
	
	// Technically we can do this, but it makes no real sense because we must always have a valid shader program to draw geometry
	glUseProgram( 0 );
}

void Boxes::setForce(int index, float X, float Y, float Z)
{
	force[index].x = X;
	force[index].y = Y;
	force[index].z = Z;
}
void Boxes::travelTime (float deltaTs)
{
	//This function is my physics calculator
	//For detailed explaination and information see my game report
	for (int i = 0; i < size; i++)
	{

		float dragX = (-velocity[i].x * drag);
		float dragY = (-velocity[i].y * drag);
		float dragZ = (-velocity[i].z * drag);

		accel[i].x = (force[i].x + dragX) / mass;
		accel[i].y = (force[i].y + dragY) / mass;
		accel[i].z = (force[i].z + dragZ) / mass;

		_position[i].x = _position[i].x + (velocity[i].x*deltaTs);
		_position[i].y = _position[i].y + (velocity[i].y*deltaTs);
		_position[i].z = _position[i].z + (velocity[i].z*deltaTs);

		velocity[i].x = velocity[i].x + (accel[i].x*deltaTs);
		velocity[i].y = velocity[i].y + (accel[i].y*deltaTs);
		velocity[i].z = velocity[i].z + (accel[i].z*deltaTs);

		force[i].x = force[i].y = force[i].z = 0.0f;
	}
}

void Boxes::Repel()
{
	
	//forceX=0.0f;
	//forceY=0.0f;
	for (int j = 0; j < size; j++)
	{
		for (int i = 0; i < size; i++)
		{
			if (i != j)
			{

				float tempx = (_position[j].x - _position[i].x); //distance between local position and specifed position
				float tempy = (_position[j].y - _position[i].y);
				float tempz = (_position[j].z - _position[i].z);

				if (type == 1) //Type is repel
				{
					if ((tempx >= -0.1f && tempx <= 0.1f) && (tempy >= -0.1f && tempy <= 0.1f) && (tempz >= -0.1f && tempz <= 0.1f)) //If within a -50 and +50 pixels on the X and Y 
					{
						if (tempx < 0) // add negative force to the X
						{
							force[j].x += -1000.0f;
						}
						if (tempx > 0)
						{
							force[j].x += 1000.0f;
						}
						if (tempy < 0)
						{
							force[j].y += -1000.0f;
						}
						if (tempy > 0)
						{
							force[j].y += 1000.0f;
						}
						if (tempz < 0)
						{
							force[j].z += -1000.0f;
						}
						if (tempz > 0)
						{
							force[j].z += 1000.0f;
						}
					}
				}
			}
		}
	}
	/*
	if (type == 2) //type is attract 
	{
		if ((tempx >= -250 && tempx <= 250) &&(tempy >= -250 && tempy <= 250))
		{
			if ( tempx < 0) // add positive force to the X
			{
				forceX+=110.0f;
			}
			if( tempx > 0)
			{
				forceX+=-110.0f;
			}
			if ( tempy < 0)
			{
				forceY+=110.0f;
			}
			if( tempy > 0)
			{
				forceY+=-110.0f;
			}
		}
	}
	*/
}
void Boxes::AI(void)
{        
	for (int i = 0; i < size; i++)
	{
		//This function makes my particles orrient and revolve around a Target point, set by the mouse position
		if (_position[i].x < target[i].x) //is the x position of the particle less than the target X
		{
			force[i].x += 5.0f; // If so add to the X force of the particle
		}
		if (_position[i].x > target[i].x)
		{
			force[i].x += -5.0f;
		}
		if (_position[i].y < target[i].y)
		{
			force[i].y += 5.0f;
		}
		if (_position[i].y > target[i].y)
		{
			force[i].y += -5.0f;
		}
		if (_position[i].z < target[i].z)
		{
			force[i].z += 5.0f;
		}
		if (_position[i].z > target[i].z)
		{
			force[i].z += -5.0f;
		}
	}
}
