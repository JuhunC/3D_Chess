#pragma once
#include"Piece.h"

class Pointer {
public:
	bool user;
	int x, z;
	obj* ptr_obj;
	Piece* bind_piece= nullptr;
	int prior_x, prior_z;
	
	Pointer(int z, int x, bool is_user);
	void reset(int z, int x, bool is_user); // reset to specific location + 180deg rotate
	
	
	void bind(Piece* piece_ptr);
	void unbind();

	void move(int dx, int dz);
	void change_turn();
	
	void drawPhongSurface() { ptr_obj->drawPhongSurface(); }
	void drawWithShader(const GL2_ShaderProgram& shader) { ptr_obj->drawWithShader(shader); }
	void applyLighting(const GL2_Light& light) { ptr_obj->applyLighting(light); }

	bool is_binded();
	bool is_user() { return user; }

	static Piece* getEmptyPiece() { return new Piece(); }
};
class Chess {
public:
	bool is_user_turn = true;
	Pointer* pointer;

	Chess() {
		if (rand() % 2 == 0) is_user_turn = false;
		else is_user_turn = true;
		Chess::init();
	}
	void pointer_move(int& dx, int& dz) {
		pointer->move(dx, dz);
	}
	void applyLighting(const GL2_Light& light) {
		for (int z = 0; z < BOARD_SIZE; z++) {
			for (int x = 0; x < BOARD_SIZE; x++) {
				if(board[z][x]->getType() != NOT_PIECE)
					board[z][x]->applyLighting(light);
			}
		}
		pointer->applyLighting(light);
	}
	void drawBoardWithShader(const GL2_ShaderProgram& shader) {
		for (int z = 0; z < BOARD_SIZE; z++) {
			for (int x = 0; x < BOARD_SIZE; x++) {
				if (board[z][x]->getType() != NOT_PIECE)
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
	void enter() {
		Piece* ptr_piece = board[pointer->z][pointer->x];
		std::cout << "Enter Pressed" << std::endl;
		if (pointer->is_binded() == true) {
			if (ptr_piece->getType() == NOT_PIECE) { // exchange to an empty space
				pointer->unbind();
			}else if (pointer->bind_piece->isUser() != ptr_piece->isUser()) {
				pointer->unbind();
				//free(board[pointer->z][pointer->x]);
			}
			else if (pointer->prior_x == pointer->x
				&& pointer->prior_z == pointer->z) {
				pointer->unbind();
			}
		}
		else if (pointer->is_binded() == false){
			if (ptr_piece->getType() != NOT_PIECE
				&& ptr_piece->isUser() == pointer->is_user()) {
				std::cout << pointer->x << pointer->z << std::endl;
				pointer->bind(ptr_piece);
			}
		}
	}

	// Init Board
	void Chess::init() {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				board[i][j] = new Piece();
			}
		}
		if (is_user_turn) {
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
				if (board[z][x]->getType() != NOT_PIECE)
					board[z][x]->translate(x, z);
			}
		}
	}
};
Piece::Piece() { // default not a piece
	is_user = false; type = NOT_PIECE; pc_obj = nullptr;
}
Piece::Piece(bool is_user_, int type_) { // set piece
	this->type = type_;
	this->type_ = PC(type);
	this->is_user = is_user_;
	pc_obj = new obj(piece_file[type].c_str());
	pc_obj->scale(Scale[type]);
	pc_obj->translate(glm::vec3(0, trans_up[type], 0));
	pc_obj->BindBuffer();
	if (is_user) pc_obj->mat_.setBlue();
	else		 pc_obj->mat_.setGold();
}
Piece::Piece(bool is_user_, int type_, int x, int z) {
	Piece(is_user_, type_);
	pc_obj->translate(glm::vec3(0.1*x, 0, 0.1*z));
}
void Piece::translate(int dx, int dz) {
	pc_obj->translate(glm::vec3(0.1f*dx, 0, 0.1f*dz));
	pc_obj->updateBuffer();
	pc_obj->BindBuffer();
	this->x += dx;
	this->z += dz;
}

bool Pointer::is_binded() {
	if (bind_piece == nullptr)
		return false;
	else return true;
}
Pointer::Pointer(int z, int x, bool is_user) {
	this->x = x;
	this->z = z;
	ptr_obj = new obj("./images/Pointer.obj");
	ptr_obj->scale(0.1);
	ptr_obj->rotate(4, glm::vec3(1, 0, 0));
	ptr_obj->translate(glm::vec3(0.1*this->x, 0.25, 0.1*this->z));
	ptr_obj->BindBuffer();
	ptr_obj->mat_.setPointer();
	this->user = is_user;
	world_is_user = is_user;
}
void Pointer::reset(int z, int x, bool is_user) {
	ptr_obj->translate(glm::vec3(-0.1*this->x, 0, -0.1*this->z));

	ptr_obj->rotate(180, glm::vec3(0, 1, 0));
	this->x = x;
	this->z = z;
	ptr_obj->translate(glm::vec3(0.1*this->x, 0, 0.1*this->z));
	ptr_obj->BindBuffer();
	ptr_obj->mat_.setPointer();
	this->user = is_user;
	world_is_user = is_user;
}
void Pointer::change_turn() {
	user = !user;
	world_is_user != user;
}
void Pointer::bind(Piece* piece_ptr) {
	if (bind_piece == nullptr) {
		bind_piece = piece_ptr;
		prior_x = bind_piece->x;
		prior_z = bind_piece->z;
		std::cout << "Binded" << std::endl;
	}
}
void Pointer::unbind() {
	if (board[bind_piece->z][bind_piece->x]->isUser() != board[prior_z][prior_x]->isUser()) {
		//if (board[bind_piece->z][bind_piece->x]->getType() == NOT_PIECE
		//	|| board[bind_piece->z][bind_piece->x] == bind_piece) {
		std::swap(board[prior_z][prior_x], board[bind_piece->z][bind_piece->x]);
		free(board[prior_z][prior_x]);
		board[prior_z][prior_x] = getEmptyPiece();
		std::cout << "Unbinded" << std::endl;
		bind_piece = nullptr;
	}
	else if (board[prior_z][prior_x] == bind_piece) {
		std::cout << "Unbinded" << std::endl;
		bind_piece = nullptr;
	}
}
void Pointer::move(int dx, int dz) {
	if (this->x + dx >= 0 && this->x + dx < BOARD_SIZE
		&& this->z + dz >= 0 && this->z + dz < BOARD_SIZE) {
		ptr_obj->translate(glm::vec3(0.1*dx, 0.0, 0.1*dz));
		ptr_obj->BindBuffer();
		this->x += dx; this->z += dz;

		if (bind_piece != nullptr) {
			bind_piece->translate(dx, dz);
		}
	}
}