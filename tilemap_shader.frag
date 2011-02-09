#version 330

noperspective in vec2 vf_TilePos;
uniform sampler2D u_Tex0;
uniform usampler2D u_Tilemap;
uniform ivec2 u_Offset;

out vec4 out_Color;

void main(void) {
	ivec2 tile_pos = ivec2(vf_TilePos) + u_Offset;
	ivec2 pos_tile = (tile_pos >> 4) % textureSize(u_Tilemap, 0);
	ivec2 pos_offset = tile_pos & 0xF;
	uint tile_num = texelFetch(u_Tilemap, pos_tile, 0).r;
	uint tileset_width = uint(textureSize(u_Tex0, 0).x >> 4);
	uint div = tile_num / tileset_width;
	ivec2 tile_texel = ivec2(tile_num - (div * tileset_width), div) << 4;
	out_Color = texelFetch(u_Tex0, tile_texel + pos_offset, 0);
}
