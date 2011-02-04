#version 330
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in  vec4 in_Position;
in  vec2 in_TexCoord;
uniform mat4 in_Proj;

// We output the ex_Color variable to the next shader in the chain
out vec2 ex_TexCoord;

void main(void) {
    gl_Position = in_Proj * in_Position;
    ex_TexCoord = in_TexCoord;
}
