#ifndef COMBOSHOOTER_IMAGE_IMAGELOADER_H
#define COMBOSHOOTER_IMAGE_IMAGELOADER_H

#include <boost/noncopyable.hpp>

#include <iosfwd>
#include <stdexcept>

namespace image {

class PngException : std::runtime_error
{
public:
	PngException(const std::string& str);
};

class IOException : std::runtime_error
{
public:
	IOException(const std::string& str);
};

class Image : boost::noncopyable
{
public:
	Image();
	Image(int width, int height);
	~Image();

	void initialize(int width, int height);
	void clear();

	int getWidth() const;
	int getHeight() const;
	unsigned char* getData() const;

	static void loadPNGFileRGBA8(Image& image, std::istream& stream);

private:
	int width;
	int height;
	unsigned char* data;
};

///////////////////////////////////////////////////////////////////////////////

inline PngException::PngException(const std::string& str)
	: std::runtime_error(str)
{
}

inline IOException::IOException(const std::string& str)
	: std::runtime_error(str)
{
}

inline Image::Image()
	: width(-1), height(-1), data(0)
{
}

inline Image::Image(int width, int height)
	: width(-1), height(-1), data(0)
{
	initialize(width, height);
}

inline Image::~Image()
{
	clear();
}

inline int Image::getWidth() const
{
	return width;
}

inline int Image::getHeight() const
{
	return height;
}

inline unsigned char* Image::getData() const
{
	return data;
}

}

#endif // COMBOSHOOTER_IMAGE_IMAGELOADER_H
