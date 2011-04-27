#ifndef COMBOSHOOTER_UTIL2D_TILEMAP_H
#define COMBOSHOOTER_UTIL2D_TILEMAP_H

#include "../util3d/gl/VertexArrayObject.hpp"
#include "../util3d/gl/BufferObject.hpp"
#include "../util3d/gl/Texture.hpp"

namespace util2d {

class Tilemap
{
public:
	Tilemap(int x, int y, int width, int height);

	void draw();

	void setTexture(gl::Texture* tex);
	const gl::Texture* getTexture() const;

	void setTilemap(unsigned short* data, int width, int height);

	int x, y;
	// Warning: You shouldn't rely on being able to read back absolute values
	// from these variables, since they are fixed up to account for wrapping
	// on every draw call.
	int offx, offy;

	static void initialize_shared();
	static void deinitialize_shared();

private:
	void fixupScrolling();

	gl::Texture* texture;

	gl::VertexArrayObject tile_vao;
	gl::BufferObject tile_vbo;
	gl::Texture tilemap;
};

} // namespace util2d

#endif // COMBOSHOOTER_UTIL2D_TILEMAP_H
