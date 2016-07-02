#ifndef HEADER_Quad_CLASS
#define HEADER_Quad_CLASS

//includes go here
#include <gl/glew.h>

namespace GL {

	//A quadratic plane, centered at (0,0) with size 2, facting negative z.
	class Quad {
	public:
		Quad();

		virtual ~Quad();

		//Initializes the model, so it can be drawn
		void init();

		//Draws the model - only works if the model has been initialized.
		void draw();

		//Returns true if the model is initialized and ready to draw.
		bool isInitialized() const;

		//Frees the ressources the model has accquired - only works if the model has been initialized
		void dispose();

	private:
		GLuint vbo;
		GLuint indicesBuffer;
		GLuint uvBuffer;
		GLuint vao;
		bool initialized = false;
	};
}

#endif