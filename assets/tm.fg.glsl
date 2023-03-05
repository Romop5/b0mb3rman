#version 330 core
out vec4 FragColor;

uniform sampler2D   tile_texture;
uniform unsigned int tile_id;
uniform unsigned int tile_count_x;
uniform unsigned int tile_count_y;

// UV with quad (<0,1>x<0,1> for the whole quad)
in vec2 uv; 

vec2 invert(vec2 a)
{
    a.y = 1.0 - a.y;
    return a;
}

void main()
{
    ivec2 tex_size = textureSize(tile_texture,0);
    ivec2 tile_uv_size = tex_size / ivec2(tile_count_x, tile_count_y);

    ivec2 inside_tile_uv = ivec2(uv * vec2(tile_uv_size));

    // Hack: uv inside tile should always be in <0,tile_width-1> and <0, tile_height-1> interval
    inside_tile_uv = clamp(inside_tile_uv, ivec2(0), tile_uv_size-1);

    ivec2 tile_pos_uv = ivec2(tile_id % tile_count_x,tile_id / tile_count_x)*tile_uv_size;
    ivec2 texture_uv = tile_pos_uv + inside_tile_uv;

    // Invert v: OpenGL indexes the texture from the bottom-left corner as (0,0)
    texture_uv.y = tex_size.y - texture_uv.y-1;

    FragColor = texelFetch(tile_texture, texture_uv,0);
} 