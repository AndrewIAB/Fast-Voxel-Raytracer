#include "RayRenderer.hpp"

#include <iostream>
#include <fstream>
#include <glad/glad.h>

unsigned int LoadProgram(std::string name);

RayRenderer::RayRenderer(int width, int height, unsigned int quadVao) :
	frameWidth(width), frameHeight(height), vao(quadVao)
{
	program = LoadProgram("raytrace");

	glGenFramebuffers(1, &frameBuffer);
	glGenTextures(1, &frameTexture);
	glGenTextures(1, &noiseTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	const int noiseWidth = width;
	const int noiseHeight = height;
	const size_t noiseScale = noiseWidth * noiseHeight * 4;

	unsigned char* noiseData = new unsigned char[noiseScale];

	glBindTexture(GL_TEXTURE_2D, frameTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture, 0);

	for (size_t i = 0; i < noiseScale; i++) {
		noiseData[i] = (unsigned int)rand() & 0xff;
	}

	glBindTexture(GL_TEXTURE_2D, noiseTexture);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		GL_RGB, noiseWidth, noiseWidth, 0,
		GL_RGB, GL_UNSIGNED_BYTE, noiseData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RayRenderer::~RayRenderer() {
	glDeleteProgram(program);
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &frameTexture);
	glDeleteTextures(1, &noiseTexture);
}

unsigned int RayRenderer::GetFrameTexture() {
	return frameTexture;
}

void RayRenderer::SetCameraPosition(float x, float y, float z) {
	glUseProgram(program);
	glUniform3f(loc_u_Position, x, y, z);
}

void RayRenderer::SetCameraRotation(float x, float y) {
	float cx = cosf(x);
	float sx = sinf(x);
	
	float cy = cosf(y);
	float sy = sinf(y);
	
	float rotX[9] = {
		1, 0, 0,
		0, cx, sx,
		0, -sx, cx
	};

	float rotY[9] = {
		cy, 0, -sy,
		0, 1, 0,
		sy, 0, cy
	};

	glUseProgram(program);
	glUniformMatrix3fv(loc_u_RotationX, 1, GL_FALSE, rotX);
	glUniformMatrix3fv(loc_u_RotationY, 1, GL_FALSE, rotY);
}

void RayRenderer::Render() {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, frameWidth, frameHeight);

	glBindVertexArray(vao);
	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0 + bind_u_NoiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}