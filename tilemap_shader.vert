#version 330

in vec2 in_TilePos;
uniform vec2 u_Pos;
uniform mat4 u_ScreenTransform;

noperspective out vec2 vf_TilePos;

void main(void) {
	gl_Position = u_ScreenTransform * vec4(u_Pos + in_TilePos, 0.f, 1.f);
	vf_TilePos = in_TilePos;
}
