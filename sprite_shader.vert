#version 330

in vec2 in_Pos;
in vec4 in_Transform;
in ivec2 in_TexPos;
in ivec2 in_TexSize;

out vec4 vg_Transform;
flat out ivec2 vg_TexPos;
flat out ivec2 vg_TexSize;

void main(void) {
	gl_Position = vec4(in_Pos, 0.f, 1.f);
	vg_Transform = in_Transform / 256.f;
	vg_TexPos = in_TexPos * 8;
	vg_TexSize = in_TexSize * 8;
}
