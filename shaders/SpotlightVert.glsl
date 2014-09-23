#version 400 core
/// @brief flag to indicate if model has unit normals if not normalize
uniform bool Normalize;
/// @brief the current fragment normal for the vert being processed
out vec3 fragmentNormal;
// the eye position of the camera
uniform vec3 viewerPos;
/// @brief the vertex passed in
layout (location =0) in vec3 inVert;
/// @brief the normal passed in
layout (location =2) in vec3 inNormal;
/// @brief the in uv
layout (location =1) in vec2 inUV;
out vec3 eyeDirection;
out vec3 eyeCord3;
struct Materials
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};


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

// our material
uniform Materials material;
// number of lights
#define numLights 8
// array of lights
uniform Lights light[numLights];
// the vertex position
out vec3 vPosition;
// Model * View matrix combined in app
uniform mat4 MV;
// Model View Projection shader combined in app
uniform mat4 MVP;
// our normal matrix
uniform mat3 normalMatrix;

void main()
{
// calculate the fragments surface normal
fragmentNormal = (normalMatrix*inNormal);
// if set normalize the fragment
if (Normalize == true)
{
  fragmentNormal = normalize(fragmentNormal);
}

// Get vertex position in eye coordinates
vec4 vertexPos = MV * vec4(inVert,1.0);
vec3 vertexEyePos = vertexPos.xyz / vertexPos.w;

vPosition = vertexEyePos;
// calculate the vertex position
gl_Position = MVP*vec4(inVert,1.0);

}
