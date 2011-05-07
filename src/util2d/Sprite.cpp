#include "Sprite.hpp"

#include "../util3d/MatrixTransform.hpp"
#include "SpriteBatch.hpp"

namespace util2d {

Sprite::Sprite(SpriteBatch& parent_)
	: x(0), y(0), transform(math::mat_transform::identity<2>()),
	r(255), g(255), b(255), a(255),
	img_x(0), img_y(0), img_w(0), img_h(0),
	parent(parent_)
{
}

Sprite::~Sprite()
{
}

Sprite& Sprite::operator=(const Sprite& o)
{
	x = o.x;
	y = o.y;
	transform = o.transform;

	r = o.r;
	g = o.g;
	b = o.b;
	a = o.a;

	img_x = o.img_x;
	img_y = o.img_y;
	img_w = o.img_w;
	img_h = o.img_h;

	return *this;
}

void Sprite::remove()
{
	parent.remove(*this);
}

} // namespace util2d
