#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 WorldCoords;
 
uniform mat4 u_ProjectionView;   //make sure the view matrix reduce to mat3, no translation part

void main()
{
    WorldCoords = aPos;
      
    vec4 pos = u_ProjectionView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;   
    //z is always 1;
    //st the skybox is always behind everything else with z = 1.0;  change the depth test to GL_LEQUAL in the host: glDepthFunc(GL_LEQUAL);


}