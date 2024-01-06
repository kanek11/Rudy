#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

 
layout(location = 0) out vec2 f_texCoords;


uniform mat4 u_view;
uniform mat4 u_projection;

uniform float u_sprite_size;  

void main()
{
    vec4 view_pos = u_view *  gl_in[0].gl_Position ;

    float halfSize = u_sprite_size / 2.0;
    vec4 offset1 = vec4(-halfSize , -halfSize, 0.0, 0.0);
    vec4 offset2 = vec4( halfSize , -halfSize, 0.0, 0.0);
    vec4 offset3 = vec4(-halfSize ,  halfSize, 0.0, 0.0);
    vec4 offset4 = vec4( halfSize ,  halfSize, 0.0, 0.0);

    f_texCoords = vec2(0.0, 0.0);
    gl_Position = u_projection *  (  view_pos + offset1 );
    EmitVertex();                             
                                              
    f_texCoords = vec2(1.0, 0.0);             
    gl_Position = u_projection  * (  view_pos + offset2 );
    EmitVertex();                             
                                              
    f_texCoords = vec2(0.0, 1.0);             
    gl_Position = u_projection  * (  view_pos + offset3 );
    EmitVertex();                             
                                              
    f_texCoords = vec2(1.0, 1.0);             
    gl_Position = u_projection  * (  view_pos + offset4 );
    EmitVertex();

    EndPrimitive();

}