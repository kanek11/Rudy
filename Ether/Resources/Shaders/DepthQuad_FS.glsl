#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_DepthMap;
uniform float u_Near_plane;
uniform float u_Far_plane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float zValue = depth * 2.0 - 1.0; // Back to NDC 
    float near = u_Near_plane;
    float far = u_Far_plane;
    return (2.0 * near * far) / (far + near - zValue * (far - near));
}

void main()
{
    float depthValue = texture(u_DepthMap, TexCoords).r;
    FragColor = vec4(vec3(LinearizeDepth(depthValue) / u_Far_plane), 1.0); // perspective
    //FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}