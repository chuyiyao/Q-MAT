#version 330 core

in vec3 position_w;
in vec3 normal_c;
in vec3 eyeDirection_c;
in vec3 lightDirection_c;

uniform vec3 lightPosition_w;

out vec3 fcolor;

void main(){
        // Light emission properties
        vec3 lightColor = vec3(1,1,1);
        float lightPower = 50.0f;

        //Material properties;
        vec3 MaterialDiffuseColor = vec3(1.0,0.0,1.0);

        vec3 MaterialAmbientColor = vec3(0.25,0.25,0.25)*MaterialDiffuseColor;
        vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

        float distance = length(lightPosition_w-position_w);
        vec3 n = normalize(normal_c);
        vec3 l = normalize(lightDirection_c);
        float cosTheta = clamp(dot(n,l),0,1);

        vec3 E = normalize(eyeDirection_c);
        vec3 R = reflect(-l,n);
        float cosAlpha = clamp ( dot(E,R), 0,1);

        fcolor = MaterialDiffuseColor*lightColor*lightPower*cosTheta/(distance*distance)+
                         MaterialAmbientColor+
                         MaterialSpecularColor*lightColor*lightPower*pow(cosAlpha,5)/(distance*distance);
        //fcolor = MaterialDiffuseColor;
		//fcolor = vec3(1.0,0.0,1.0);
}
