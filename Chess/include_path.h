#pragma once
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
#include "ObjReader.h"
#include "ConventionalMacros.h"

bool world_is_user;
GL2_World gl_world;
int ptr_dx = 0, ptr_dz = 0;
bool enter_pressed = false;
#define BOARD_SIZE 8
#define PIECE_NUM 16 // number of variety of pieces
#define NOT_PIECE -1
const std::string board_file[] = { "./images/chess_board_2_in.obj","./images/chess_board_2_out.obj",
						"./images/chess_board_3_in.obj","./images/chess_board_3_out.obj",
						"./images/chess_board_4_in.obj","./images/chess_board_4_out.obj"
};
const std::string piece_file[] = {
						"./images/pieces/original/King_repaired.obj",
						"./images/pieces/original/Queen_repaired.obj",
						"./images/pieces/original/Bishop_repaired.obj",
						"./images/pieces/original/Knight_repaired.obj",
						"./images/pieces/original/Rook_repaired.obj",
						"./images/pieces/original/Pawn_repaired.obj"
};
enum PC { King, Queen, Bishop, Knight, Rook, Pawn };
float Scale[] = { 0.15,	0.15,	0.13,	0.13,	0.13,	0.1 };
float trans_up[] = { 0.08,	0.08,	0.07,	0.07,	0.07,	0.05 };

void printMat4(glm::mat4 mat) {
	for (int i = 0; i < mat.length(); i++) {
		for (int j = 0; j < mat[i].length(); j++) {
			std::cout << mat[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}
void setUserViewPoint(int& width, int& height) {

	const float zNear = 0.001, zFar = 100.0, fov = 45.0;			// UI
	gl_world.camera_.Resize(width, height, fov, zNear, zFar);

	gl_world.camera_.projection_ *= glm::translate(glm::vec3(-0.35f, -0.7f, +0.0f));
	gl_world.camera_.projection_ *= glm::rotate(0.6f, glm::vec3(1.0f, 0.0f, 0.0f));
	gl_world.initShaders();
}
void setPCViewPoint(int& width, int& height) {
	const float zNear = 0.001, zFar = 100.0, fov = 45.0;			// UI
	gl_world.camera_.Resize(width, height, fov, zNear, zFar);

	gl_world.camera_.projection_ *= glm::translate(glm::vec3(-0.35f, -0.7f, +0.0f));
	gl_world.camera_.projection_ *= glm::rotate(0.6f, glm::vec3(1.0f, 0.0f, 0.0f));
	gl_world.initShaders();
}
void changeTurn(bool is_user, int& width, int& height) {
	if (is_user) {
		setUserViewPoint(width, height);
	}
	else {
		setPCViewPoint(width, height);
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	gl_world.camera_.UpdateDolly(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		gl_world.camera_.StartMouseRotation(xpos, ypos);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		gl_world.camera_.EndMouseRotation(xpos, ypos);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		gl_world.camera_.StartMousePan(xpos, ypos);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		gl_world.camera_.EndMousePan(xpos, ypos);
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//std::cout << xpos << " " << ypos << std::endl;

	gl_world.camera_.ProcessMouseMotion(xpos, ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	int vec = 1.0;
	if (!world_is_user) vec *= -1.0;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		ptr_dz = 1 * vec;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		ptr_dz = -1 * vec;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		ptr_dx = 1 * vec;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		ptr_dx = -1 * vec;
	}
	else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		enter_pressed = true;
	}
}
