#include "FrameRenderer.hpp"

#include <iostream>
#include <glad/glad.h>

unsigned int LoadProgram(std::string name);

FrameRenderer::FrameRenderer(int width, int height, unsigned int quadVao) :
	frameWidth(width), frameHeight(height), vao(quadVao)
{
	program = LoadProgram("frame");
}

FrameRenderer::~FrameRenderer() {
	glDeleteProgram(program);
}

void FrameRenderer::Render(unsigned int texture) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, frameWidth, frameHeight);

	glBindVertexArray(vao);
	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0 + bind_u_Texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}