#pragma once
#include"include_path.h"
#include"obj.h"

class Piece {
	bool is_user;
	int type;
	PC type_;
	obj* pc_obj;
public:
	
	bool destroyed = false;
	int destroy_cnt = 0;
	bool first_move = true;
	int x, z; // board location
	Piece(); //default no piece location
	Piece(bool is_user_, int type_); // piece without location
	Piece(bool is_user_, int type_, int x, int z); // piece with location
	void update() {
		if (destroyed == true && destroy_cnt < 130) {
			pc_obj->translate(glm::vec3(0, -0.01f, 0));
			pc_obj->BindBuffer();
			destroy_cnt++;
		}
		else {
			if (destroyed == true) {
				free(pc_obj);
				destroyed = false;
			}
		}
	}
	// Removal
	Piece* destroy() {
		destroyed = true;
		return this;
	}

	// Object Related Functions
	void drawPhongSurface() { pc_obj->drawPhongSurface(); }
	void drawWithShader(const GL2_ShaderProgram& shader) { pc_obj->drawWithShader(shader); }
	void applyLighting(const GL2_Light& light) { pc_obj->applyLighting(light); }
	void translate(int dx, int dz); // move piece x,z * 0.1(rate)
	int getZdirection() { if (is_user) return 1;	else return -1; } // get direction for movement

	// Getter/Setter function
	bool isUser() { return is_user; }
	int getType() { return type; }
	PC getPCType() { return type_; }
	void setType(int type_) { this->type = type_; this->type_ = PC(type_); }
	void setType(PC type_) { this->type_ = type_; this->type = PC(type_); }
	obj* getObjPtr() { return pc_obj; }
};
Piece::Piece() { // default not a piece
	is_user = false; type = NOT_PIECE; pc_obj = nullptr;
}
Piece::Piece(bool is_user_, int type_) { // set piece
	this->type = type_;
	this->type_ = PC(type);
	this->is_user = is_user_;
	if (is_animal == false) {
		pc_obj = new obj(piece_file[type].c_str());
	}
	else if (is_animal == true) {
		pc_obj = new obj(piece_file_animal[type].c_str());
	}
	if (is_animal == false) {
		pc_obj->scale(Scale[type]);
		pc_obj->translate(glm::vec3(0, trans_up[type], 0));
	}
	else if (is_animal == true) {
		if (this->is_user == false)
			pc_obj->rotate(180, glm::vec3(0, 1, 0));
		pc_obj->scale(Scale_animal[type]);
		if (this->is_user == false) {
			pc_obj->translate(glm::vec3(0, trans_up_animal[type], 0));
			pc_obj->translate(glm::vec3(0, 0, -1 * trans_front_animal[type]));
		}
		else {
			pc_obj->translate(glm::vec3(0, trans_up_animal[type], 0));
			pc_obj->translate(glm::vec3(0, 0, trans_front_animal[type]));
		}
		
	}
	
	pc_obj->BindBuffer();
	if (is_user) pc_obj->mat_.setBlue();
	else		 pc_obj->mat_.setGold();
}
Piece::Piece(bool is_user_, int type_, int x, int z) {
	Piece(is_user_, type_);
	pc_obj->translate(glm::vec3(1.0*x, 0, 1.0*z));
}
void Piece::translate(int dx, int dz) {
	pc_obj->translate(glm::vec3(1.0f*dx, 0, 1.0f*dz));
	pc_obj->updateBuffer();
	pc_obj->BindBuffer();
	this->x += dx;
	this->z += dz;
}
std::vector<Piece*> dest_pieces;
int dest_num;