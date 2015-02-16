
#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include <glm.hpp>
#include <SDL.h>
#include "glew.h"

/// Class to store and display a model
class GameModel
{
public:

	/// Constructor calls InitialiseVAO and InitialiseShaders
	GameModel();
	~GameModel();

	/// Loads object model into OpenGL
	void InitialiseVAO();

	/// Loads shaders for the object into OpenGL
	void InitialiseShaders();

	/// Currently just updates rotation to make the model rotate
	void Update( float deltaTs );

	/// Draws object using the given camera view and projection matrices
	void Draw(glm::mat4 viewMatrix, glm::mat4 projMatrix);

	/// For setting the position of the model
	void SetPosition( float posX, float posY, float posZ ) {_position.x = posX; _position.y = posY; _position.z = posZ;}

	//Getters and Setters!

	inline int getHealth(void)
	{
		return health;
	};
	inline void setHealth(int i)
	{
		health = i;
	};
	//Positions
	inline float getPosx(void)
	{
		return _position.x;
	};
	inline void setPosx(float i)
	{
		_position.x = i;
	};
	inline float getPosy(void)
	{
		return _position.y;
	};
	inline float getPosz(void)
	{
		return _position.z;
	};
	inline void setPosy(float i)
	{
		_position.y = i;
	};
	inline void setPosz(float i)
	{
		_position.z = i;
	};
	//Other
	inline bool getDelete(void)
	{
		return deleteme;
	};
	inline void setDelete(void)
	{
		deleteme = true;
	};
	inline void setTarget(int x, int y)
	{
		targetX=x;
		targetY=y;
	};
	inline void setType(int i)
	{
		type=i;
		
	};
	inline int getType(void)
	{
		return type;
		
	};

	//Ai functions
	void AI(float x, float y, float z);
	void AI(void);
	//Physics Functions
	void setForce(float x, float y, float z);
	void travelTime(float deltaTs);

protected:

	/// Object position vector
	glm::vec3 _position;

	/// Euler angles for rotation
	glm::vec3 _rotation;

	/// Vertex Array Object for model in OpenGL
	GLuint _VAO;

	/// Shader program
	GLuint _program;

	/// Uniform locations
	GLint _shaderModelMatLocation, _shaderViewMatLocation, _shaderProjMatLocation;

	/// Object's model matrix
	/// This is rebuilt in the update function
	glm::mat4 _modelMatrix;

	/// Number of vertices in the model
	unsigned int _numVertices;

	// Variables for gameplay
	float forceX, forceY, forceZ;
	float accelX, accelY, accelZ;
	float mass;
	float drag;
	float vX, vY, vZ;


	int targetX;
	int targetY;

	int health;
	int type;
	float position_x;
	float position_y;
	int U_id;
	bool deleteme;

};


#endif
