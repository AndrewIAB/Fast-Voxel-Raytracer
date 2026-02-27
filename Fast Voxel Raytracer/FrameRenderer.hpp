#pragma once

class FrameRenderer {
private:
	unsigned int program;

	const int frameWidth, frameHeight;
	const unsigned int vao;

	static const int bind_u_Texture = 0;
public:
	FrameRenderer(int width, int height, unsigned int quadVao);
	~FrameRenderer();

	void Render(unsigned int texture);
};