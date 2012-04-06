#version 150

in vec4 vPosition;
in vec3 vNormal;
in vec4 vColor;
in vec4 vAmbient;
in vec4 vDiffuse;
in vec4 vSpecular;
in vec4 vShininess;


in vec4 vAmbientDiffuseColor;

in vec4 vSpecularColor;
in float vSpecularExponent;


out vec4 color;
out vec4 ambient;
out vec3 normal;
out vec3 position;
out vec3 vN;

uniform vec3 AmbientProduct, DiffiuseProduct, SpecularProduct;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 ambient_light;
uniform float Shininess;


void
main()
{
	vec4 vNormal = vec4(vNormal, 0.0);
    vec4 veyepos = model_view*vPosition;
	vec3 L = normalize( light_position.xyz - veyepos.xyz);
	vec3 E = normalize(-veyepos.xyz);
	vec3 H = normalize(L+E);

	vN = normalize(model_view * vNormal).xyz;

	//spec = vec4(1,1,1,1);
	// same as: Projection*ModelView*vPosition
	gl_Position = projection * veyepos;
	color = vColor;//*ambient;//amb + diff + spec;
	
}