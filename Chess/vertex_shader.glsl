#version 330

uniform mat4 mvp;

//uniform mat4 gl_ModelViewMatrix;
//uniform mat4 gl_ProjectionMatrix;

in vec3 a_pos;
in vec3 a_nor;
in vec2 a_uv;

out vec4 v;
out vec4 N;
out vec2 uv;

void main()
{
	//v = gl_ModelViewMatrix * gl_ProjectionMatrix * vec4(a_pos, 1.0);
	//N = normalize(gl_ModelViewMatrix * gl_ProjectionMatrix * vec4(a_nor, 0.0f));
	v = mvp * vec4(a_pos, 1.0);
	N = normalize(mvp * vec4(a_nor, 0.0f));
	uv = a_uv;
	gl_Position = v;	
};