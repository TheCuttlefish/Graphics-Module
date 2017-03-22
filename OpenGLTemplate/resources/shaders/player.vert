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

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader -- using Gouraud (interpolated) shading

out vec2 vTexCoord;	// Texture coordinate


out vec3 worldPosition;	// used for skybox
uniform float jellyScale;
uniform float hitColour;//red
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
	
		//ripple
		pos.y+=sin(pos.z*10+_t/2);
		
		//up and down
		pos.y+=cos(_t/4)*5;

		//scale
		pos.x += (.9+pos.x*.1)*cos(_t/2);
		pos.z += (.9+pos.z*.1)*cos(_t/2);
		//player input scale
		pos.x = (pos.x*jellyScale);
		pos.z = (pos.z*jellyScale);

		//damage animation
		pos.x -=cos((pos.y*(hitColour))/3+_t/100)*5;
		pos.z -=sin((pos.y*(hitColour))/3+_t/100)*5;
		

		//p.x -=cos(p.y*(hitColour)+_t/100)*2;
		//p.z -=cos(p.y*(hitColour)+_t/100)*2;

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
	