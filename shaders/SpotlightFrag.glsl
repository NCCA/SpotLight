#version 330 core

/// @brief[in] the vertex normal
in vec3 fragmentNormal;
/// @brief our output fragment colour
layout (location=0) out vec4 fragColour;
/// @brief material structure
struct Materials
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

// @brief light structure
struct Lights
{
		vec4 position;
		vec3 direction;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		float spotCosCutoff;
		float spotCosInnerCutoff;
		float spotExponent;
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
};
// @param material passed from our program
uniform Materials material;
/// @param lights passed from our program
#define numLights 8

uniform Lights light[numLights];
// our vertex position calculated in vert shader
in vec3 vPosition;


vec4 spotLight (int _lightNum)
{
		float nDotVP;       // normal * light direction
		float nDotR;        // normal * light reflection vector
		float pf=0;           // power factor
		float spotDot;      // cosine of angle between spotlight
		float spotAttenuation;     // spotlight attenuation factor;
		float attenuation;  // computed attenuation factor
		float d;            // distance from surface to light position
		vec3 VP;            // direction from surface to light position
		vec3 reflection;    // direction of maximum highlights

		// Compute vector from surface to light position
		VP = vec3 (light[_lightNum].position) - vPosition;

		// Compute distance between surface and light position
		d = length (VP);

		// Normalize the vector from surface to light position
		VP = normalize (VP);

		// Compute attenuation
		attenuation = 1.f / (light[_lightNum].constantAttenuation +
												 light[_lightNum].linearAttenuation * d +
												 light[_lightNum].quadraticAttenuation * d * d);

		// See if point on surface is inside cone of illumination
		spotDot = dot (-VP, normalize (light[_lightNum].direction));

		if (spotDot < light[_lightNum].spotCosCutoff)
		{
				spotAttenuation = 0.f;
		}
		else
		{
				// we are going to ramp from the outer cone value to the inner using
				// smoothstep to create a smooth value for the falloff
				float spotValue=smoothstep(light[_lightNum].spotCosCutoff,light[_lightNum].spotCosInnerCutoff,spotDot);
				spotAttenuation = pow (spotValue, light[_lightNum].spotExponent);
		}

		// Combine the spot and distance attenuation
		attenuation *= spotAttenuation;
		// calc the reflection for the highlight
		reflection = normalize (reflect (-normalize (VP), normalize(fragmentNormal)));

		nDotVP = max (0.f, dot (fragmentNormal, VP));
		nDotR = max (0.f, dot (normalize (fragmentNormal), reflection));
// the clamp code below is the same as this but removes the branch which is quicker
//    if (nDotVP == 0.f)
//        pf = 0.f;
//    else
//        pf = pow (nDotR, material.shininess);
		pf=clamp(nDotVP,0.0,pow (nDotR, material.shininess));
		// combine the light / material values
		vec4 ambient = material.ambient * light[_lightNum].ambient * attenuation;
		vec4 diffuse = material.diffuse * light[_lightNum].diffuse * nDotVP * attenuation;
		vec4 specular = material.specular * light[_lightNum].specular * pf * attenuation;

		return ambient + diffuse + specular;
}

void main(void)
{
    fragColour=vec4(0.1);
// loop for all the lights and add spotlight values
for (int i = 0; i < numLights; ++i)
{
    fragColour += spotLight (i);
}
}


