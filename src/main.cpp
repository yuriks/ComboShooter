#include "util3d/Matrix.h"
#include "util3d/Vector.h"
#include "util3d/MatrixTransform.h"
#include "util3d/gl/VertexArrayObject.h"
#include "util3d/gl/BufferObject.h"
#include "util3d/gl/Shader.h"
#include "util3d/gl/ShaderProgram.h"
#include "util3d/gl/Texture.h"
#include "image/ImageLoader.h"
#include "util2d/SpriteBatch.h"
#include "util2d/Tilemap.h"

#include <iostream>
#include <fstream>

#include "util3d/gl3w.h"

//#define GLFW_GL3_H
#include <GL/glfw.h>

struct vertex_data {
	float x, y, z, w;
//	float r, g, b, a;
	float u, v;
	float fill1, fill2;
};
static_assert (sizeof(vertex_data) == sizeof(float)*8, "Oops. Padding.");

static const vertex_data data[3] = {
	{-.5f,  .5f, 0.f, 1.f, /*1.f, 0.f, 0.f, 1.f,*/ 1.f, 1.f},
	{ .5f, -.5f, 0.f, 1.f, /*0.f, 1.f, 0.f, 1.f,*/ 0.f, 0.f},
	{-.5f, -.5f, 0.f, 1.f, /*0.f, 0.f, 1.f, 1.f,*/ 1.f, 0.f}
};
static_assert (sizeof(data) == sizeof(vertex_data)*3, "Oops. Padding.");

mat4 screen_transform;

void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
	std::cout << message << std::endl;
}

void sprite_test()
{
	image::Image img;
	{
		std::ifstream f("testimg-rgba.png", std::ios::in | std::ios::binary);
		image::Image::loadPNGFileRGBA8(img, f);
	}

	util2d::SpriteBatch::initialize_shared();

	util2d::SpriteBatch spr_batch;
	const int num_spr = 512;
	util2d::Sprite* spr[num_spr];
	const float pi = 3.14159265f;
	float rot[num_spr];
	signed char spdx[num_spr];
	signed char spdy[num_spr];
	for (int i = 0; i < num_spr; ++i) {
		spr[i] = spr_batch.newSprite();
		rot[i] = rand() / (float)RAND_MAX * 2*pi;
		spdx[i] = rand() % 4;
		spdy[i] = rand() % 4;

		if (rand() & 1) spdx[i] = -1;
		if (rand() & 1) spdy[i] = -1;

		spr[i]->x = rand() % 799;
		spr[i]->y = rand() % 599;
		spr[i]->img_x = spr[i]->img_y = 0;
		spr[i]->img_w = spr[i]->img_h = 1;
		spr[i]->r = rand() % 256;
		spr[i]->g = rand() % 256;
		spr[i]->b = rand() % 256;
		spr[i]->a = rand() % 256;

		spr[i]->transform(0,0) = 4.f * std::sin(rot[i]*2.f) * std::cos(rot[i]);
		spr[i]->transform(0,1) = 4.f * std::sin(rot[i]);
		spr[i]->transform(1,0) = -4.f * std::sin(rot[i]*2.f) * std::sin(rot[i]);
		spr[i]->transform(1,1) = 4.f * std::cos(rot[i]);
	}


	gl::Texture tex;
	glActiveTexture(GL_TEXTURE0);
	tex.bind(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	tex.width = img.getWidth();
	tex.height = img.getHeight();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getData());

	spr_batch.setTexture(&tex);

	bool running = true;

	glClearColor(.2f, .2f, .2f, 1.f);

	while (running) {
		double time_start = glfwGetTime();

		for (int i = 0; i < num_spr; ++i) {
			rot[i] += 0.01f;

			spr[i]->x += spdx[i];
			spr[i]->y += spdy[i];

			if (spr[i]->x >= 799 || spr[i]->x < 0) spdx[i] = -spdx[i];
			if (spr[i]->y >= 599 || spr[i]->y < 0) spdy[i] = -spdy[i];

			const float s2 = std::sin(rot[i]*2.f);
			const float c = std::cos(rot[i]);
			const float s = std::sin(rot[i]);

			spr[i]->transform(0,0) = 4.f * s2 * c;
			spr[i]->transform(0,1) = 4.f * s;
			spr[i]->transform(1,0) = -4.f * s2 * s;
			spr[i]->transform(1,1) = 4.f * c;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		spr_batch.draw();

		std::cout << "\r" << (glfwGetTime() - time_start) * 1000.0 << "ms" << std::flush;

		glfwSwapBuffers();

		running = glfwGetWindowParam(GLFW_OPENED) != 0;
	}
}

void tilemap_test()
{
	image::Image img;
	{
		std::ifstream f("tileset.png", std::ios::in | std::ios::binary);
		image::Image::loadPNGFileRGBA8(img, f);
	}

	util2d::Tilemap::initialize_shared();

	gl::Texture tex;
	glActiveTexture(GL_TEXTURE0);
	tex.bind(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	tex.width = img.getWidth();
	tex.height = img.getHeight();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getData());

	unsigned short map[20*2*15*2];
	for (int i = 0; i < 20; ++i) {
		for (int j = 0; j < 15; ++j) {
			map[20*2*j + i] = 20*j + i;
			map[20*2*j + i + 20] = 20*j + i;
			map[20*2*(j+15) + i] = 20*j + i;
			map[20*2*(j+15) + i + 20] = 20*j + i;
		}
	}

	util2d::Tilemap tilemap(0, 0, 320*2, 240*2);
	tilemap.setTexture(&tex);
	tilemap.setTilemap(map, 20*2, 15*2);

	bool running = true;

	glClearColor(.2f, .2f, .2f, 1.f);

	while (running) {
		double time_start = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT);

		for (int i = 0; i < 128; i += 8) {
			tilemap.x = tilemap.y = i;
			tilemap.draw();
		}

		std::cout << "\r" << (glfwGetTime() - time_start) * 1000.0 << "ms" << std::flush;

		glfwSwapBuffers();

		running = glfwGetWindowParam(GLFW_OPENED) != 0;
	}
}

int main(int argc, char *argv[])
{
	glfwInit();

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwOpenWindow(800, 600, 8, 8, 8, 8, 24, 8, GLFW_WINDOW);

	if (gl3wInit() != 0) {
		std::cerr << "Failed to initialize gl3w." << std::endl;
	} else if (!gl3wIsSupported(3, 3)) {
		std::cerr << "OpenGL 3.3 not supported." << std::endl;
	}

	if (glDebugMessageCallbackARB) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
		glDebugMessageCallbackARB(debug_callback, 0);
	}

	{
		vec3 s = {2.f/800.f, -2.f/600.f, 1.f};
		vec3 t = {-1.f, 1.f, 0.f};
		screen_transform = mat_transform::translate(t) * mat_transform::scale(s);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//sprite_test();
	tilemap_test();

	glfwTerminate();

	return 0;
}
