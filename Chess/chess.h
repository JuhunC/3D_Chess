#pragma once
#include"Pointer.h"

class Chess {
public:
	Pointer* pointer;

	Chess() {
		Chess::init();
	}
	void pointer_move(int& dx, int& dz) {
		pointer->move(dx, dz);
	}
	void applyLighting(const GL2_Light& light) {
		for (int z = 0; z < BOARD_SIZE; z++) {
			for (int x = 0; x < BOARD_SIZE; x++) {
				if(board[z][x]!= nullptr)
					board[z][x]->applyLighting(light);
			}
		}
		pointer->applyLighting(light);
	}
	void drawBoardWithShader(const GL2_ShaderProgram& shader) {
		for (int z = 0; z < BOARD_SIZE; z++) {
			for (int x = 0; x < BOARD_SIZE; x++) {
				if (board[z][x]!= nullptr)
					board[z][x]->drawWithShader(shader);
			}
		}
		pointer->drawWithShader(shader);
	}
	void drawBoardWithPhongSurface() {
		for (int z = 0; z < BOARD_SIZE; z++) {
			for (int x = 0; x < BOARD_SIZE; x++) {
				if (board[z][x]->getType() != NOT_PIECE)
					board[z][x]->drawPhongSurface();
			}
		}
		pointer->drawPhongSurface();
	}
	void enter(); // enter action
	void init();
};
void Chess::enter() {
	if (pointer->is_binded() == true) {
		pointer->try_unbind();
	}
	else {
		pointer->try_bind();
	}
	//Piece* ptr_piece = board[pointer->z][pointer->x];
	//std::cout << "Enter Pressed" << std::endl;
	//if (pointer->is_binded() == true) {
	//	if (ptr_piece->getType() == NOT_PIECE) { // exchange to an empty space
	//		pointer->unbind();
	//	}
	//	else if (pointer->bind_piece->isUser() != ptr_piece->isUser()) {
	//		pointer->unbind();
	//		//free(board[pointer->z][pointer->x]);
	//	}
	//	else if (pointer->prior_x == pointer->x
	//		&& pointer->prior_z == pointer->z) {
	//		pointer->unbind();
	//	}
	//}
	//else if (pointer->is_binded() == false) {
	//	if (ptr_piece->getType() != NOT_PIECE
	//		&& ptr_piece->isUser() == pointer->is_user()) {
	//		std::cout << pointer->x << pointer->z << std::endl;
	//		pointer->bind(ptr_piece);
	//	}
	//}
}
// Init Board
void Chess::init() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = nullptr;
		}
	}
	if (rand()%2==1) {
		pointer = new Pointer(0, 4, true);
	}
	else {
		pointer = new Pointer(3, 7, false);
	}
	board[0][0] = new Piece(true, PC::Rook);
	board[0][1] = new Piece(true, PC::Knight);
	board[0][2] = new Piece(true, PC::Bishop);
	board[0][3] = new Piece(true, PC::King);
	board[0][4] = new Piece(true, PC::Queen);
	board[0][5] = new Piece(true, PC::Bishop);
	board[0][6] = new Piece(true, PC::Knight);
	board[0][7] = new Piece(true, PC::Rook);
	for (int i = 0; i < BOARD_SIZE; i++) {
		board[1][i] = new Piece(true, PC::Pawn);
	}
	board[7][0] = new Piece(false, PC::Rook);
	board[7][1] = new Piece(false, PC::Knight);
	board[7][2] = new Piece(false, PC::Bishop);
	board[7][3] = new Piece(false, PC::King);
	board[7][4] = new Piece(false, PC::Queen);
	board[7][5] = new Piece(false, PC::Bishop);
	board[7][6] = new Piece(false, PC::Knight);
	board[7][7] = new Piece(false, PC::Rook);
	for (int i = 0; i < BOARD_SIZE; i++) {
		board[6][i] = new Piece(false, PC::Pawn);
	}
	for (int z = 0; z < BOARD_SIZE; z++) {
		for (int x = 0; x < BOARD_SIZE; x++) {
			if (board[z][x] != nullptr)
				board[z][x]->translate(x, z);
		}
	}
}

