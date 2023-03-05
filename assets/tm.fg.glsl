#version 330 core
out vec4 FragColor;

uniform sampler2D   tile_texture;
uniform unsigned int tile_id;
uniform unsigned int tile_count_x;
uniform unsigned int tile_count_y;

in vec2 uv; 

vec2 invert(vec2 a)
{
    a.y = 1.0 - a.y;
    return a;
}

void main()
{
    vec2 tile_uv_size = 1.0 / vec2(tile_count_x, tile_count_y);
    vec2 tile_pos = vec2(tile_id % tile_count_x ,tile_id / tile_count_x)*tile_uv_size;
    vec2 tile_uv = tile_pos + uv * tile_uv_size;

    // Invert uv (in OpenGL, UV:[0,0] starts in bottom-left of the texture)
    //tile_uv.y = 1.0 - tile_uv.y;
    //tile_uv = invert(tile_uv);
    FragColor = vec4(1.0);
    FragColor = texture(tile_texture, tile_uv);
} 