#version 150
in vec4 color;
in vec4 ambient;
in vec3 normal;
in vec3 position;
in vec3 vN;

out vec4  fColor;

uniform vec3 AmbientProduct, DiffiuseProduct, SpecularProduct;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 ambient_light;

uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Ks;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor

void main()
{
	vec3 L = normalize( light_position.xyz - position.xyz);
	vec3 E = normalize(-position.xyz);
	vec3 N = normalize(vN);

	vec3 H = normalize(L+E);
	vec4 amb = vec4((Kd + Ka),1.0) * ambient_light;
	vec4 diff = max(dot(L,N), 0.0) *  vec4((Kd + Ka),1.0) * light_color;
	vec4 spec = pow( max (dot(N,H), 0.0), Shininess) *  vec4(Ks,1.0) * light_color  ;
	if(dot(L,N) < 0.0){
		spec = vec4(0,0,0,1);
	}

	fColor = amb + diff + spec;

    //fColor = color;
	// test for normal values
	// same normal values should have same color value
	//fColor = vec4(normal, 1.0);//color * ambient;
	fColor = amb + diff + spec;
}