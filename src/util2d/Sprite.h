#ifndef COMBOSHOOTER_UTIL2D_SPRITE_H
#define COMBOSHOOTER_UTIL2D_SPRITE_H

#include "../util3d/Matrix.h"

#include <memory>
#include <list>

namespace util2d {

class SpriteBatch;

class Sprite
{
public:
	~Sprite();

	// Important: pointer is no longer valid after calling this!
	void remove();

	short x, y;
	mat2 transform;
	unsigned char r, g, b, a;

	unsigned char img_x, img_y;
	unsigned char img_w, img_h;

private:
	explicit Sprite(SpriteBatch& parent);
	
	SpriteBatch& parent;
	std::list<Sprite>::iterator iter_to_self;

	friend class SpriteBatch;
};

} // namespace util2d

#endif // COMBOSHOOTER_UTIL2D_SPRITE_H
