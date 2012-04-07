#version 400
in vec3 Position;
in vec3 Normal;
struct SpotLightInfo {
    vec4 position; // Position in eye coords.
    vec3 intensity; // Amb., Diff., and Specular intensity
    vec3 direction; // Normalized direction of the spotlight
    float exponent; // Angular attenuation exponent
    float cutoff; // Cutoff angle (between 0 and 90)
};
uniform SpotLightInfo Spot;
uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor
layout( location = 0 ) out vec4 FragColor;
vec3adsWithSpotlight( )
{
    vec3 s = normalize( vec3( Spot.position) - Position );
    float angle = acos( dot(-s, Spot.direction) );
    float cutoff = radians( clamp( Spot.cutoff, 0.0, 90.0 ) );
    vec3 ambient = Spot.intensity * Ka;
    if( angle < cutoff ) {
        float spotFactor = pow( dot(-s, Spot.direction),
                               Spot.exponent );
        vec3 v = normalize(vec3(-Position));
        vec3 h = normalize( v + s );
        return
        ambient +
        spotFactor * Spot.intensity * (
                                       Kd * max( dot(s, Normal), 0.0 ) +
                                       Ks * pow(max(dot(h,Normal), 0.0),Shininess));
    } else {
        return ambient;
    }
}
void main() {
    FragColor = vec4(adsWithSpotlight(), 1.0);
}