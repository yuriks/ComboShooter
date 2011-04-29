#version 330

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

flat in vec4 vg_Transform[1];
flat in ivec2 vg_TexPos[1];
flat in ivec2 vg_TexSize[1];
flat in vec4 vg_Color[1];
uniform mat4 u_ScreenTransform;
uniform sampler2D u_Tex0;

out vec2 gf_TexCoord;
flat out vec4 gf_Color;

void main(void) {
	ivec2 tex_size = textureSize(u_Tex0, 0);
	// vec4(left, top, right, bottom)
	ivec4 tex_coords = ivec4(vg_TexPos[0], vg_TexPos[0] + vg_TexSize[0]);

	mat2 m = mat2(vg_Transform[0].xz, vg_Transform[0].yw);
	vec2 s = vg_TexSize[0] / 2.f;

	gl_Position = u_ScreenTransform * vec4((m * -s) + s + gl_in[0].gl_Position.xy, 0.f, 1.f);
	gf_TexCoord = tex_coords.xy;
	gf_Color = vg_Color[0];
	EmitVertex();

	gl_Position = u_ScreenTransform * vec4((m * vec2(s.x, -s.y)) + s + gl_in[0].gl_Position.xy, 0.f, 1.f);
	gf_TexCoord = tex_coords.zy;
	gf_Color = vg_Color[0];
	EmitVertex();

	gl_Position = u_ScreenTransform * vec4((m * vec2(-s.x, s.y)) + s + gl_in[0].gl_Position.xy, 0.f, 1.f);
	gf_TexCoord = tex_coords.xw;
	gf_Color = vg_Color[0];
	EmitVertex();

	gl_Position = u_ScreenTransform * vec4((m * s) + s + gl_in[0].gl_Position.xy, 0.f, 1.f);
	gf_TexCoord = tex_coords.zw;
	gf_Color = vg_Color[0];
	EmitVertex();

	EndPrimitive();
}
