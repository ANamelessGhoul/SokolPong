/* quad vertex shader */
@vs vs
in vec3 position0;
in vec4 color0;

out vec4 color;

void main() {
    gl_Position = vec4(position0, 1.0);
    color = color0;
}
@end

/* quad fragment shader */
@fs fs
in vec4 color;
out vec4 fragColor;

void main() {
    fragColor = color;
}
@end

/* quad shader program */
@program quad vs fs

