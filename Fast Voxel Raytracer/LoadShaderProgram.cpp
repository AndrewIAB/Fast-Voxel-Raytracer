#include <iostream>
#include <fstream>
#include <glad/glad.h>

bool LoadShader(unsigned int shader, const char* path) {
	std::ifstream file(path, std::ios::binary);

	if (file.bad() || file.is_open() == false) {
		std::cout << "Error opening shader file " << path << '\n';
		return false;
	}

	file.seekg(0, std::ios::end);
	int size = file.tellg();
	file.seekg(0, std::ios::beg);

	char* buffer = new char[size < 512 ? 512 : size];

	file.read(buffer, size);

	glShaderSource(shader, 1, &buffer, &size);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderInfoLog(shader, 512, nullptr, buffer);
		std::cout << "== Error compiling \"" << path << "\" ==\n" << buffer;
	}

	delete[] buffer;
	return success == GL_TRUE;
}

unsigned int LoadProgram(std::string name) {
	std::string vertPath = "assets\\" + name + ".vert";
	std::string fragPath = "assets\\" + name + ".frag";

	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	unsigned int program = 0;

	bool passVert = LoadShader(vertShader, vertPath.data());
	bool passFrag = LoadShader(fragShader, fragPath.data());

	if (passVert && passFrag) {
		std::cout << "Successfully compiled " << vertPath << " and " << fragPath << '\n';
		program = glCreateProgram();
		glAttachShader(program, vertShader);
		glAttachShader(program, fragShader);
		glLinkProgram(program);
	}

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		char buffer[512];
		glGetProgramInfoLog(program, 512, nullptr, buffer);

		std::cout << "== Error linking \"" << name << "\" ==\n" << buffer;

		glDeleteProgram(program);
		program = 0;
	}
	else {
		std::cout << "Successfully linked " << name << '\n';
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}