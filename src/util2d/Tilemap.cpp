#include "Tilemap.h"

#include "../util3d/Vector.hpp"
#include "../util3d/Matrix.hpp"
#include "../util3d/MatrixTransform.hpp"
#include "../util3d/gl/VertexArrayObject.h"
#include "../util3d/gl/BufferObject.h"
#include "../util3d/gl/ShaderProgram.h"
#include "../util3d/gl/Texture.h"

#include <cassert>
#include <fstream>

extern math::mat4 screen_transform;

namespace util2d {

namespace {

gl::Shader* tile_base_vert_shader = 0;
gl::Shader* tile_base_frag_shader = 0;
gl::ShaderProgram* tile_base_shader = 0;

GLuint u_Tex0, u_Tilemap, u_ScreenTransform, u_Pos, u_Offset;

struct tile_data {
	GLushort x, y;
};
static const int tile_size = 4;
static_assert (sizeof(tile_data) == tile_size, "Struct doesn't fit on a 4-byte slot.");

} // namespace

Tilemap::Tilemap(int x, int y, int width, int height)
	: x(x), y(y), offx(0), offy(0), texture(0)
{
	if (!tile_base_shader)
		initialize_shared();

	tile_vao.bind();

	const tile_data vertices[4] = {
		{0,               0},
		{(GLushort)width, 0},
		{0,               (GLushort)height},
		{(GLushort)width, (GLushort)height}
	};

	tile_vbo.bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, 4*tile_size, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_UNSIGNED_SHORT, GL_FALSE, tile_size, (void*)offsetof(tile_data, x));
	glEnableVertexAttribArray(0);
}

void Tilemap::draw()
{
	fixupScrolling();

	tile_vao.bind();

	tile_base_shader->use();

	glUniform1i(u_Tex0, 0);
	glUniform1i(u_Tilemap, 1);

	glUniformMatrix4fv(u_ScreenTransform, 1, true, &screen_transform.data[0]);

	glUniform2f(u_Pos, GLfloat(x), GLfloat(y));
	glUniform2i(u_Offset, offx, offy);

	glActiveTexture(GL_TEXTURE0);
	texture->bind(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	tilemap.bind(GL_TEXTURE_2D);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Tilemap::setTexture(gl::Texture* tex)
{
	texture = tex;
}

const gl::Texture* Tilemap::getTexture() const
{
	return texture;
}

void Tilemap::setTilemap(unsigned short* data, int width, int height)
{
	glActiveTexture(GL_TEXTURE1);
	tilemap.bind(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	tilemap.width = width;
	tilemap.height = height;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);
}

void Tilemap::fixupScrolling()
{
	if (offx < 0) {
		offx = (tilemap.width * 16) + (offx % (tilemap.width * 16));
	} else if (offx > (tilemap.width * 16)) {
		offx %= (tilemap.width * 16);
	}

	if (offy < 0) {
		offy = (tilemap.height * 16) + (offy % (tilemap.height * 16));
	} else if (offx > (tilemap.height * 16)) {
		offy %= (tilemap.height * 16);
	}
}

void Tilemap::initialize_shared()
{
	if (!tile_base_vert_shader) {
		tile_base_vert_shader = new gl::Shader(GL_VERTEX_SHADER);
		gl::Shader& s = *tile_base_vert_shader;

		{
			std::ifstream f("tilemap_shader.vert");
			s.setSource(f);
		}
		s.compile();
		std::cerr << "### Vertex Shader Log ###\n";
		s.printInfoLog(std::cerr);
	}

	if (!tile_base_frag_shader) {
		tile_base_frag_shader = new gl::Shader(GL_FRAGMENT_SHADER);
		gl::Shader& s = *tile_base_frag_shader;

		{
			std::ifstream f("tilemap_shader.frag");
			s.setSource(f);
		}
		s.compile();
		std::cerr << "### Fragment Shader Log ###\n";
		s.printInfoLog(std::cerr);
	}

	if (!tile_base_shader) {
		tile_base_shader = new gl::ShaderProgram;
		gl::ShaderProgram& s = *tile_base_shader;

		s.attachShader(*tile_base_vert_shader);
		s.attachShader(*tile_base_frag_shader);

		s.bindAttribute(0, "in_TilePos");
		glBindFragDataLocation(s, 0, "out_Color");

		s.link();
		std::cerr << "### Linking Log ###\n";
		s.printInfoLog(std::cerr);

		u_Tex0 = s.getUniformLocation("u_Tex0");
		u_Tilemap = s.getUniformLocation("u_Tilemap");
		u_ScreenTransform = s.getUniformLocation("u_ScreenTransform");
		u_Pos = s.getUniformLocation("u_Pos");
		u_Offset = s.getUniformLocation("u_Offset");
	}
}

void Tilemap::deinitialize_shared()
{
	delete tile_base_shader;
	delete tile_base_vert_shader;
	delete tile_base_frag_shader;
}

} // namespace util2d
