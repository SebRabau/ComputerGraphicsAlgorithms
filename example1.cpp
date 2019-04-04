// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <iostream>
#include <vector>
#include "common/shaders.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include "common/controls.hpp"
#include <glm/gtx/transform.hpp>
#include <cmath>

#include "source\example1\view.h"


int main(void)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    int width = 1920;
    int height = 1080;
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow(width, height, "Basic Example", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    //we set the cursor at the centre so that it always start on our origin (0,0,0)
    glfwSetCursorPos(window, width / 2, height / 2);
    
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

	//Instantiante MVC
	View* view = new View();
    
	//Generate Triangle Hard-Coded
	float zero = 0.0f;
	float size = 1.0f;
	float sheight = (size / 2)*tan(1.0472);
	float theight = ((2*sheight)/ 3)*tan(1.0472);
	float centre = sheight / 3;

	

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec2> UVcolours;
	std::vector<glm::vec3> LNormals;

	//bool player = loadOBJ("sphere.obj", Vertices, UVcolours, LNormals);
	bool object = loadAssImp("car.obj", indices, Vertices, UVcolours, LNormals);


	std::cout << "Loaded" << std::endl;
	std::cout << "Vertices: " << Vertices.size() << " UVs: " << UVcolours.size() << " Normals: " << LNormals.size() << std::endl;
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint VBuff;
	glGenBuffers(1, &VBuff);
	glBindBuffer(GL_ARRAY_BUFFER, VBuff);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size()*sizeof(glm::vec3), &Vertices[0], GL_STATIC_DRAW);

	std::vector<glm::vec3> colours;

	for (int i = 0; i < Vertices.size(); i++) {

		colours.push_back(glm::vec3(1.0, 0.0, 0.0)); //Change vec3 to change colour

	}

	GLuint CBuff;
	glGenBuffers(1, &CBuff);
	glBindBuffer(GL_ARRAY_BUFFER, CBuff);
	glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(glm::vec3), &colours[0], GL_STATIC_DRAW);

	GLuint NBuff;
	glGenBuffers(1, &NBuff);
	glBindBuffer(GL_ARRAY_BUFFER, NBuff);
	glBufferData(GL_ARRAY_BUFFER, LNormals.size() * sizeof(glm::vec3), &LNormals[0], GL_STATIC_DRAW);

	std::cout << "Loading Shaders" << std::endl;
    
    //GLuint programID = LoadShaders("LightVertexShader.glsl", "LightFragmentShader.glsl");
	GLuint programID = LoadShaders("ColorsGeometryShader.glsl", "ColorsVertexShader.glsl", "ColorsFragmentShader.glsl");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewID = glGetUniformLocation(programID, "V");
	GLuint ModelID = glGetUniformLocation(programID, "M");
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint CameraID = glGetUniformLocation(programID, "CameraPosition_worldspace");

    do {
        // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        
        glUseProgram(programID);
		computeMatricesFromInputs(window, width, height);

		//********Calculate the MVP matrix
		//***********PROJECTION*****************

		glm::mat4 Projection = getProjectionMatrix();

		glm::vec3 cameraPosition(0, 5, -5);
		
		// Camera matrix
		glm::mat4 View = glm::lookAt(
			cameraPosition, // Camera is at (0,0,-1), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
				
		glUniform3f(CameraID, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		//***********MODEL*****************
		glm::mat4 Model = glm::mat4(1.0);

		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);

		glm::vec3 lightPosition(5, 5, 5);
		glUniform3f(LightID, lightPosition.x, lightPosition.y, lightPosition.z);


		//***********DRAWING*****************
		view->draw(0, 0, width/2, height, &VBuff, Vertices.size(), &CBuff, &NBuff);

		view->draw(800, 0, width/2, height, &VBuff, Vertices.size(), &CBuff, &NBuff);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &VBuff);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}


