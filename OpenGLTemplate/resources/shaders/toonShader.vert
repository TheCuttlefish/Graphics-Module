#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; 
uniform MaterialInfo material1; 



//my time
uniform float _t;
// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader -- using Gouraud (interpolated) shading
out vec3 vColour;	// Colour computed using reflectance model
out vec2 vTexCoord;	// Texture coordinate

out vec3 worldPosition;	// used for skybox

out float fIntensity;

uniform bool wobble;
// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	
	//cell shading!!
	vec3 h = normalize(v + s);
	float fDiffuseIntensity = max(dot(s, n), 0.0);
	vec3 diffuseColour = light1.Ld * material1.Md * fDiffuseIntensity;
	vec3 specularColour = vec3(0.0);
	float fSpecularIntensity = 0.0;

	if (fDiffuseIntensity > 0.0) {
		fSpecularIntensity = pow(max(dot(h, n), 0.0), material1.shininess);
		specularColour = light1.Ls * material1.Ms * fSpecularIntensity;
	}
	fIntensity  = fDiffuseIntensity + fSpecularIntensity;
	//cell sahding end!


	return ambient + diffuse + specular;

}



// This is the entry point into the vertex shader
void main()
{	



//cell!!
vec3 lightDir;
vec3 normal;
lightDir = -light1.position.xyz;
normal = inNormal;








// Save the world position for rendering the skybox
	worldPosition = inPosition;

	//jelly wobble
	vec3 p = inPosition;
	if(wobble){
	p.x-=cos(p.y-_t/5);
	p.y-=sin(p.x-_t/5);
	p.z-=sin(p.z-_t/5);
	}
	//p.x-=cos(100-_t);
	//p.y-=cos(100-_t);
	//p.z-=cos(100-_t);
	// Transform the vertex spatial position using 
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(p, 1.0f);
	//gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0f);
	
	// Get the vertex normal and vertex position in eye coordinates
	vec3 vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vec4 vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0f);
		
	// Apply the Phong model to compute the vertex colour
	vColour = PhongModel(vEyePosition, vEyeNorm);
	
	// Pass through the texture coordinate
	vTexCoord = inCoord;
} 
	