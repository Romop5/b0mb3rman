#version 330 core
layout (location = 0) in vec3 coord; // the position variable has attribute position 0

uniform vec4 quad;
uniform mat4 projection;

out vec2 uv;

void main()
{
    vec2 scale = vec2(quad[2] - quad[0], quad[3] - quad[1]);
    vec2 pos_2d = scale*coord.xy + quad.xy;
    gl_Position = projection*vec4(pos_2d,0.0,1.0);
    gl_Position.y = gl_Position.y*-1;
    uv = coord.xy; 
}