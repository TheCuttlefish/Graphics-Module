#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

//my time
uniform float _t;
uniform bool moveTexture;
// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader -- using Gouraud (interpolated) shading

out vec2 vTexCoord;	// Texture coordinate

out vec3 worldPosition;	// used for skybox
uniform bool calm;
// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.

out vec3 n;
out vec4 p;

// This is the entry point into the vertex shader
void main()
{	

// Save the world position for rendering the skybox
	worldPosition = inPosition;


	//jelly wobble
	vec3 pos = inPosition;
	if(calm){
		pos.x-=cos(pos.y-_t/3)/3;//- default(catmullrom);
		pos.y-=sin(pos.x-_t/3)/3;
	}else{
		pos.x-=cos(pos.y-_t)/3;//- crazy(seaweed)
		pos.y-=sin(pos.x-_t)/3;
	}

	// Transform the vertex spatial position using 
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(pos, 1.0f);
	//gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0f);
	
	// Get the vertex normal and vertex position in eye coordinates
	vec3 vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vec4 vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0f);
	
	n = vEyeNorm;
	p = vEyePosition;

	
	// Pass through the texture coordinate
	vTexCoord = inCoord;
} 
	