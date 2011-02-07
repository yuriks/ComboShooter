#ifndef COMBOSHOOTER_UTIL2D_TILEMAP_H
#define COMBOSHOOTER_UTIL2D_TILEMAP_H

#include "../util3d/gl/VertexArrayObject.h"
#include "../util3d/gl/BufferObject.h"
#include "../util3d/gl/Texture.h"

namespace util2d {

class Tilemap
{
public:
	Tilemap(int x, int y, int width, int height);

	void draw() const;

	void setTexture(gl::Texture* tex);
	const gl::Texture* getTexture() const;

	void setTilemap(unsigned short* data, int width, int height);

	int x, y;

	static void initialize_shared();
	static void deinitialize_shared();

private:
	gl::Texture* texture;

	int tilemap_w, tilemap_h;

	gl::VertexArrayObject tile_vao;
	gl::BufferObject tile_vbo;
	gl::Texture tilemap;
};

} // namespace util2d

#endif // COMBOSHOOTER_UTIL2D_TILEMAP_H
