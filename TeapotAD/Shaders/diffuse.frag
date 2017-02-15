#version 430

in vec3 vertPos;
in vec3 N;

struct SpotLightData {
	vec3 position; // Light position 
	vec3 intensity;
	vec3 direction;
	float exponent;
	float cutoff;
};
uniform SpotLightData Spotlight;

uniform vec3 Ka;	// Ambient reflectivity 
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ks;	// Specular reflectivity
uniform float specularShininess;	// Specular shininess 

layout( location = 0 ) out vec4 FragColour;

vec3 phongWithSpotlight();

void main() {
   FragColour = vec4(phongWithSpotlight(), 1.0);
}

vec3 phongWithSpotlight()
{
	vec3 s = normalize( vec3(Spotlight.position) - vertPos ); // Vector from light to vertex
    
	float angle = acos( dot(-s, Spotlight.direction) ); // Angle between
    
	float cutoff = radians( clamp(Spotlight.cutoff, 0.0, 90.0) ); // Cutoff angle clamped between 0 and 90 and  converted to radians.

	vec3 ambientComponent = Spotlight.intensity * Ka;		

	if (angle < cutoff) {
		float spotFactor = pow( dot(-s, Spotlight.direction), Spotlight.exponent);
		
		vec3 v = normalize(-vertPos);
		vec3 h = normalize(v + s);

		return ambientComponent + spotFactor * Spotlight.intensity * (Kd * max(dot(s, N), 0.0) + Ks * pow(max(dot(h, N), 0.0), specularShininess));
	} else {
		return ambientComponent;  
	}
}