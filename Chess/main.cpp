#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>
#include <GL/freeglut.h>
#include <cstring>
#include <stdlib.h>		  // srand, rand
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iostream>
#include "math.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include "Geometry/TriangularSurface/StaticTriangularSurface.h"
#include "GL2/GL2_Object.h"
#include "GL2/GL2_World.h"
#include "GL2/GL2_Light.h"
#include "GL2/GL2_Material.h"
#include "ParticleSystem.h"
#include "callback.h"
#include "chess.h"
#include "ObjReader.h"

#define NUM_PARTICLES 1000
int width_window = 640;
int height_window = 480;

int ptr_dx=0, ptr_dz = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		ptr_dz = 1;
	}else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
		ptr_dz = -1;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		ptr_dx = 1;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		ptr_dx = -1;
	}
}


int main(int argc, char *argv[])
{
	GLFWwindow *window = nullptr;

	/* Initialize the library */
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_SAMPLES, 32);

	// window resolution
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width_window = mode->width * 0.8f;
	height_window = mode->height * 0.8f;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width_window, height_window, "Hello World", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	// callbacks here
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glClearColor(0, 0, 0, 1); // white background

	printf("%s\n", glGetString(GL_VERSION));

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	const float aspect = (float)width / (float)height;
	glViewport(0, 0, width, height);
	glOrtho(-1.2*aspect, 1.2*aspect, -1.2, 1.2, -100.0, 100.0);
	gluLookAt(0.1, 0.1, 0.1, 0.0, 0.0, 0.0, 0, 1, 0);//camera takes care of it

	const float zNear = 0.001, zFar = 100.0, fov = 45.0;			// UI
	gl_world.camera_.Resize(width, height, fov, zNear, zFar);
	gl_world.initShaders();

	//	glEnable(GL_MULTISAMPLE);

	// particle system init
	ParticleSystem ps;
	ps.initParticleSystem(NUM_PARTICLES);



	StaticTriangularSurface surface;

	
	// Board Object Setting
	GL2_Object** gl_obj=(GL2_Object**)malloc(sizeof(GL2_Object*)*BOARD_SIZE);
	for (int i = 0; i < BOARD_SIZE; i++) {
		gl_obj[i] = (GL2_Object*)malloc(sizeof(GL2_Object)*BOARD_SIZE);
		for (int j = 0; j < BOARD_SIZE; j++) {
			if((i+j)%2 ==0)
				surface.readObj(board_file[4].c_str(), true, true);
			else
				surface.readObj(board_file[5].c_str(), true, true);
			surface.scale(0.1);
			
			surface.translate(TV(0.1*i, 0.0, 0.1*j));
			
			gl_obj[i][j] = GL2_Object();
			gl_obj[i][j].initPhongSurface(surface);
			if ((i + j) % 2 == 0) {
				gl_obj[i][j].mat_.setBoard1();
			}
			else {
				gl_obj[i][j].mat_.setBoard2();
			}
		}
	}

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_COLOR_MATERIAL);

	glLoadIdentity();
	Chess my_chess;
	GL2_Light light;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		gl_world.camera_.ContinueRotation();

		glm::mat4 vp = gl_world.camera_.GetWorldViewMatrix();

		// update particle system
		ps.advanceOneTimeStep(0.001);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(gl_world.shaders_.program_id_);

		gl_world.shaders_.sendUniform(vp, "mvp");

		//gl_obj.drawPhongSurface();
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				gl_obj[i][j].applyLighting(light);
				gl_obj[i][j].drawWithShader(gl_world.shaders_);
			}
		}

		// Check Pointer Movement
		my_chess.pointer_move(ptr_dx, ptr_dz);
		ptr_dx = 0; ptr_dz = 0;

		// Draw Chess Pieces
		my_chess.applyLighting(light);
		my_chess.drawBoardWithShader(gl_world.shaders_);

		glUseProgram(0);
		// Old-style rendering
		glLoadMatrixf(&vp[0][0]);

		glLineWidth(2);
		gl_world.drawAxes();
		glDisable(GL_LIGHTING);

		// draw particles
		// old version
		
		//ob1.updateBuffer();
		
		for (int p = 0; p < ps.particles.size(); p++) {
			const TV red = TV(1.0f, 0.0f, 0.0f);
			const TV blue = TV(0.0f, 1.0f, 0.0f);
			
			TV& pos(ps.particles[p].pos);
			TV& vel(ps.particles[p].vel);

			const float alpha = vel.getMagnitude() * 0.5;

			const TV blend_color = alpha * red + (1 - alpha) * blue;
			
			glPointSize(ps.particles[p].mass);
			glBegin(GL_POINTS);
			glColor3fv(blend_color.values_);
			glVertex3fv(ps.particles[p].pos.values_);
			glEnd();
		}
		
		



		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	glfwTerminate();

	return 0;
}

