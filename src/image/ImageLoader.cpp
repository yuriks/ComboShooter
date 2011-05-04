#include "ImageLoader.hpp"

#include <cassert>
#include <istream>

#include <png.h>
#include <csetjmp>

namespace image {

void Image::initialize(unsigned int width_, unsigned int height_)
{
	assert(width_ != -1);
	assert(height_ != -1);

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

extern "C" void read_data(png_structp png_ptr, png_bytep data, png_size_t len)
{
	std::istream* stream = static_cast<std::istream*>(png_get_io_ptr(png_ptr));

	if (!stream->read(reinterpret_cast<char*>(data), len)) {
		png_error(png_ptr, "I/O Error");
	}
}

void Image::loadPNGFileRGBA8(Image& image, std::istream& stream)
{
	png_structp png_ptr = 0;
	png_infop info_ptr = 0;
	bool clear_image = false;
	png_uint_32 img_width = -1;
	png_uint_32 img_height = -1;
	int bit_depth, color_type, interlace_type;
	png_bytep* row_pointers = 0;
	unsigned int i;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (png_ptr == 0) {
		if (clear_image)
			image.clear();
		delete[] row_pointers;
		throw PngException("Error creating png read struct.");
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == 0) {
		png_destroy_read_struct(&png_ptr, 0, 0);
		throw PngException("Error creating png info struct.");
	}

	if (setjmp(png_jmpbuf(png_ptr)) != 0) {
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);

		throw PngException("Unknown error.");
	}

	png_set_read_fn(png_ptr, static_cast<void*>(&stream), read_data);

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &img_width, &img_height, &bit_depth, &color_type, &interlace_type, 0, 0);

	//if (interlace_type != PNG_INTERLACE_NONE)
	//	png_error(png_ptr, "Interlaced images not supported.");
	png_set_interlace_handling(png_ptr);

	png_set_strip_16(png_ptr);

	png_set_gray_to_rgb(png_ptr);
	
	png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);

	png_read_update_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &img_width, &img_height, &bit_depth, &color_type, 0, 0, 0);
	// In case I didn't do all needed transformations above. (Likely.)
	if (bit_depth != 8)
		png_error(png_ptr, "Unsupported format: Not 8-bit.");
	if (color_type != PNG_COLOR_TYPE_RGBA)
		png_error(png_ptr, "Unsupported format: Not RGBA.");

	clear_image = true;
	image.initialize(img_width, img_height);

	row_pointers = new png_bytep[img_height];
	for (i = 0; i < img_height; ++i) {
		row_pointers[i] = &image.getData()[i*img_width*4];
	}

	png_read_image(png_ptr, row_pointers);

	png_read_end(png_ptr, info_ptr);

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
}

}
