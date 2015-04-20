
#ifndef __GAME_OBJECT2__
#define __GAME_OBJECT2__

#include <glm.hpp>
#include <SDL.h>
#include "glew.h"
#include <vector>

/// Class to store and display a model
class P_base
{
public:

	/// Constructor calls InitialiseVAO and InitialiseShaders
	P_base(int i);
	~P_base();

	/// Loads object model into OpenGL
	void InitialiseVAO();

	/// Loads shaders for the object into OpenGL
	void InitialiseShaders();

	/// Currently just updates rotation to make the model rotate
	void Update(float deltaTs, float mx, float my, float mz);

	/// Draws object using the given camera view and projection matrices
	void Draw(const glm::mat4 &viewMatrix, const glm::mat4 &projMatrix);

	/// For setting the position of the model
	void SetPosition(float posX, float posY, float posZ) { _position.x = posX; _position.y = posY; _position.z = posZ; }

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
	inline float getPosx()
	{
		return _position.x;
	};
	inline void setPosx(float i)
	{
		_position.x = i;
	};
	inline float getPosy()
	{
		return _position.y;
	};
	inline float getPosz()
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
	inline void setTarget( float x, float y, float z)
	{
		target.x = x;
		target.y = y;
		target.z = z;
	};
	inline void setType(int i)
	{
		type = i;

	};
	inline int getType(void)
	{
		return type;

	};

	//Ai functions
	void Repel(void);
	void AI(void);
	//Physics Functions
	void setForce(float x, float y, float z);
	void travelTime(float deltaTs);

protected:

	/// Object position vector


	/// Euler angles for rotation


	/// Vertex Array Object for model in OpenGL
	GLuint _VAO;
	GLuint IndexBufferId;

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
	int size;

	glm::vec3 _position;
	glm::vec3 _rotation;
	glm::vec3 force;
	glm::vec3 velocity;
	glm::vec3 accel;
	glm::vec3 target;

	float mass;
	float drag;


	int health;
	int type;
	int U_id;
	bool deleteme;

};


#endif
