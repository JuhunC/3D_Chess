#pragma once
#include"include_path.h"
#include"obj.h"
class Piece {
	
	bool is_user;
	int type;
	PC type_;
	obj* pc_obj;
public:
	bool first_move = false;
	int x, z; // board location
	Piece(); //default no piece location
	Piece(bool is_user_, int type_); // piece without location
	Piece(bool is_user_, int type_, int x, int z); // piece with location
	// TODO: Removal
	void destroy() {}

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
	pc_obj = new obj(piece_file[type].c_str());
	pc_obj->scale(Scale[type]);
	pc_obj->translate(glm::vec3(0, trans_up[type], 0));
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