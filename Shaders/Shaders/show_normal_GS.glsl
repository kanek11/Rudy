#version 460 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;
 

in VS_OUT
{
    vec3 world_position;
    vec3 normal;

} gs_in[];

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {

    for (int i = 0; i < 3; ++i) {
        // 输出原始顶点
        gl_Position = gl_in[i].gl_Position; 
        EmitVertex();

        // 输出沿法线方向的顶点
        float normalLength = 0.05;
        vec3 normalEnd = gs_in[i].world_position + normalize( gs_in[i].normal ) *  normalLength;
        vec4 clip_normalEnd = u_projection * u_view * vec4(normalEnd, 1.0);
        gl_Position = clip_normalEnd; 

        EmitVertex();

        EndPrimitive();
    }
}