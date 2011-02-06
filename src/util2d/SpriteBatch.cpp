#include "SpriteBatch.h"

#include "../util3d/Vector.h"
#include "../util3d/Matrix.h"
#include "../util3d/MatrixTransform.h"
#include "../util3d/gl/VertexArrayObject.h"
#include "../util3d/gl/BufferObject.h"
#include "../util3d/gl/ShaderProgram.h"
#include "../util3d/gl/Texture.h"

#include <cassert>
#include <fstream>

namespace util2d {

namespace {

gl::Shader* spr_base_vert_shader = 0;
gl::Shader* spr_base_geom_shader = 0;
gl::Shader* spr_base_frag_shader = 0;
gl::ShaderProgram* spr_base_shader = 0;

struct sprite_data {
	GLshort x, y;
	GLshort m[4]; // 8:8 fixed point
	GLubyte img_x, img_y;
	GLubyte img_w, img_h;
};
static const int sprite_size = 16;
static_assert (sizeof(sprite_data) <= sprite_size, "Struct doesn't fit on a 16-byte slot.");

} // namespace

SpriteBatch::SpriteBatch()
	: texture(0), buffer_size(64)
{
	if (!spr_base_shader)
		initialize_shared();

	spr_vao.bind();

	spr_vbo.bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, buffer_size*sprite_size, 0, GL_STREAM_DRAW);

	glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, sprite_size, (void*)offsetof(sprite_data, x));
	glVertexAttribPointer(1, 4, GL_SHORT, GL_FALSE, sprite_size, (void*)offsetof(sprite_data, m));
	glVertexAttribIPointer(2, 2, GL_UNSIGNED_BYTE, sprite_size, (void*)offsetof(sprite_data, img_x));
	glVertexAttribIPointer(3, 2, GL_UNSIGNED_BYTE, sprite_size, (void*)offsetof(sprite_data, img_w));
	for (int i = 0; i < 4; ++i)
		glEnableVertexAttribArray(i);
}

Sprite* SpriteBatch::newSprite()
{
	sprites.push_front(Sprite(*this));
	Sprite& spr = sprites.front();
	spr.iter_to_self = sprites.begin();
	
	if (sprites.size() > buffer_size) {
		buffer_size *= 2;
		spr_vbo.bind(GL_ARRAY_BUFFER);
		glBufferData(GL_ARRAY_BUFFER, buffer_size*sprite_size, 0, GL_STREAM_DRAW);
	}

	return &spr;
}

void SpriteBatch::draw() const
{
	spr_vao.bind();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	spr_base_shader->use();

	glUniform1i(spr_base_shader->getUniformLocation("u_Tex0"), 0);
	glUniform2i(spr_base_shader->getUniformLocation("u_TextureSize"), texture->width, texture->height);

	vec3 s = {2.f/800.f, -2.f/600.f, 1.f};
	vec3 t = {-1.f, 1.f, 0.f};
	mat4 m = mat_transform::translate(t) * mat_transform::scale(s);
	
	glUniformMatrix4fv(spr_base_shader->getUniformLocation("u_ScreenTransform"), 1, false, &m.data[0]);

	glActiveTexture(GL_TEXTURE0);
	texture->bind(GL_TEXTURE_2D);

	do {
		char* buf = (char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		for (auto i = sprites.cbegin(); i != sprites.cend(); ++i, buf += sprite_size) {
			sprite_data& spr = *reinterpret_cast<sprite_data*>(buf);

			spr.x = i->x;
			spr.y = i->y;
			for (int j = 0; j < 4; ++j) {
				spr.m[j] = static_cast<GLshort>(i->transform.data[j] * 256.f + .5f);
				int z = spr.m[j];
				z += 0;
			}
			spr.img_x = i->img_x;
			spr.img_y = i->img_y;
			spr.img_w = i->img_w;
			spr.img_h = i->img_h;
		}
	} while (glUnmapBuffer(GL_ARRAY_BUFFER) != GL_TRUE);

	glDrawArrays(GL_POINTS, 0, sprites.size());
}

void SpriteBatch::setTexture(gl::Texture* tex)
{
	texture = tex;
}

const gl::Texture* SpriteBatch::getTexture() const
{
	return texture;
}

void SpriteBatch::initialize_shared()
{
	if (!spr_base_vert_shader) {
		spr_base_vert_shader = new gl::Shader(GL_VERTEX_SHADER);
		gl::Shader& s = *spr_base_vert_shader;

		{
			std::ifstream f("sprite_shader.vert");
			s.setSource(f);
		}
		s.compile();
		std::cerr << "### Vertex Shader Log ###\n";
		s.printInfoLog(std::cerr);
	}

	if (!spr_base_geom_shader) {
		spr_base_geom_shader = new gl::Shader(GL_GEOMETRY_SHADER);
		gl::Shader& s = *spr_base_geom_shader;

		{
			std::ifstream f("sprite_shader.geom");
			s.setSource(f);
		}
		s.compile();
		std::cerr << "### Geometry Shader Log ###\n";
		s.printInfoLog(std::cerr);
	}

	if (!spr_base_frag_shader) {
		spr_base_frag_shader = new gl::Shader(GL_FRAGMENT_SHADER);
		gl::Shader& s = *spr_base_frag_shader;

		{
			std::ifstream f("sprite_shader.frag");
			s.setSource(f);
		}
		s.compile();
		std::cerr << "### Fragment Shader Log ###\n";
		s.printInfoLog(std::cerr);
	}

	if (!spr_base_shader) {
		spr_base_shader = new gl::ShaderProgram;
		gl::ShaderProgram& s = *spr_base_shader;

		s.attachShader(*spr_base_vert_shader);
		s.attachShader(*spr_base_geom_shader);
		s.attachShader(*spr_base_frag_shader);

		s.bindAttribute(0, "in_Pos");
		s.bindAttribute(1, "in_Transform");
		s.bindAttribute(2, "in_TexPos");
		s.bindAttribute(3, "in_TexSize");
		glBindFragDataLocation(s, 0, "out_Color");

		s.link();
		std::cerr << "### Linking Log ###\n";
		s.printInfoLog(std::cerr);
	}
}

void SpriteBatch::deinitialize_shared()
{
	delete spr_base_shader;
	delete spr_base_vert_shader;
	delete spr_base_frag_shader;
}

void SpriteBatch::remove(Sprite& spr)
{
	auto& iter = spr.iter_to_self;
	sprites.erase(iter);
}

} // namespace util2d
