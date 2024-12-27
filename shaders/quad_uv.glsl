/* quad vertex shader */
@vs vs
in vec4 position;
in vec4 color0;
in vec4 bytes0;
in vec2 texcoord0;

out vec4 color;
out vec2 uv;
out vec4 bytes;

void main() {
    gl_Position = position;
    color = color0;
    uv = texcoord0;
    bytes = bytes0;
}
@end

/* quad fragment shader */
@fs fs
in vec4 color;
in vec2 uv;
in vec4 bytes;
out vec4 frag_color;

uniform layout(binding = 0) texture2D _texture0;
uniform layout(binding = 1) sampler texture0_smp;
#define texture0 sampler2D(_texture0, texture0_smp)

void main() {
    int texture_index = int(bytes.x * 255.0);

    vec4 texture_color = vec4(1.0);
    if (texture_index == 0){
        // this is text, it's only got the single .r channel so we stuff it into the alpha
        texture_color.a = texture(texture0, uv).r;
    }

    frag_color = texture_color * color;
}
@end

/* quad shader program */
@program quad_uv vs fs

