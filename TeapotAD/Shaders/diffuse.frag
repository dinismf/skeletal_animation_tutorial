#version 430

in vec3 vertPos;
in vec3 N;

uniform vec3 lightPos;
uniform vec3 lightIntensity;
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ka;	// Ambient reflectivity 
uniform vec3 Ks;	// Specular reflectivity
uniform float specularShininess;	// Specular shininess 

float dotRV;

layout( location = 0 ) out vec4 FragColour;

vec3 ads()
{
	vec3 n = normalize(N);
		
	vec3 l = normalize( vec3(lightPos) - vertPos);

	float df = max( dot(l,n), 0.0);
	float sf = 0.0f;

	if (df > 0.0f) 
	{
		vec3 v = normalize(vec3(vertPos));
		vec3 r = reflect(l, n);
		sf = max(dot(r,v), 0.0f);
	}

	return lightIntensity * ( Ka + Kd * df  + Ks * pow( sf, specularShininess ));
}

void main() {

   FragColour = vec4(ads(),1.0);

}