#ifndef __RENDERENGINE
#define __RENDERENGINE

#define ARENA_SIZE 10.0
#define NUMBER_OF_LARGE_ASTEROIDS 4
#define NUMBER_OF_MEDIUM_ASTEROIDS_PER_LARGE 2
#define NUMBER_OF_SMALL_ASTEROIDS_PER_MEDIUM 2
#define LARGE_ASTEROID_RADIUS 3.0
#define MEDIUM_ASTEROID_RADIUS 1.5
#define SMALL_ASTEROID_RADIUS 0.75

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLHelper.h"
#include "Cube.hpp"
using namespace glm;
using namespace std;
class RenderEngine
{
public:
	RenderEngine()
	{
		initialized = false;
		this->P = glm::ortho(-1, 1, -1, 1);
	}

	~RenderEngine()
	{
		if(initialized)
		{
			// Clean up the buffers
			glDeleteBuffers(1, &positionBuffer);
			glDeleteBuffers(1, &elementBuffer);
			glDeleteBuffers(1, &colorBuffer);
			glDeleteBuffers(1, &normBuffer);
			glDeleteVertexArrays(1, &vertexArray);
		}
	}

	void init()
	{
		clock.Reset();
		setupGlew();
		setupShader();
		setupBuffers();
		initialized = true;
	}

	void display(vec3 camera = vec3(1,3,5), vec3 target = vec3(3,3,0), vec3 up = vec3(0,0,1), bool pickingEnabled = false)
	{
		glEnable(GL_DEPTH_TEST);
		//clear the old frame
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//use shader
		glUseProgram(shaderProg);
		
		//draw
		glBindVertexArray(vertexArray);

		mat4 C = mat4(1);
		C = glm::lookAt(camera, target,up);
		P = perspective(90.f, 16.0f/9.0f, 0.1f, 200.f);

		glUniformMatrix4fv(cameraSlot, 1, GL_FALSE, &C[0][0]);
		glUniformMatrix4fv(perspectiveSlot, 1, GL_FALSE, &P[0][0]);
		GLfloat t = clock.GetElapsedTime();
		glUniform1f(timeSlot, t);

		glm::mat4 T = glm::mat4(1);
		T =  glm::scale(T, glm::vec3(ARENA_SIZE, ARENA_SIZE, ARENA_SIZE));

		GLuint elementsSize = points.size();
		
		if(pickingEnabled){
//			printf("Picking is enabled\n");
			glUniform1i(pickingSlot, GL_TRUE);
		} else {
			glUniform1i(pickingSlot, GL_FALSE);
		}
		
		glUniformMatrix4fv(matSlot, 1, GL_FALSE, &T[0][0]);
		glUniform1f(idSlot, 0.0);
		glDrawElements(GL_TRIANGLES, arena.triangles.size(), GL_UNSIGNED_INT, 0);
		
		//cleanup
		glBindVertexArray(0);
		glUseProgram(0);
		checkGLError("display");
	}
	
	void reshape(int const & newWidth, int const & newHeight)
	{
		glViewport(0, 0, newWidth, newHeight);
	}



private:
	bool initialized;
	Cube arena;

	vector<GLfloat> points;
	vector<GLfloat> normals;
	vector<GLfloat> colors;
	vector<GLuint> elements;

	GLuint shaderProg;

	GLuint positionBuffer;
	GLuint elementBuffer;
	GLuint colorBuffer;
	GLuint normBuffer;
	GLuint vertexArray;

	GLint positionSlot;
	GLint matSlot;
	GLint perspectiveSlot;
	GLint cameraSlot;
	GLint colorSlot;
	GLint normSlot;
	GLint timeSlot;
	GLint pickingSlot;
	GLint idSlot;

	sf::Clock clock;
	
	unsigned int w;
	unsigned int h;
	
	glm::mat4 P;

	void setupGlew()
	{
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			exit(1);
		}
		printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
	}

	void setupShader()
	{
		char const * vertPath = "Shaders/simple.vert";
		char const * fragPath = "Shaders/simple.frag";

		shaderProg = ShaderManager::shaderFromFile(&vertPath, &fragPath, 1, 1);

		// Find out where the shader expects the data
		positionSlot = glGetAttribLocation(shaderProg, "pos");
		matSlot =      glGetUniformLocation(shaderProg, "M");
		perspectiveSlot = glGetUniformLocation(shaderProg, "P");
		cameraSlot = glGetUniformLocation(shaderProg, "C");
		colorSlot = glGetAttribLocation(shaderProg, "color");
		normSlot = glGetAttribLocation(shaderProg, "norm");
		timeSlot = glGetUniformLocation(shaderProg, "time");
		pickingSlot = glGetUniformLocation(shaderProg, "picking");
		idSlot = glGetUniformLocation(shaderProg, "ID");

		checkGLError("shader");
	}

	void setupBuffers()
	{
		arena = Cube();
		points = vector<GLfloat>(arena.vertices);
		normals = vector<GLfloat>(arena.normals);
		colors = vector<GLfloat>(arena.colorFloats);
		elements = vector<GLuint>(arena.triangles);

		//setup the vertex array
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		//setup position buffer
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(points.data()[0]), points.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(positionSlot);
		glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//setup normal Buffer
		printf("Norm Slot: %d\n", normSlot);
		glGenBuffers(1, &normBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normBuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(normals.data()[0]), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(normSlot);
		glVertexAttribPointer(normSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		// Do the same thing for the color data
		printf("Color Slot: %d\n", colorSlot);
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(colors.data()[0]), colors.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(colorSlot);
		glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// now the elements
		glGenBuffers(1, &elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(elements.data()[0]), elements.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
		
		checkGLError("setup");
	}
};

#endif
