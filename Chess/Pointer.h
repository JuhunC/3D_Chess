#pragma once
#include"Piece.h"

Piece* board[BOARD_SIZE][BOARD_SIZE]; // z by x
void printBoard() {
	for (int z = 0; z < BOARD_SIZE; z++) {
		for (int x = 0; x < BOARD_SIZE; x++) {
			if (board[z][x] == nullptr) {
				std::cout << 9 << "\t";
			}
			else {
				std::cout << board[z][x]->getType() << "\t";
			}
		}
		std::cout << std::endl;
	}
}
void destroySelect(bool isUser) {
	for (int x = 0; x < BOARD_SIZE; x++) {
		for (int z = 0; z < BOARD_SIZE; z++) {
			if (board[z][x] != nullptr) {
				if (board[z][x]->isUser() == isUser) {
					(dest_pieces)[dest_num] = board[z][x]->destroy();
					dest_num++;
					board[z][x] = nullptr;
				}
			}
		}
	}
}
int isGameOver() {
	bool king_exist_usr = false;
	bool king_exist_pc = false;
	for (int x = 0; x < BOARD_SIZE; x++) {
		for (int z = 0; z < BOARD_SIZE; z++) {
			if (board[z][x] != nullptr) {
				if (board[z][x]->getPCType() == King) {
					if (board[z][x]->isUser() == true) {
						//std::cout << "USER KING " << z << " " << x << " " << std::endl;
						king_exist_usr = true;
					}
					else if (board[z][x]->isUser() == false) {
						//std::cout << "PC KING " << z << " " << x << " " << std::endl;
						king_exist_pc = true;
					}
				}
			}
		}
	}

	if (king_exist_usr == false) {
		destroySelect(true);
		return GAME_OVER_PC_WON;
	}
	else if (king_exist_pc == false) {
		destroySelect(false);
		return GAME_OVER_USER_WON;
	}
	return GAME_NOT_OVER;
}
bool check_path_clear_cross(int& p_z, int& p_x, int&z, int&x, bool& turn) {
	int x_ = p_x, z_ = p_z;
	int dx=1, dz=1;
	if (x - p_x < 0) dx = -1;
	if (z - p_z < 0) dz = -1;
	bool clear = true;
	for (; x_ != x, z_ != z; x_ += dx, z_ += dz) {
		if (x_ == x && z_ == z) {}
		else if (x_ == p_x && z_ == p_z) {}
		else if (board[z_][x_] != nullptr) {
			clear = false;
			break;
		}
	}
	return clear;
}
bool check_path_clear(int& p_z, int& p_x, int& z, int& x) {
	int start_z = min(p_z, z);
	int end_z = max(p_z, z);
	int start_x = min(p_x, x);
	int end_x = max(p_x, x);
	bool clear = true;
	for (int z_ = start_z; z_ <= end_z; z_++) {
		for (int x_ = start_x; x_ <= end_x; x_++) {
			if (z_ == p_z && x_ == p_x) {
			}
			else if (z_ == z && x_ == x) {
			}
			else if (board[z_][x_] != nullptr) {
				clear = false;
				break;
			}
		}
	}
	return clear;
}

class Pointer {
public:
	bool* user = &world_is_user;
	int x, z;
	obj* ptr_obj;
	Piece* bind_piece = nullptr;
	int prior_x, prior_z;

	Pointer(int z, int x, bool is_user);
	void reset(int z, int x, bool is_user); // reset to specific location + 180deg rotate

	bool try_bind();
	bool try_unbind();
	void bind();
	void unbind();

	bool check_oponent_piece(int z, int x, bool isUser);
	bool check_moveable_loc(int&z, int&x, PC piece_type);

	void move(int dx, int dz);
	void change_turn();
	void cancel_action();

	void drawPhongSurface() { ptr_obj->drawPhongSurface(); }
	void drawWithShader(const GL2_ShaderProgram& shader) { ptr_obj->drawWithShader(shader); }
	void applyLighting(const GL2_Light& light) { ptr_obj->applyLighting(light); }

	bool is_binded();
	bool is_user() { return *user; }

	static Piece* getEmptyPiece() { return new Piece(); }
};
// Cancel if binded
void Pointer::cancel_action() {
	if (is_binded() == true) {
		this->move(prior_x - x, prior_z - z);
		board[z][x] = bind_piece;
		bind_piece = nullptr;
		this->user != this->user;
	}
	changeTurn_VIEW(is_user());
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
	ptr_obj->scale(0.8);
	ptr_obj->rotate(4, glm::vec3(1, 0, 0));
	ptr_obj->translate(glm::vec3(1.0f*this->x, 2 ,1.0f*this->z));
	ptr_obj->BindBuffer();
	ptr_obj->mat_.setPointer();
	*this->user = is_user;
	world_is_user = is_user;
}
void Pointer::reset(int z, int x, bool is_user) {
	ptr_obj->translate(glm::vec3(-1.0f*this->x, 0, -1.0f*this->z));
	ptr_obj->rotate(180, glm::vec3(0, 0, 1));
	this->x = x;
	this->z = z;
	ptr_obj->translate(glm::vec3(1.0f*this->x, 0, 1.0f*this->z));
	ptr_obj->BindBuffer();
	ptr_obj->mat_.setPointer();
	*this->user = is_user;
	world_is_user = is_user;
}
void Pointer::change_turn() {
	ptr_obj->translate(glm::vec3(-1.0f*this->x, 0, -1.0f*this->z));
	ptr_obj->rotate(180, glm::vec3(0, 1, 0));
	ptr_obj->translate(glm::vec3(1.0f*this->x, 0, 1.0f*this->z));
	ptr_obj->BindBuffer();
	*user = !*user;
	if (*user) { std::cout << "User Turn" << std::endl; }
	else std::cout << "PC Turn" << std::endl;
	bind_piece == nullptr;
	changeTurn_VIEW(*user);
}
void Pointer::bind(){
	bind_piece = board[z][x];
	prior_z = z;
	prior_x = x;
}
void Pointer::unbind() {
	board[z][x] = bind_piece;
	board[prior_z][prior_x] = nullptr;
	bind_piece = nullptr;
}
void Pointer::move(int dx, int dz) {
	if (this->x + dx >= 0 && this->x + dx < BOARD_SIZE
		&& this->z + dz >= 0 && this->z + dz < BOARD_SIZE) {
		ptr_obj->translate(glm::vec3(1.0*dx, 0.0, 1.0*dz));
		ptr_obj->BindBuffer();
		this->x += dx; this->z += dz;

		if (bind_piece != nullptr) {
			bind_piece->translate(dx, dz);
		}
	}
}
bool Pointer::check_oponent_piece(int z, int x, bool isUser) {
	if (board[z][x] != nullptr) {
		if (board[z][x]->isUser() == isUser)
			return false;
		else {
			(dest_pieces)[dest_num] = board[z][x]->destroy();
			dest_num++;
			
			return true;
		}
	}
	return true;
}
bool Pointer::check_moveable_loc(int&z, int&x, PC pc_type) {
	// TODO: jumping not solved;
	switch (pc_type) {
	case King:
		if (std::abs(z - prior_z) <= 1
			&& std::abs(x-prior_x) <= 1) {
			if (board[z][x] == nullptr)
				return true;
			else if (board[z][x]->isUser() != world_is_user)
				return true;
		}break;
	case Queen:
		if (z == prior_z // horizontal movement
			|| x == prior_x) {
			if(check_path_clear(prior_z, prior_x, z,x))
				return true;
		}
		else if (std::abs(z - prior_z) == std::abs(x - prior_x)) {
			if (check_path_clear_cross(prior_z, prior_x, z, x, world_is_user))
				return true;
		}break;
	case Bishop:
		if (std::abs(z - prior_z) == std::abs(x - prior_x)) {
			if (check_path_clear_cross(prior_z, prior_x, z, x, world_is_user))
				return true;
		}break;
	case Knight:
		if (std::abs(z - prior_z) + std::abs(x - prior_x) == 3) {
			return true;
		}
		break;
	case Rook:
		if (z == prior_z || x == prior_x){
			if (check_path_clear(prior_z, prior_x, z,x))
				return true;
		}
		break;
	case Pawn:
		if (this->bind_piece->first_move == true && // first move : move two block
			(z-prior_z) == 2*getUserVec(*user)
			&& board[z][x] == nullptr){
			this->bind_piece->first_move = false;
			return true;
		}
		else if ((z - prior_z) == 1 * getUserVec(*user) 
			&& (x-prior_x) == 0
			&& board[z][x] == nullptr) { // move one block
			if(this->bind_piece->first_move == true) // erase first move
				this->bind_piece->first_move = false;
			return true;
		}
		else if ((z - prior_z) == 1 * getUserVec(*user)
			&& std::abs(x - prior_x) == 1
			&& board[z][x] != nullptr
			&& board[z][x]->isUser() != *user) {
			if (this->bind_piece->first_move == true) // erase first move
				this->bind_piece->first_move = false;
			return true;
		}
		else {
			std::cout << "[" << z << "," << x << "] : not reacheable" << std::endl;
		}
		break;
	default:
		std::cout << "Error : Pointer::check_moveable_loc" << std::endl;
		exit(-1);
	}
	return false;
}
bool Pointer::try_unbind() {
	if (is_binded() == true) {
		if (z == prior_z && x == prior_x) { // same place - undo whatever
			cancel_action();
			return true;
		}else if (check_moveable_loc(z,x, bind_piece->getPCType())==true
			&& check_oponent_piece(z,x, bind_piece->isUser())==true) {
			unbind();
			change_turn();
		}
		else {
			std::cout << "Unable to unbind : that piece cannot go there" << std::endl;
			return false;
		}
	}
	else {
		std::cout << "Unable to unbind" << std::endl;
		return false;
	}
	return true;
}
bool Pointer::try_bind() {
	if (is_binded() == false){	// must not be binded
		if (board[z][x] != nullptr) {
			if (board[z][x]->isUser() == is_user()) {
				bind();
				return true;
			}
			else {
				std::cout << x << "\t" << z << "\t" << board[z][x]->isUser() << std::endl;
				std::cout << "Unable to bind : piece not owned" << std::endl;
			}
		}
		else {
			std::cout << "Unable to bind : board not null(TODO: Change vs PC)" << std::endl;
		}
	}
	else {
		std::cout << "Unable to bind : not yet binded" << std::endl;
		return false;
	}
	return true;
}