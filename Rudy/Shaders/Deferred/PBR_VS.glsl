#version 460 core

//===input
layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_TexCoords;

//===output
out VS_OUT
{
    vec2 TexCoords;

} vs_out;
 

void main()
{

    vs_out.TexCoords = in_TexCoords;
    gl_Position = vec4(in_Pos, 1.0); 

}
