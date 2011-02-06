#include "Sprite.h"

#include "../util3d/MatrixTransform.h"
#include "SpriteBatch.h"

namespace util2d {

Sprite::Sprite(SpriteBatch& parent_)
	: x(0), y(0), transform(mat_transform::identity<2>()),
	img_x(0), img_y(0), img_w(0), img_h(0),
	parent(parent_)
{
}

Sprite::~Sprite()
{
}

void Sprite::remove()
{
	parent.remove(*this);
}

} // namespace util2d
