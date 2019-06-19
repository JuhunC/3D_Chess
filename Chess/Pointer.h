#pragma once
#include"Piece.h"
Piece* board[BOARD_SIZE][BOARD_SIZE]; // z by x
bool chech_path_clear_cross(int& p_z, int& p_x, int&z, int&x) {
	int start_z = min(p_z, z);
	int end_z = max(p_z, z);
	int start_x = min(p_x, x);
	int end_x = max(p_x, x);
	bool clear = true;
	std::cout << start_z << end_z << start_x << end_x << std::endl;
	for (int i = 1; i < std::abs(end_z-start_z); i++) {
		std::cout << start_z+i<< end_z+i << std::endl;
		if (board[start_z + i][start_x + i] != nullptr) {
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
	std::cout << start_z << end_z << start_x << end_x << std::endl;
	for (int z_ = start_z; z_ <= end_z; z_++) {
		for (int x_ = start_x; x_ <= end_x; x_++) {
			std::cout << z_ << x_ << std::endl;
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
bool Pointer::check_moveable_loc(int&z, int&x, PC pc_type) {
	// TODO: jumping not solved;
	switch (pc_type) {
	case King:
		if (std::abs(z - prior_z) <= 1
			&& std::abs(x-prior_x) <= 1) {
			return true;
		}break;
	case Queen:
		if (z == prior_z // horizontal movement
			|| x == prior_x) {
			if(check_path_clear(prior_z, prior_x, z,x))
				return true;
		}
		else if (std::abs(z - prior_z) == std::abs(x - prior_x)) {
			if (chech_path_clear_cross(prior_z, prior_x, z, x))
				return true;
		}break;
	case Bishop:
		if (std::abs(z - prior_z) == std::abs(x - prior_x)) {
			if (chech_path_clear_cross(prior_z, prior_x, z, x))
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
		if (this->bind_piece->first_move == true &&
			(z-prior_z) == 2*getUserVec(*user)){
			return true;
		}else if ((z - prior_z) == 1 * getUserVec(*user))
			return true;
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
		if (z == prior_z && x == prior_x) {
			cancel_action();
			return true;
		}else if (check_moveable_loc(z,x, bind_piece->getPCType())==true) {
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