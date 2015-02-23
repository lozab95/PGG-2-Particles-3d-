
#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include <glm.hpp>
#include <SDL.h>
#include "glew.h"
#include <vector>

/// Class to store and display a model
class GameModel
{
public:

	/// Constructor calls InitialiseVAO and InitialiseShaders
	GameModel(int i);
	~GameModel();

	/// Loads object model into OpenGL
	void InitialiseVAO();

	/// Loads shaders for the object into OpenGL
	void InitialiseShaders();

	/// Currently just updates rotation to make the model rotate
	void Update(float deltaTs, float mx, float my, float mz);

	/// Draws object using the given camera view and projection matrices
	void Draw(const glm::mat4 &viewMatrix, const glm::mat4 &projMatrix);

	/// For setting the position of the model
	void SetPosition(int index, float posX, float posY, float posZ) { _position[index].x = posX; _position[index].y = posY; _position[index].z = posZ; }

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
	inline float getPosx(int index)
	{
		return _position[index].x;
	};
	inline void setPosx(int index, float i)
	{
		_position[index].x = i;
	};
	inline float getPosy(int index)
	{
		return _position[index].y;
	};
	inline float getPosz(int index)
	{
		return _position[index].z;
	};
	inline void setPosy(int index, float i)
	{
		_position[index].y = i;
	};
	inline void setPosz(int index, float i)
	{
		_position[index].z = i;
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
	inline void setTarget(int index, float x, float y, float z)
	{
		target[index].x = x;
		target[index].y = y;
		target[index].z = z;
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
	void Repel(void);
	void AI(void);
	//Physics Functions
	void setForce(int index, float x, float y, float z);
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

	std::vector<glm::mat4> _modelMatrix;
	/// Number of vertices in the model
	unsigned int _numVertices;

	// Variables for gameplay
	int size;

	std::vector<glm::vec3> _position;
	std::vector<glm::vec3> _rotation;
	std::vector<glm::vec3> force;
	std::vector<glm::vec3> velocity;
	std::vector<glm::vec3> accel;
	std::vector<glm::vec3> target;

	float mass;
	float drag;
	

	int health;
	int type;
	int U_id;
	bool deleteme;

};


#endif
