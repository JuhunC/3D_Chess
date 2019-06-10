#pragma once
#include"Piece.h"
Piece* board[BOARD_SIZE][BOARD_SIZE]; // z by x

class Pointer {
public:
	bool user;
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

	void drawPhongSurface() { ptr_obj->drawPhongSurface(); }
	void drawWithShader(const GL2_ShaderProgram& shader) { ptr_obj->drawWithShader(shader); }
	void applyLighting(const GL2_Light& light) { ptr_obj->applyLighting(light); }

	bool is_binded();
	bool is_user() { return user; }

	static Piece* getEmptyPiece() { return new Piece(); }
};
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
	this->user = is_user;
	world_is_user = is_user;
}
void Pointer::reset(int z, int x, bool is_user) {
	ptr_obj->translate(glm::vec3(-1.0f*this->x, 0, -1.0f*this->z));

	ptr_obj->rotate(180, glm::vec3(0, 1, 0));
	this->x = x;
	this->z = z;
	ptr_obj->translate(glm::vec3(1.0f*this->x, 0, 1.0f*this->z));
	ptr_obj->BindBuffer();
	ptr_obj->mat_.setPointer();
	this->user = is_user;
	world_is_user = is_user;
}
void Pointer::change_turn() {
	user = !user;
	world_is_user != user;
}
void Pointer::bind(){
	bind_piece = board[z][x];
}
void Pointer::unbind() {
	
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
			return true;
		}
		else if (std::abs(z - prior_z) == std::abs(x - prior_x)) {
			return true;
		}break;
	case Bishop:
		if (std::abs(z - prior_z) == std::abs(x - prior_x)) {
			return true;
		}break;
	case Knight:
		if (std::abs(z - prior_z) + std::abs(x - prior_x) == 3) {
			return true;
		}
		break;
	case Rook:
		if (z == prior_z || x == prior_x) return true;
		break;
	case Pawn:
		if (z - prior_z == 1 * getUserVec(is_user()))
			return true;
		break;
	default:
		std::cout << "Error : Pointer::check_moveable_loc" << std::endl;
		exit(-1);
	}
	return false;
}
bool Pointer::try_unbind() {
	if (is_binded() == true) {
		if (check_moveable_loc(z,x, bind_piece->getPCType())==true) {
			unbind();
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
}
bool Pointer::try_bind() {
	if (is_binded() == false	// must not be binded
		&& board[z][x]->isUser() == is_user()) { 
		bind();
		return true;
	}
	else {
		std::cout << "Unable to bind" << std::endl;
		return false;
	}
}