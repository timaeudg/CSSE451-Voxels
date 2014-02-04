
#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include "glew/glew.h"
#include <SFML/Window.hpp>
#define RESOLUTION 512

GLuint shaderProg;

GLuint positionBuffer;
GLuint colorBuffer;

GLuint positionSlot;
GLuint colorSlot;

GLfloat positions[10] = {-1,-1, 1,1, -0.5,-0.366, 0,0.5, 0.5,-0.366};
GLfloat colors[15] = {1,1,1, 0.5,0.5,0.5, 1,0,0, 0,1,0, 0,0,1};

void printProgramLinkLog(GLuint obj);
void printProgramCompileLog(GLuint obj);

/*
 Draw a single frame
 */
void display()
{
	// Clear the color bits in the display buffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Use a simple shader to render the line
	glUseProgram(shaderProg);
	
	// Render using vertex attributes (data already on GPU) (~2008, 3.0)
	// http://www.arcsynthesis.org/gltut/Basics/Tut01%20Following%20the%20Data.html
	
	// Tell OpenGL we want to use a buffer on the GPU
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	
	// Tell OpenGL what shader data slot we want to use
	glEnableVertexAttribArray(positionSlot);
	
	// Tell OpenGL how to interpret the data
	glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	// Do the same thing for colors
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glEnableVertexAttribArray(colorSlot);
	glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	// Draw some primitives as: glDrawArrays(type, base, size)
	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_TRIANGLES, 2, 3);
	
	// Tell OpenGL we are done with the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Tell OpenGL we are done with the shader slot
	glDisableVertexAttribArray(positionSlot);
	glDisableVertexAttribArray(colorSlot);
	
	// Tell OpenGL we are done with the shader
	glUseProgram(0);
}

/*
 Initialize the graphics state
 */
void graphicsInit()
{
	// glew will help us use GL functions, so set it up here
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
	
	// Shaders are programs that do the actual rendering on the GPU
	// We will discuss these in detail later, for now, just set them up
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	char const * vertSource = "attribute vec2 vert; attribute vec3 color; varying vec4 smoothColor; void main() { gl_Position=vec4(vert.xy, 0, 1); smoothColor=vec4(color.xyz, 1); }";
	char const * fragSource = "varying vec4 smoothColor; void main() { gl_FragColor = smoothColor; }";
	
	glShaderSource(vertShader, 1, (char const **)&vertSource, NULL);
	glShaderSource(fragShader, 1, (char const **)&fragSource, NULL);
	
	glCompileShader(vertShader);
	printProgramCompileLog(vertShader);
	glCompileShader(fragShader);
	printProgramCompileLog(fragShader);
	
	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertShader);
	glAttachShader(shaderProg, fragShader);
	
	glLinkProgram(shaderProg);
	printProgramLinkLog(shaderProg);
	
	// The data we will render needs to be on the GPU
	// These commands upload the data
	
	// Find out where the shader expects the data
	positionSlot = glGetAttribLocation(shaderProg, "vert");
	colorSlot = glGetAttribLocation(shaderProg, "color");

	// Generate a GPU side buffer
	glGenBuffers(1, &positionBuffer);
	
	// Tell OpenGL we want to work with the buffer we just made
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	
	// Allocate and upload data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	
	// Do the same thing for the color data
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


class GLBox
{
public:
	GLBox()
	{
		// Create the main window
		App = new sf::Window(sf::VideoMode(RESOLUTION, RESOLUTION, 32), "program1");
		
		graphicsInit();
		
		// Start render loop
		while (App->IsOpened())
		{			
			// Set the active window before using OpenGL commands
			// It's not needed here because the active window is always the same,
			// but don't forget it if you use multiple windows or controls
			App->SetActive();
			
			// Handle any events that are in the queue
			sf::Event Event;
			while (App->GetEvent(Event))
			{
				// Close window : exit
				if (Event.Type == sf::Event::Closed)
					App->Close();
				
				// Escape key : exit
				if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
					App->Close();
				
				// This is for grading your code. DO NOT REMOVE
				if(Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Equal)
				{
					unsigned char *dest;
					unsigned int w = App->GetWidth();
					unsigned int h = App->GetHeight();
					glPixelStorei(GL_PACK_ALIGNMENT, 1);
					dest = (unsigned char *) malloc( sizeof(unsigned char)*w*h*3);
					glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, dest);
					
					FILE *file;
					file = fopen("_program1.ppm", "wb");
					fprintf(file, "P6\n%i %i\n%i\n", w, h, 255);
					for(int i=h-1; i>=0; i--)
						fwrite(&dest[i*w*3], sizeof(unsigned char), w*3, file);
					fclose(file);
					free(dest);
				}
			}
			
			// Render the scene
			display();
			
			// Finally, display rendered frame on screen
			App->Display();
		}
	}
	
	~GLBox()
	{
		// Clean up the buffer
		glDeleteBuffers(1, &positionBuffer);
	}
private:
	sf::Window *App;
};

void printProgramCompileLog(GLuint obj)
{
	GLint infologLength;
	GLint status;
	int charsWritten = 0;
	char *infoLog;
	
	glGetProgramiv(obj, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
		return;
	
	printf("Error compiling shader: ");
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s",infoLog);
		free(infoLog);
	}
	printf("\n");
}

void printProgramLinkLog(GLuint obj)
{
	GLint infologLength;
	GLint status;
	char *infoLog;
	int charsWritten  = 0;
	
	glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_TRUE)
		return;
	
	printf("Error linking shader: ");
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s",infoLog);
		free(infoLog);
	}
	printf("\n");
}

int main()
{
	GLBox prog;
	
    return EXIT_SUCCESS;
}
