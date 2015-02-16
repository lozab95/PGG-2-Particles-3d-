
#include "GameModel.h"

#include <iostream>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>


// This does not belong here - should really have a nice shader class etc for sorting all this stuff out!
// Useful little function to just check for compiler errors
bool CheckShaderCompiled( GLint shader )
{
	GLint compiled;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
	if ( !compiled )
	{
		GLsizei len;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

		// OpenGL will store an error message as a string that we can retrieve and print
		GLchar* log = new GLchar[len+1];
		glGetShaderInfoLog( shader, len, &len, log );
		std::cerr << "ERROR: Shader compilation failed: " << log << std::endl;
		delete [] log;

		return false;
	}
	return true;
}


GameModel::GameModel()
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

	type = 1; //Runner Particle, will repel others when Ai activated
	vX=0.0f;//Velocity on X and Y
	vY=0.0f;
	vZ=0.0f;
	accelX=0.0f;//Acceleration on X and Y
	accelY=0.0f;
	accelZ=0.0f;

	drag = 0.5f; //Drag force to slow forces
	mass = 2.0f; // Mass, to act with force to create realisitic slowly and acceleration
	forceX = 0.0f;// Force values for the X and Y axis
	forceY = 0.0f;
	forceZ = 0.0f;

	U_id = 1; // Unit ID is set to 1
	deleteme =  false; // This bool is used to delete the specific 
	position_x = 0.0f; //Initializing units position to 0
	position_y = 0.0f;
}

GameModel::~GameModel()
{
	// TODO: destroy VAO, shaders etc
}

void GameModel::InitialiseVAO()
{
		// Creates one VAO
	glGenVertexArrays( 1, &_VAO );
	// 'Binding' something makes it the current one we are using
	// This is like activating it, so that subsequent function calls will work on this item
	glBindVertexArray( _VAO );




	// Simple vertex data for a cube
	// (actually this is only four sides of a cube, you will have to expand this code if you want a complete cube :P )
	float vertices[] = {
		-0.1f, 0.1f, 0.1f,
		-0.1f,-0.1f, 0.1f,
		 0.1f, 0.1f, 0.1f,

		-0.1f,-0.1f, 0.1f,
		 0.1f,-0.1f, 0.1f,
		 0.1f, 0.1f, 0.1f,


		 0.1f, 0.1f, 0.1f,
		 0.1f,-0.1f, 0.1f,
		 0.1f, 0.1f,-0.1f,

		 0.1f,-0.1f, 0.1f,
		 0.1f,-0.1f,-0.1f,
		 0.1f, 0.1f,-0.1f,


		-0.1f, 0.1f, 0.1f,
		-0.1f, 0.1f,-0.1f,
		-0.1f,-0.1f, 0.1f,

		-0.1f,-0.1f, 0.1f,
		-0.1f, 0.1f,-0.1f,
		-0.1f,-0.1f,-0.1f,

		 0.1f, 0.1f,-0.1f,
		 0.1f,-0.1f,-0.1f,
		-0.1f, 0.1f,-0.1f,

		-0.1f, 0.1f,-0.1f,
		 0.1f,-0.1f,-0.1f,
		-0.1f,-0.1f,-0.1f

	};
	// Number of vertices in above data
	_numVertices = 24;

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
		 
		 0.0f, 0.0f, 0.5f,
		 0.0f, 0.0f, 0.5f,
		 0.0f, 0.0f, 0.5f,

		 0.5f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 
		 0.5f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 
		-0.5f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		 
		-0.5f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		
		 0.0f, 0.0f,-0.5f,
		 0.0f, 0.0f,-0.5f,
		 0.0f, 0.0f,-0.5f,
		 
		 0.0f, 0.0f,-0.5f,
		 0.0f, 0.0f,-0.5f,
		 0.0f, 0.0f,-0.5f
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

void GameModel::InitialiseShaders()
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
								uniform vec3 diffuseColour  = {0.8f,0.1f,0.1f};\n\
								uniform vec3 specularColour = {0.0f,0.0f,0.0f};\n\
								uniform float shininess     = 50.0f;\n\
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


void GameModel::Update( float deltaTs )
{
	// update the rotation angle of our cube
	_rotation.y += deltaTs * 0.5f;
	while( _rotation.y > (3.14159265358979323846 * 2.0) )
	{
		_rotation.y -= (3.14159265358979323846 * 2.0);
	}

	// Build the model matrix!
	// First we start with an identity matrix
	// This is created with the command: glm::mat4(1.0f)
	// Next, we translate this matrix according to the object's _position vector:
	_modelMatrix = glm::translate(glm::mat4(1.0f), _position );
	// Next, we rotate this matrix in the y-axis by the object's y-rotation:
	_modelMatrix = glm::rotate(_modelMatrix, _rotation.y, glm::vec3(0,1,0) );
	// And there we go, model matrix is ready!

}

void GameModel::Draw(glm::mat4 viewMatrix, glm::mat4 projMatrix)
{
	// Ok, here I like to indent drawing calls - it's just a personal style, you may not like it and that's fine ;)
	// Generally you will need to be activating and deactivating OpenGL states
	// I just find it visually easier if the activations / deactivations happen at different tab depths
	// This can help when things get more complex

	// Activate the shader program
	glUseProgram( _program );

		// Activate the VAO
		glBindVertexArray( _VAO );

			// Send matrices to the shader as uniforms like this:
			glUniformMatrix4fv(_shaderModelMatLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix) );
			glUniformMatrix4fv(_shaderViewMatLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix) );
			glUniformMatrix4fv(_shaderProjMatLocation, 1, GL_FALSE, glm::value_ptr(projMatrix) );


			// Tell OpenGL to draw it
			// Must specify the type of geometry to draw and the number of vertices
			glDrawArrays(GL_TRIANGLES, 0, _numVertices);
			
		// Unbind VAO
		glBindVertexArray( 0 );
	
	// Technically we can do this, but it makes no real sense because we must always have a valid shader program to draw geometry
	glUseProgram( 0 );
}

void GameModel::setForce(float X, float Y, float Z)
{
	forceX += X;
	forceY += Y;
	forceZ += Z;
}
void GameModel::travelTime (float deltaTs)
{
	//This function is my physics calculator
	//For detailed explaination and information see my game report
	float dragX = (-vX * drag);
	float dragY = (-vY * drag);
	float dragZ = (-vZ * drag);

	accelX = (forceX + dragX) / mass;
	accelY = (forceY + dragY) / mass;
	accelZ = (forceZ + dragZ) / mass;

	_position.x= _position.x + (vX*deltaTs); 
	_position.y= _position.y + (vY*deltaTs);
	_position.z= _position.z + (vZ*deltaTs);

	vX = vX + (accelX*deltaTs);
	vY = vY + (accelY*deltaTs);
	vZ = vZ + (accelZ*deltaTs);

	forceX = forceY = forceZ = 0.0f;
}

void GameModel::AI(float x, float y, float z)
{
	
	//forceX=0.0f;
	//forceY=0.0f;
	float tempx = (_position.x - x); //distance between local position and specifed position
	float tempy = (_position.y - y);
	float tempz = (_position.z - z);

	if (type == 1) //Type is repel
	{
		if ((tempx >= -0.3 && tempx <= 0.3) &&(tempy >= -0.3 && tempy <= 0.3)&&(tempz >= -0.3 && tempz <= 0.3)) //If within a -50 and +50 pixels on the X and Y 
		{
			if ( tempx < 0) // add negative force to the X
			{
				forceX+=-1.0f;
			}
			if( tempx > 0)
			{
				forceX+=1.0f;
			}
			if ( tempy < 0)
			{
				forceY+=-1.0f; 
			}
			if( tempy > 0)
			{
				forceY+=1.0f;
			}
			if ( tempz < 0)
			{
				forceZ+=-1.0f; 
			}
			if( tempz > 0)
			{
				forceZ+=1.0f;
			}
		}
	}
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
}
void GameModel::AI(void)
{                                                 
	//This function makes my particles orrient and revolve around a Target point, set by the mouse position
	if ( _position.x <= targetX) //is the x position of the particle less than the target X
	{
		forceX+=300.0f; // If so add to the X force of the particle
	}
	else if( _position.x >= targetX)
	{
		forceX+=-300.0f;
	}
	if ( _position.x <= targetY)
	{
		forceY+=300.0f;
	}
	else if( _position.x >= targetY)
	{
		forceY+=-300.0f;
	}
}
