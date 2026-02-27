#version 430 core

in vec2 i_Uv;
out vec4 o_Color;

layout (binding = 0) uniform sampler2D u_Texture;

void main() {
	vec4 textureSample = texture(u_Texture, i_Uv);
	
	o_Color = vec4(textureSample.xyz, 1.);
}