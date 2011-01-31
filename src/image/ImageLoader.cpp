#include "ImageLoader.h"

#include <cassert>

namespace image {

void Image::initialize(int width_, int height_)
{
	assert(width_ > 0);
	assert(height_ > 0);

	clear();
	data = new unsigned char[width_ * height_ * 4];
	width = width_;
	height = height_;
}

void Image::clear()
{
	delete[] data;
	width = -1;
	height = -1;
	data = 0;
}

}
