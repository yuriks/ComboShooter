#version 330

in vec2 in_Pos;
in vec4 in_Transform;
in ivec2 in_TexPos;
in ivec2 in_TexSize;
in vec4 in_Color;

flat out vec4 vg_Transform;
flat out ivec2 vg_TexPos;
flat out ivec2 vg_TexSize;
flat out vec4 vg_Color;

void main(void) {
	gl_Position = vec4(in_Pos, 0.f, 1.f);
	vg_Transform = in_Transform;
	vg_TexPos = in_TexPos * 8;
	vg_TexSize = in_TexSize * 8;
	vg_Color = in_Color;
}
