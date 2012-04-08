#version 150
in vec4 color;
in vec4 ambient;
in vec3 normal;
in vec3 position;
in vec3 vN;

in vec3 fAmbient;
in vec3 fDiffuse;
in vec3 fSpecular;

out vec4  fColor;

uniform vec3 AmbientProduct, DiffiuseProduct, SpecularProduct;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;
uniform vec3 light_direction;
uniform vec3 light_intensity;
uniform float light_exponent;
uniform float light_cutoff;
uniform vec4 light_color;
uniform vec4 ambient_light;
uniform vec4 scene_light_color;

uniform vec4 rightHeadlight_position;
uniform vec3 rightHeadlight_direction;
uniform vec3 rightHeadlight_intensity;
uniform float rightHeadlight_exponent;
uniform float rightHeadlight_cutoff;
uniform vec4 rightHeadlight_color;

uniform vec4 policeLight1_position;
uniform vec3 policeLight1_direction;
uniform vec3 policeLight1_intensity;
uniform float policeLight1_exponent;
uniform float policeLight1_cutoff;
uniform vec4 policeLight1_color;

uniform vec4 policeLight2_position;
uniform vec3 policeLight2_direction;
uniform vec3 policeLight2_intensity;
uniform float policeLight2_exponent;
uniform float policeLight2_cutoff;
uniform vec4 policeLight2_color;

/* CAN'T USE ENUMS???
enum LIGHTS
{
	HEADLIGHT_LEFT,
	HEADLIGHT_RIGHT,
	POLICE_LIGHT_RED,
	POLICE_LIGHT_GREEN,
	NUMBER_OF_LIGHTS
};
*/
struct spotLightStruct {

	vec4 position;
	vec3 intensity;
	vec3 direction;
	vec4 color;
	float exponent;
	float cutoff;
};
uniform spotLightStruct[4];
uniform vec4 lightPositions[4];
uniform vec3 lightDirections[4];
uniform vec3 lightIntensities[4];
uniform float lightExponents[4];
uniform float lightCutoffs[4];
uniform vec4 lightColors[4];
uniform vec4 ambientLight[4];

uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Ks;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor



vec3 spotlight_function(spotLightStruct thisSpotlight)
{
	

	vec3 E = normalize(-position.xyz);
	vec3 N = normalize(vN);
	vec3 L = normalize( thisSpotlight.position.xyz - position.xyz);
	vec3 H = normalize(L+E);
	vec4 amb = vec4((Kd + Ka),1.0) * ambient_light;
	vec4 diff = max(dot(L,N), 0.0) *  vec4((Kd + Ka),1.0) * scene_light_color;
	vec4 spec = pow( max (dot(N,H), 0.0), Shininess) *  vec4(Ks,1.0) * scene_light_color  ;
	if(dot(L,N) < 0.0){
		spec = vec4(0,0,0,1);
	}
	

	vec3 s = normalize(vec3(thisSpotlight.position) - position);
	vec3 spotDir = normalize(thisSpotlight.direction);
	//SAME - float angle = acos(dot(-s,spotDir));
	float angle = acos( dot(-s, spotDir));
	float cutoff = radians( clamp(light_cutoff, 0.0, 90.0) );
	vec3 spot_ambient = light_intensity * Ka;

	if ( angle < cutoff) {
		float spotFactor = pow( dot(-s, spotDir), thisSpotlight.exponent);
		vec3 v = normalize(vec3(-position));
		vec3 h = normalize(v + s);

		return spot_ambient + spotFactor * thisSpotlight.intensity * 
			(
				Kd * max( dot(s, normal),0.0) +
				Ks * pow( max(dot(h,normal),0.0), Shininess)
			);
	}
	else
	{
		vec3 spot_ambient = ambient_light.xyz * Ka;
		return spot_ambient;
	}
}

void main()
{
	// test for normal values
	// same normal values should have same color value
	//fColor = vec4(normal, 1.0);//color * ambient;
	//fColor = amb + diff + spec;
	//SPOTLIGHT
	spotLightStruct currentSpotlight;
	currentSpotlight.position = light_position;
	currentSpotlight.direction = light_direction;
	currentSpotlight.intensity = light_intensity;
	currentSpotlight.exponent = light_exponent;
	currentSpotlight.color = light_color;
	currentSpotlight.cutoff = light_cutoff;
	currentSpotlight.exponent = light_exponent;

	fColor = vec4(spotlight_function(currentSpotlight), 1.0);
	currentSpotlight.position = rightHeadlight_position;
	currentSpotlight.direction = rightHeadlight_direction;
	//currentSpotlight.intensity = rightHeadlight_intensity;
	//currentSpotlight.exponent = rightHeadlight_exponent;

	fColor = fColor + vec4(spotlight_function(currentSpotlight), 1.0);

	currentSpotlight.position =		policeLight1_position;
	currentSpotlight.direction =	policeLight1_direction;
	currentSpotlight.color =		policeLight1_color;
	currentSpotlight.intensity =	policeLight1_intensity;
	//currentSpotlight.exponent =	policeLight1_exponent;

	fColor = fColor + vec4(spotlight_function(currentSpotlight), 1.0);

	currentSpotlight.position =		policeLight2_position;
	currentSpotlight.direction =	policeLight2_direction;
	currentSpotlight.color =		policeLight2_color;
	currentSpotlight.intensity =	policeLight2_intensity;
	//currentSpotlight.exponent =	policeLight2_exponent;

	fColor = fColor + vec4(spotlight_function(currentSpotlight), 1.0);
}