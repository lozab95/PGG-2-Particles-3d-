
#include <SDL.h>
#include <vector>
#include "glew.h"
#include "time.h"
#include <iostream>
#include <sstream>
// The GLM library contains vector and matrix functions and classes for us to use
// They are designed to easily work with OpenGL!
#include <glm.hpp> // This is the main GLM header
#include <gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include "CheckShader.h"
#include "Boxes.h"
#include "P_Base.h"


bool collision(float, float, float, float, float, float, float, float);

// An initialisation function, mainly for GLEW
// This will also print to console the version of OpenGL we are using
bool InitGL()
{
	// GLEW has a problem with loading core OpenGL
	// See here: https://www.opengl.org/wiki/OpenGL_Loading_Library
	// The temporary workaround is to enable its 'experimental' features
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cerr << "Error: GLEW failed to initialise with message: " << glewGetErrorString(err) << std::endl;
		return false;
	}
	std::cout << "INFO: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	std::cout << "INFO: OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "INFO: OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "INFO: OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	return true;
}


int main(int argc, char *argv[])
{
	// This is our initialisation phase

	// SDL_Init is the main initialisation function for SDL
	// It takes a 'flag' parameter which we use to tell SDL what systems we're going to use
	// Here, we want to use SDL's video system, so we give it the flag for this
	// Incidentally, this also initialises the input event system
	// This function also returns an error value if something goes wrong
	// So we can put this straight in an 'if' statement to check and exit if need be
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		// Something went very wrong in initialisation, all we can do is exit
		std::cout << "Whoops! Something went very wrong, cannot initialise SDL :(" << std::endl;
		return -1;
	}



	// This is how we set the context profile
	// We need to do this through SDL, so that it can set up the OpenGL drawing context that matches this
	// (of course in reality we have no guarantee this will be available and should provide fallback methods if it's not!)
	// Anyway, we basically first say which version of OpenGL we want to use
	// So let's say 4.3:
	// Major version number (4):
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	// Minor version number (3):
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Then we say whether we want the core profile or the compatibility profile
	// Flag options are either: SDL_GL_CONTEXT_PROFILE_CORE   or   SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
	// We'll go for the core profile
	// This means we are using the latest version and cannot use the deprecated functions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	unsigned seed = time(0);

	// Now we have got SDL initialised, we are ready to create a window!
	// These are some variables to help show you what the parameters are for this function
	// You can experiment with the numbers to see what they do
	int winPosX = 100;
	int winPosY = 100;
	int winWidth = 1280;
	int winHeight = 960;
	SDL_Window *window = SDL_CreateWindow("My Window!!!",  // The first parameter is the window title
		winPosX, winPosY,
		winWidth, winHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	// The last parameter lets us specify a number of options
	// Here, we tell SDL that we want the window to be shown and that it can be resized
	// You can learn more about SDL_CreateWindow here: https://wiki.libsdl.org/SDL_CreateWindow?highlight=%28\bCategoryVideo\b%29|%28CategoryEnum%29|%28CategoryStruct%29
	// The flags you can pass in for the last parameter are listed here: https://wiki.libsdl.org/SDL_WindowFlags

	// The SDL_CreateWindow function returns an SDL_Window
	// This is a structure which contains all the data about our window (size, position, etc)
	// We will also need this when we want to draw things to the window
	// This is therefore quite important we don't lose it!






	// The SDL_Renderer is a structure that handles rendering
	// It will store all of SDL's internal rendering related settings
	// When we create it we tell it which SDL_Window we want it to render to
	// That renderer can only be used for this window
	// (yes, we can have multiple windows - feel free to have a play sometime)
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);


	// Now that the SDL renderer is created for the window, we can create an OpenGL context for it!
	// This will allow us to actually use OpenGL to draw to the window
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	// Call our initialisation function to set up GLEW and print out some GL info to console
	if (!InitGL())
	{
		return -1;
	}


	// We are going to work out how much time passes from frame to frame
	// We will use this variable to store the time at our previous frame
	// This function returns the number of milliseconds since SDL was initialised
	unsigned int lastTime = SDL_GetTicks();


	// Enable the depth test to make sure triangles in front are always in front no matter the order they are drawn
	glEnable(GL_DEPTH_TEST);

	// Create a model
	P_base *PlayerBase = new P_base(1);

	PlayerBase->setPosx(0.5f);
	PlayerBase->setPosy(0.0f);
	PlayerBase->setPosz(-3.0f);


	Boxes *boxes = new Boxes(2000); //create a new asteroid 


	// Set object's position like this:

	float Mx = 0; //Mouse locations
	float My = 0;

	// We are now preparing for our main loop (also known as the 'game loop')
	// This loop will keep going round until we exit from our program by changing the bool 'go' to the value false
	// This loop is an important concept and forms the basis of most games you'll be writing
	// Within this loop we generally do the following things:
	//   * Check for input from the user (and do something about it!)
	//   * Update our world
	//   * Draw our world
	// We will come back to this in later lectures

	bool spacetrue = false;
	bool go = true;
	int target = 0;
	while (go)
	{

		unsigned int current = SDL_GetTicks();
		float deltaTs = (float)(current - lastTime) / 1000.0f;
		lastTime = current;

		//std::cout << 1.0f / deltaTs << std::endl;
		std::cout << boxes->getSize() << std::endl;
		//Collision Checks
		for (int i = 0; i < boxes->getSize(); i++)
		{
			float sum = PlayerBase->getPosz() - boxes->getPosz(i);
			if ((sum < 2.0) && (sum > -2.0))
			{
				if (collision(PlayerBase->getPosx() - 0.25f, PlayerBase->getPosy() - 0.25f, boxes->getPosx(i) - 0.15f, boxes->getPosy(i) - 0.15f, 0.5, 0.5, 0.3, 0.3) == true) //check if its near the mouse
				{
					boxes->setDelete(i);//set delete bool of the box 
				}
			}
			if (boxes->getDelete(i) == true)
			{
				boxes->resizeDown(i); //delete the specific box and all relevant data
			}

		}

		//std::cout << "x:  " << Mx << "-- y:  " << My << "px:  " << Particle1[0]->getPosx() << "py:  " << Particle1[0]->getPosy() << std::endl;
		// Here we are going to check for any input events
		// Basically when you press the keyboard or move the mouse, the parameters are stored as something called an 'event'
		// SDL has a queue of events
		// We need to check for each event and then do something about it (called 'event handling')
		// the SDL_Event is the datatype for the event
		SDL_Event incomingEvent;
		// SDL_PollEvent will check if there is an event in the queue
		// If there's nothing in the queue it won't sit and wait around for an event to come along (there are functions which do this, and that can be useful too!)
		// For an empty queue it will simply return 'false'
		// If there is an event, the function will return 'true' and it will fill the 'incomingEvent' we have given it as a parameter with the event data
		while (SDL_PollEvent(&incomingEvent))
		{
			// If we get in here, we have an event and need to figure out what to do with it
			// For now, we will just use a switch based on the event's type
			switch (incomingEvent.type)
			{
			case SDL_QUIT:
				// The event type is SDL_QUIT
				// This means we have been asked to quit - probably the user clicked on the 'x' at the top right corner of the window
				// To quit we need to set our 'go' bool to false so that we can escape out of the game loop
				go = false;
				break;

				// If you want to learn more about event handling and different SDL event types, see:
				// https://wiki.libsdl.org/SDL_Event
				// and also: https://wiki.libsdl.org/SDL_EventType
				// but don't worry, we'll be looking at handling user keyboard and mouse input soon


			case SDL_MOUSEMOTION:

				boxes->resizeUP();

				Mx = incomingEvent.motion.x;
				My = incomingEvent.motion.y;
				Mx = (Mx / 100) - 6.40;
				My = ((My / 100) - 4.80) * -1;


			case SDL_KEYDOWN:
				// The event type is SDL_KEYDOWN
				// This means that the user has pressed a key
				// Let's figure out which key they pressed
				switch (incomingEvent.key.keysym.sym)
				{
				case SDLK_DOWN:
					//Move Down
					PlayerBase->setForce(0, -1, 0);
					break;
				case SDLK_UP:
					//Move Up
					PlayerBase->setForce(0, 1, 0);
					break;
				case SDLK_LEFT:
					//Move Left
					PlayerBase->setForce(-1, 0, 0);
					break;
				case SDLK_RIGHT:
					//Move Right
					PlayerBase->setForce(1, 0, 0);
					break;
				case SDLK_a:
					break;
				case SDLK_d:
					break;
				case SDLK_w:
					PlayerBase->setForce(0, 0, -1);
					break;
				case SDLK_s:
					PlayerBase->setForce(0, 0, 1);
					break;
				case SDLK_SPACE:
				{
								   if (spacetrue == false)
								   {
									   spacetrue = true;
								   }
								   else
								   {
									   spacetrue = false;
								   }
								   break;
				}
					break;
				}
				break;
			}
		}

		if (spacetrue == true)
		{
			target = 0;
			target = boxes->Find(PlayerBase->getPosx(), PlayerBase->getPosy(), PlayerBase->getPosz());
			//if (target != 0)
			{
				PlayerBase->setTarget(boxes->getPosx(target), boxes->getPosy(target), boxes->getPosz(target));
				PlayerBase->AI();
			}
		}
		// Update the model, to make it rotate
		boxes->Update(deltaTs);
		PlayerBase->Update(deltaTs, Mx, My, (deltaTs * -100));


		boxes->Repel();
		boxes->AI();
		boxes->travelTime(deltaTs);
		PlayerBase->travelTime(deltaTs);


		//For Particle repeling



		// Draw our world

		// Specify the colour to clear the framebuffer to
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		// This writes the above colour to the colour part of the framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// Construct a projection matrix for the camera
		glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 40000.0f);

		// Create a viewing matrix for the camera
		// Don't forget, this is the opposite of where the camera actually is
		// You can think of this as moving the world away from the camera
		glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2.5f));

		// Draw the object using the given view (which contains the camera orientation) and projection (which contains information about the camera 'lense')
		boxes->Draw(View, Projection);
		PlayerBase->Draw(View, Projection);



		// This tells the renderer to actually show its contents to the screen
		// We'll get into this sort of thing at a later date - or just look up 'double buffering' if you're impatient :P
		SDL_GL_SwapWindow(window);

		// Limiter in case we're running really quick
		/*
		if( deltaTs < (1.0f/50.0f) )	// not sure how accurate the SDL_Delay function is..
		{
		SDL_Delay((unsigned int) (((1.0f/50.0f)
		- deltaTs)*1000.0f) );
		}
		*/
	}

	// If we get outside the main game loop, it means our user has requested we exit


	// Our cleanup phase, hopefully fairly self-explanatory ;)
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

bool collision(float x1, float y1, float x2, float y2, float x1d, float y1d, float x2d, float y2d) //Check if collision is true
{
	if (((x1 + x1d) < x2) || ((x2 + x2d)< x1))//&& (( (y1+y1d) > y2 ) && (y1+y1d) < (y2+y2d)))
	{
		return false;
	}
	else if ((y1 > (y2 + y2d)) || (y2 > (y1 + y1d)))
	{
		return false;// return true if collision is true.
	}
	else
	{
		return true;
	}

}