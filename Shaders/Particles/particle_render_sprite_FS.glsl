#version 460 core

out vec4 out_FragColor;

layout(location =0) in vec2 f_texCoords;

uniform vec4 u_color;    
uniform sampler2D u_sprite;  

void main()
{
    float alpha = texture(u_sprite, f_texCoords).r;
    out_FragColor = vec4(u_color.rgb, alpha * u_color.a);
    //out_FragColor = vec4(u_color.rgb, 1.0f);
}