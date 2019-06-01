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


int board[8][8] = {
	{5,4,5,4,5,4,5,4},
	{4,5,4,5,4,5,4,5},
	{5,4,5,4,5,4,5,4},
	{4,5,4,5,4,5,4,5},
	{5,4,5,4,5,4,5,4},
	{4,5,4,5,4,5,4,5},
	{5,4,5,4,5,4,5,4},
	{4,5,4,5,4,5,4,5}

	/*{0,2,3,2,3,2,3,1},
	{2,5,4,5,4,5,4,3},
	{3,4,5,4,5,4,5,2},
	{2,5,4,5,4,5,4,3},
	{3,4,5,4,5,4,5,2},
	{2,5,4,5,4,5,4,3},
	{3,4,5,4,5,4,5,2},
	{1,3,2,3,2,3,2,0}*/
};

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

	//surface.readObj("./images/chess_board_2_in.obj", true, true);
	//surface.readObj("./images/chess_board_4_out.obj", true, true);
	// object setting
	
	// Board Object Setting
	GL2_Object** gl_obj=(GL2_Object**)malloc(sizeof(GL2_Object*)*BOARD_SIZE);
	for (int i = 0; i < BOARD_SIZE; i++) {
		gl_obj[i] = (GL2_Object*)malloc(sizeof(GL2_Object)*BOARD_SIZE);
		for (int j = 0; j < BOARD_SIZE; j++) {
			surface.readObj(board_file[board[i][j]].c_str(), true, true);
			surface.scale(0.1);
			
			surface.translate(TV(0.1*i, 0.0, 0.1*j));
			
			gl_obj[i][j] = GL2_Object();
			gl_obj[i][j].initPhongSurface(surface);
			if ((i + j) % 2 == 0) {
				gl_obj[i][j].mat_.setBlue();
			}
			else {
				gl_obj[i][j].mat_.setRed();
			}
		}
	}

	// Piece Object Setting
	//int tmp;
	//obj*** pc_obj = (obj***)malloc(sizeof(obj**)*2);
	//for (int i = 0; i < 2; i++) {
	//	pc_obj[i] = (obj**)malloc(sizeof(obj*)*PIECE_NUM);
	//	for (int j = 0; j < PIECE_NUM; j++) {
	//		switch (j) {
	//		case 0:
	//			tmp = 0; break; // King
	//		case 1:
	//			tmp = 1; break; // Queen
	//		case 2: case 3:
	//			tmp = 2; break; // Bishop
	//		case 4: case 5:
	//			tmp = 3; break; // Knight
	//		case 6: case 7:
	//			tmp = 3; break; // Rook
	//		default:
	//			tmp = 5; break; // Pawn
	//		}
	//		pc_obj[i][j] = new obj(piece_file[tmp].c_str());
	//		pc_obj[i][j]->scale(0.1);
	//		pc_obj[i][j]->translate(glm::vec3(0, 0.05, 0));
	//		pc_obj[i][j]->BindBuffer();
	//		
	//		
	//		/*pc_obj[i][j].translate(glm::vec3(0.1*i, 0.0, 0.1*j));
	//		pc_obj[i][j].updateBuffer();*/
	//		/*surface.readObj(piece_file[tmp].c_str(), true, true);
	//		surface.scale(0.1);

	//		surface.translate(TV(0.0, 0.05, 0.0));*/
	//		
	//		if (i == 0) {
	//			pc_obj[i][j]->mat_.setGold();
	//		}
	//		else {
	//			pc_obj[i][j]->mat_.setRed();
	//		}
	//		
	//	}
	//}
	
	//gl_obj.initPhongSurface(surface);
	//gl_obj.mat_.setRed();
	
	// Board Pieces Pointer
	/*obj*** brd_pc_ptr = (obj***)malloc(sizeof(obj**) * BOARD_SIZE);
	for (int i = 0; i < BOARD_SIZE; i++) {
		brd_pc_ptr[i] = (obj**)malloc(sizeof(obj*)*BOARD_SIZE);
		for (int j = 0; j < BOARD_SIZE; j++) {
			brd_pc_ptr[i][j] = nullptr;
		}
	}

	mapPieces2Board(pc_obj, brd_pc_ptr);*/
	/*obj ob1(piece_file[0].c_str());
	ob1.BindBuffer();*/
	//ob1.translate(glm::vec3(0.1, 0.0, 0.1));

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

		//ob1.applyLighting(light);
		my_chess.applyLighting(light);
		my_chess.drawBoardWithShader(gl_world.shaders_);
		//for (int i = 0; i < BOARD_SIZE; i++) {
		//	for (int j = 0; j < BOARD_SIZE; j++) {
		//		if (brd_pc_ptr[i][j] != nullptr) {
		//			brd_pc_ptr[i][j]->applyLighting(light);
		//			brd_pc_ptr[i][j]->updateBuffer();
		//			brd_pc_ptr[i][j]->drawWithShader(gl_world.shaders_);
		//		}
		//		/*if (brd_pc_ptr[i][j] != nullptr) {
		//		
		//			
		//			brd_pc_ptr[i][j]->applyLighting(light);
		//			brd_pc_ptr[i][j]->drawWithShader(gl_world.shaders_);
		//		}*/
		//	}
		//}
		
		//ob1.drawWithShader(gl_world.shaders_);
		//gl_obj.applyLighting(light);
		//gl_obj.drawWithShader(gl_world.shaders_);
		//gl_obj.drawPhongSurface();

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

