#pragma once

#include <string>

class RayRenderer {
private:
	unsigned int program;
	unsigned int frameBuffer;
	unsigned int frameTexture;
	unsigned int noiseTexture;

	const unsigned int vao;
	const int frameWidth, frameHeight;

	static const int loc_u_RotationX = 0;
	static const int loc_u_RotationY = 1;
	static const int loc_u_Position = 2;

	static const int bind_u_NoiseTexture = 0;
public:
	RayRenderer(int width, int height, unsigned int quadBuffer);
	~RayRenderer();

	unsigned int GetFrameTexture();

	void SetCameraPosition(float x, float y, float z);
	void SetCameraRotation(float x, float y);

	void Render();
};