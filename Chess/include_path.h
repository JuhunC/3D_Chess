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
// sound lib
#include <Windows.h>
#include <mmsystem.h>
#include<mciapi.h>

#include<thread>
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
#pragma comment(lib, "Winmm.lib")

bool is_animal = false;

void playDyingSound() {
	PlaySound("./sound/pacman_dying_sound.wav", NULL, NULL);
	if (is_animal == false)
		PlaySound("./sound/stadium_sound.wav", NULL, SND_LOOP | SND_ASYNC);
	else PlaySound("./sound/farm_sound.wav", NULL, SND_LOOP | SND_ASYNC);
}

bool world_is_user;
GL2_World gl_world;
GLFWwindow *window = nullptr;
int ptr_dx = 0, ptr_dz = 0;
bool enter_pressed = false;
bool cancel_action = false;
bool global_game_over = false;

#define BOARD_SIZE 8
#define PIECE_NUM 16 // number of variety of pieces
#define NOT_PIECE -1
#define GAME_ERROR -1
#define GAME_NOT_OVER 1
#define GAME_OVER_USER_WON 2
#define GAME_OVER_PC_WON 3
#define VOLUME_OFF 0
#define VOLUME_LOW 858993459
#define VOLUME_NORMAL 1717986918
#define VOLUME_MEDIUM -1717986919
#define VOLUME_HIGH -858993460
#define VOLUME_VERY_HIGH -1
#define NUM_PARTICLES 100

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
const std::string piece_file_animal[] = {
						"./images/pieces/Chess_-_Lion_King_1.obj",
						"./images/pieces/Chess_-_Lioness_Queen.obj",
						"./images/pieces/Chess_-_Seal_Bishop.obj",
						"./images/pieces/Chess_-_Horse_1.obj",
						"./images/pieces/Chess_-_Poodle_Rook.obj",
						"./images/pieces/Chess_-_Penguin_Pawn_1.obj"
};
enum PC { King, Queen, Bishop, Knight, Rook, Pawn };
float Scale[] = { 1.3,	1.3,	1.13,	1.13,	1.13,	1.1 };
float trans_up[] = { 0.7,	0.7,	0.6,	0.6,	0.6,	0.5 };

float Scale_animal[] = { 4.0,	3.0,	1.5,	1.13,	2.0,	1.1 };
float trans_up_animal[] = { 0.7,	0.7,	0.6,	0.6,	0.6,	0.5 };
float trans_front_animal[] = { 1.7, 1.3, 0.4, 0.1, 0.7, 0 };

int getUserVec(bool isuser); // get direction by the user
void setUserViewPoint();					// reset VIEW to USER
void setPCViewPoint();						// reset VIEW to PC
void changeTurn_VIEW(bool is_user);	// change VIEW by turn

void printMat4(glm::mat4 mat); // print mat to console

// callbacks
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


void setUserViewPoint() {
	gl_world.camera_.USR_VIEW();
}
void setPCViewPoint() {
	gl_world.camera_.PC_VIEW();
}
void changeTurn_VIEW(bool is_user) {
	if (is_user) {
		setUserViewPoint();
	}
	else {
		setPCViewPoint();
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
	int vec = getUserVec(world_is_user);
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
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		std::cout << "ESC" << std::endl;
		cancel_action = true;
	}
}

int getUserVec(bool isuser) {
	if (isuser == true) return 1;
	else return -1;
}
void printMat4(glm::mat4 mat) {
	for (int i = 0; i < mat.length(); i++) {
		for (int j = 0; j < mat[i].length(); j++) {
			std::cout << mat[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}
int min(int x, int z) {
	if (x > z) return z;
	else return x;
}
int max(int x, int z) {
	if (x > z) return x;
	else return z;
}