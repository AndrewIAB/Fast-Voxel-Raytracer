#version 430 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_Uv;

out vec2 i_Uv;
out vec3 i_CoordPos;

layout (location = 0) uniform mat3 u_RotationX;
layout (location = 1) uniform mat3 u_RotationY;

void main() {
	i_Uv = a_Uv;
	i_CoordPos = vec3(a_Position, -1.) * u_RotationX * u_RotationY;
	gl_Position = vec4(a_Position, 0., 1.);
}