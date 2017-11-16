#version 330 core
layout(location = 0) in vec3 vposition_m;
layout(location = 1) in vec3 vnormal_m;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPosition_w;

out vec3 position_w;
out vec3 normal_c;
out vec3 eyeDirection_c;
out vec3 lightDirection_c;

void main(){
    gl_Position = MVP * vec4(vposition_m, 1.0);
	
    position_w = (M*vec4(vposition_m,1.0)).xyz;
    normal_c = (V*M*vec4(vnormal_m,1.0)).xyz;
    eyeDirection_c = vec3(0,0,0)-(V*M*vec4(vposition_m,1)).xyz;
    lightDirection_c = (V*M*vec4(lightPosition_w-vposition_m,0)).xyz;

}
