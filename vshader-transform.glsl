#version 150

in vec4 vPosition;
in vec3 vNormal;
in vec4 vColor;
in vec3 vAmbient;
in vec3 vDiffuse;
in vec3 vSpecular;
in vec4 vShininess;


in vec4 vAmbientDiffuseColor;

in vec4 vSpecularColor;
in float vSpecularExponent;


out vec4 color;

out vec3 normal;
out vec3 position;
out vec3 vN;

uniform mat4 model_view;
uniform mat4 projection;
uniform mat3 NormalMatrix;

uniform vec4 ambient_light;
uniform float Shininess;

void
main()
{
	vec4 vNormal = vec4(vNormal, 0.0);
    vec4 veyepos = model_view*vPosition;

	vN = normalize(model_view * vNormal).xyz;
	normal = vN;
	//spec = vec4(1,1,1,1);
	// same as: Projection*ModelView*vPosition
	
	gl_Position = projection * veyepos;
	//position = veyepos.xyz;
	position = vec3(model_view * vPosition);
	// not used
	color = vColor;//*ambient;//amb + diff + spec;
}