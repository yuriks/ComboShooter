#include "util3d/Matrix.h"
#include "util3d/Vector.h"
#include "util3d/MatrixTransform.h"
#include "util3d/gl/VertexArrayObject.h"
#include "util3d/gl/BufferObject.h"
#include "util3d/gl/Shader.h"
#include "util3d/gl/ShaderProgram.h"
#include "util3d/gl/Texture.h"
#include "image/ImageLoader.h"

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

void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
	std::cout << message << std::endl;
}

int main(int argc, char *argv[])
{
	image::Image img;

	{
		std::ifstream f("testimg-rgba.png", std::ios::in | std::ios::binary);
		image::Image::loadPNGFileRGBA8(img, f);
	}

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
		gl::VertexArrayObject vao;
		vao.bind();

		gl::BufferObject vbo;
		vbo.bind(GL_ARRAY_BUFFER);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data)*3, static_cast<const void*>(&data), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (float*)0 + 0);
		glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,  sizeof(vertex_data), (float*)0 + 4);
		//glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (float*)0 + 4);
		glEnableVertexAttribArray(1);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		gl::Texture tex;
		glActiveTexture(GL_TEXTURE0);
		tex.bind(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getData());

		gl::ShaderProgram shader_prog;

		{
			gl::Shader vert_shader(GL_VERTEX_SHADER);
			gl::Shader frag_shader(GL_FRAGMENT_SHADER);

			{
				std::ifstream f("main_shader.vert");
				vert_shader.setSource(f);
			}
			{
				std::ifstream f("main_shader.frag");
				frag_shader.setSource(f);
			}

			vert_shader.compile();
			frag_shader.compile();

			std::cout << "### Vertex Shader Log ###\n";
			vert_shader.printInfoLog(std::cout);
			std::cout << "### Fragment Shader Log ###\n";
			frag_shader.printInfoLog(std::cout);

			shader_prog.attachShader(vert_shader);
			shader_prog.attachShader(frag_shader);

			shader_prog.bindAttribute(0, "in_Position");
			//shader_prog.bindAttribute(1, "in_Color");
			shader_prog.bindAttribute(1, "in_TexCoord");
			glBindFragDataLocation(shader_prog, 0, "out_Color");

			shader_prog.link();
			std::cout << "### Linking Log ###\n";
			shader_prog.printInfoLog(std::cout);
		}

		shader_prog.use();

		bool running = true;

		vec3 s = {600./800., 1.f, 1.f};
		mat4 m = mat_transform::scale(s);

		glUniformMatrix4fv(shader_prog.getUniformLocation("in_Proj"), 1, false, &m.data[0]);
		glUniform1i(shader_prog.getUniformLocation("in_Tex0"), 0);

		while (running) {
			glClearColor(.2f, .2f, .2f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);

			glDrawArrays(GL_TRIANGLES, 0, 3);

			glfwSwapBuffers();

			running = glfwGetWindowParam(GLFW_OPENED) != 0;
		}
	}

	glfwTerminate();

	return 0;
}
