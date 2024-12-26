/* quad vertex shader */
@vs vs
in vec4 position;
in vec4 color0;
in vec2 texcoord0;

out vec4 color;
out vec2 uv;

void main() {
    gl_Position = position;
    color = color0;
    uv = texcoord0;
}
@end

/* quad fragment shader */
@fs fs
in vec4 color;
in vec2 uv;
out vec4 frag_color;

void main() {
    frag_color = vec4(uv, 0, 1);
}
@end

/* quad shader program */
@program quad_uv vs fs

