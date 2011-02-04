#version 330

in  vec2 ex_TexCoord;
out vec4 out_Color;
uniform sampler2D in_Tex0;

void main(void) {
    // Pass through our original color with full opacity.
    out_Color = texture2D(in_Tex0, ex_TexCoord.st);
}
