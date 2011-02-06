#version 330

in vec2 gf_TexCoord;
flat in vec4 gf_Color;
uniform sampler2D u_Tex0;

out vec4 out_Color;

void main(void) {
	// Pass through our original color with full opacity.
	out_Color = texture2D(u_Tex0, gf_TexCoord.st) * gf_Color;
	//out_Color = vec4(gf_TexCoord, 1.f, 1.f);
}
