#ifndef COMBOSHOOTER_UTIL2D_SPRITEBATCH_H
#define COMBOSHOOTER_UTIL2D_SPRITEBATCH_H

namespace gl {
	class Texture;
}

#include "Sprite.h"
#include "../util3d/gl/VertexArrayObject.h"
#include "../util3d/gl/BufferObject.h"

#include <list>

namespace util2d {

class SpriteBatch
{
public:
	SpriteBatch();

	Sprite* newSprite();
	void draw() const;

	void setTexture(gl::Texture* tex);
	const gl::Texture* getTexture() const;

	static void setup_state();
	static void initialize_shared();
	static void deinitialize_shared();

private:
	void remove(Sprite& spr);

	gl::Texture* texture;
	std::list<Sprite> sprites;

	gl::VertexArrayObject spr_vao;
	gl::BufferObject spr_vbo;

	unsigned int buffer_size; // In sprites

	friend class Sprite;
};

} // namespace util2d

#endif // COMBOSHOOTER_UTIL2D_SPRITEBATCH_H
