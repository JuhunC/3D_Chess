#pragma once
#include"include_path.h"
#include"BMPImage.h"
class obj {
public:
	const char* file = NULL;
	GLuint vbo[3];
	GLuint textures[2]; // image over the obj
	float mat_ambient[4] = { 0.1745f, 0.01175f, 0.01175f, 0.55f };
	float mat_diffuse[4] = { 0.61424f, 0.04136f, 0.04136f, 0.55f };
	float mat_specular[4] = { 0.727811f, 0.626959f, 0.626959f, 0.55f };
	glm::vec4 light_position_ = glm::vec4(0.0f, 0.3f, 0.0f, 1.0f);
	glm::vec4 light_product_ambient_ = glm::vec4(0.1f, 0.01f, 0.01f, 1.0f);
	glm::vec4 light_product_diffuse_ = glm::vec4(1.0f, 0.1f, 0.1f, 1.0f);
	glm::vec4 light_product_specular_ = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	float mat_shininess_ = 5.0;
	int num_elements_ = 0;
	TV center;
	GL2_Material mat_;
	OBJReader obj_reader;

	obj() {}
	obj(const char* file_loc) {
		file = file_loc;
		obj_reader.readObj(file);
	}
	void set_mat_ambient(float a, float b, float c, float d) {
		mat_ambient[0] = a; mat_ambient[1] = b; mat_ambient[2] = c; mat_ambient[3] = d;
	}
	void set_mat_diffuse(float a, float b, float c, float d) {
		mat_diffuse[0] = a; mat_diffuse[1] = b; mat_diffuse[2] = c; mat_diffuse[3] = d;
	}
	void set_mat_specular(float a, float b, float c, float d) {
		mat_specular[0] = a; mat_specular[1] = b; mat_specular[2] = c; mat_specular[3] = d;
	}
	void updatePhongSurfaceWithTexture(const StaticTriangularSurface& surface)
	{
		num_elements_ = surface.triangles_.num_elements_ * 3;

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, surface.vertex_positions_.num_elements_ * sizeof(float) * 3, surface.vertex_positions_.values_, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, surface.vertex_normals_.num_elements_ * sizeof(float) * 3, surface.vertex_normals_.values_, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, surface.vertex_uv_.num_elements_ * sizeof(float) * 2, surface.vertex_uv_.values_, GL_STATIC_DRAW);

		Array1D<Vector3D<unsigned int> > tri_ix;
		surface.getUnsignedIntTriangles(tri_ix);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, tri_ix.num_elements_ * sizeof(unsigned int) * 3,
			surface.triangles_.values_, GL_STATIC_DRAW);
	}
	void setTexture(const char* image_dir) {
		glGenTextures(2, textures);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);

		int texture_width, texture_height;
		unsigned char *rgb_array = nullptr;
		readBMP24(image_dir, &texture_width, &texture_height, &rgb_array);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
		// do not forget these options!
		// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	void BindBuffer() {
		glGenBuffers(3, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, obj_reader.pos_stack_.size() * sizeof(float) * 3,
			&obj_reader.pos_stack_[0], GL_STATIC_DRAW);

		// altered to normal vector
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, obj_reader.nor_stack_.size() * sizeof(float) * 3,
			&obj_reader.nor_stack_[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj_reader.ix_stack_.size() * sizeof(unsigned int) * 3,
			&obj_reader.ix_stack_[0], GL_STATIC_DRAW);
	}
	void translate(glm::vec3 t) {
		for (int i = 0; i < obj_reader.pos_stack_.size(); i++) {
			obj_reader.pos_stack_[i].x += t.x;
			obj_reader.pos_stack_[i].y += t.y;
			obj_reader.pos_stack_[i].z += t.z;
		}
	}
	void rotate(float rot_rate, glm::vec3 dir) {
		glm::quat rot = glm::angleAxis(3.14f / 4.0f * rot_rate, dir); // rotate by x axis
		glm::mat4 m_rot = glm::toMat4(rot); // quaternion to 4x4 matrix
		glm::vec4 v4;
		for (int i = 0; i < obj_reader.pos_stack_.size(); i++) {
			v4 = glm::vec4(obj_reader.pos_stack_[i].x, obj_reader.pos_stack_[i].y, obj_reader.pos_stack_[i].z, 1);
			v4 = m_rot * v4;
			obj_reader.pos_stack_[i].x = v4.x;
			obj_reader.pos_stack_[i].y = v4.y;
			obj_reader.pos_stack_[i].z = v4.z;
		}
		obj_reader.recalculateNormalVector();
	}
	void updateBuffer() {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, 0);
		//glColorPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		glPolygonMode(GL_FRONT, GL_FILL);
		glDrawElements(GL_TRIANGLES, obj_reader.ix_stack_.size() * 3, GL_UNSIGNED_INT, 0);

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	void prepareTexture() {
		GLint loc = glGetUniformLocation(gl_world.shaders_.program_id_, "my_texture");
		if (loc != -1)
		{
			glUniform1i(loc, 0); // 0 is the index of our first texture
		}
	}
	void drawWithShaderAndTexture(const GL2_ShaderProgram& program)
	{
		//glUseProgram(shader_programme);	// activate your shader!

		//TODO: use one 'lightproduct' uniform for amb, dif, and spe.
		program.sendUniform(light_position_, "light_position");
		program.sendUniform(light_product_ambient_, "light_product_ambient");
		program.sendUniform(light_product_diffuse_, "light_product_diffuse");
		program.sendUniform(light_product_specular_, "light_product_specular");
		program.sendUniform(mat_shininess_, "mat_shininess");

		// draw here
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer
		(
			0,                  // attribute 0
			3,                  // size (x, y, z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer
		(
			1,                  // attribute 1
			3,                  // size (nx, ny, nz)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer
		(
			2,					// attribute 2
			2,					// size (u, v)
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			0,					// stride
			(void*)0			// array buffer offset
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, 0);
	}
	void drawWithShader(const GL2_ShaderProgram& program)
	{
		//glUseProgram(shader_programme);	// activate your shader!

		//TODO: use one 'lightproduct' uniform for amb, dif, and spe.
		program.sendUniform(light_position_, "light_position");
		program.sendUniform(light_product_ambient_, "light_product_ambient");
		program.sendUniform(light_product_diffuse_, "light_product_diffuse");
		program.sendUniform(light_product_specular_, "light_product_specular");
		program.sendUniform(mat_shininess_, "mat_shininess");

		// draw here
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer
		(
			0,                  // attribute 0
			3,                  // size (r, g, b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer
		(
			1,                  // attribute 0
			3,                  // size (r, g, b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		glLineWidth(1);
		glPolygonMode(GL_FRONT, GL_LINES);
		glDrawElements(GL_TRIANGLES, obj_reader.ix_stack_.size() * 3, GL_UNSIGNED_INT, 0);
	}
	void obj::scale(const T s) {
		scale(s, s, s, true);
	}
	void obj::scale(const float sx, const float sy, const float sz, const bool centered) {
		center = getAABB().getCenter();

		const int num = obj_reader.pos_stack_.size();

		const float dx = obj_reader.max.x - obj_reader.min.x;
		const float dy = obj_reader.max.y - obj_reader.min.y;
		const float dz = obj_reader.max.z - obj_reader.min.z;
		const float dl = std::max(dz, std::max(dx, dy));

		// scale down the BB to a unit cube
		for (int j = 0; j < obj_reader.pos_stack_.size(); j++)
		{
			obj_reader.pos_stack_[j].x *= sx;
			obj_reader.pos_stack_[j].y *= sy;
			obj_reader.pos_stack_[j].z *= sz;
		}
		obj_reader.recalculateNormalVector();

	}
	Box3D<T> getAABB() const
	{
		if (obj_reader.pos_stack_.size() == 0) return Box3D<T>(0, 0, 0, 0, 0, 0);

		TV min(obj_reader.pos_stack_[0].x, obj_reader.pos_stack_[0].y, obj_reader.pos_stack_[0].z);
		TV max(obj_reader.pos_stack_[0].x, obj_reader.pos_stack_[0].y, obj_reader.pos_stack_[0].z);

		for (int v_ix = 1; v_ix < obj_reader.pos_stack_.size(); ++v_ix)
		{
			min.x_ = MIN2(min.x_, obj_reader.pos_stack_[v_ix].x);
			min.y_ = MIN2(min.y_, obj_reader.pos_stack_[v_ix].y);
			min.z_ = MIN2(min.z_, obj_reader.pos_stack_[v_ix].z);
			max.x_ = MAX2(max.x_, obj_reader.pos_stack_[v_ix].x);
			max.y_ = MAX2(max.y_, obj_reader.pos_stack_[v_ix].y);
			max.z_ = MAX2(max.z_, obj_reader.pos_stack_[v_ix].z);
		}
		return Box3D<T>(min, max);
	}

	void drawPhongSurface()
	{
		glUseProgram(0);

		glEnable(GL_LIGHTING);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		glPolygonMode(GL_FRONT, GL_LINE);
		glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, 0);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	void applyLighting(const GL2_Light& light)
	{
		light_product_ambient_ = light.ambient_ * mat_.ambient_ + mat_.emission_;
		light_product_diffuse_ = light.diffuse_ * mat_.diffuse_;
		light_product_specular_ = light.specular_ * mat_.specular_;

		light_position_ = light.position_;
		mat_shininess_ = mat_.shininess_;
	}
};
void mapPieces2Board(obj*** pc_obj, obj*** brd_pc_ptr) {
	// pc = obj[2][16]

	brd_pc_ptr[0][0] = pc_obj[0][6];
	brd_pc_ptr[0][1] = pc_obj[0][4];
	brd_pc_ptr[0][2] = pc_obj[0][2];
	brd_pc_ptr[0][3] = pc_obj[0][0];
	brd_pc_ptr[0][4] = pc_obj[0][1];
	brd_pc_ptr[0][5] = pc_obj[0][3];
	brd_pc_ptr[0][6] = pc_obj[0][5];
	brd_pc_ptr[0][7] = pc_obj[0][7];

	for (int i = 0; i < BOARD_SIZE; i++) {
		brd_pc_ptr[1][i] = pc_obj[0][8 + i];
	}

	brd_pc_ptr[7][0] = pc_obj[1][6];
	brd_pc_ptr[7][1] = pc_obj[1][4];
	brd_pc_ptr[7][2] = pc_obj[1][2];
	brd_pc_ptr[7][3] = pc_obj[1][0];
	brd_pc_ptr[7][4] = pc_obj[1][1];
	brd_pc_ptr[7][5] = pc_obj[1][3];
	brd_pc_ptr[7][6] = pc_obj[1][5];
	brd_pc_ptr[7][7] = pc_obj[1][7];

	for (int i = 0; i < BOARD_SIZE; i++) {
		brd_pc_ptr[6][i] = pc_obj[1][8 + i];
	}
}