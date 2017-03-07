#version 400 core

in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied

in vec3 worldPosition;
in float fIntensity;

//parsing colour
uniform vec3 setColour;
void main()
{
vec3 myColour;
					//vec3(.89,.75,.52);
vec3 sandColour = vec3(.85,.75,.6);
sandColour = setColour;
	 if(fIntensity>0.95){
			myColour = sandColour;
}else if(fIntensity>0.8){
			myColour = sandColour-.1;
}else if(fIntensity>0.7){
			myColour = sandColour-.15;
}else if(fIntensity>0.5){
			myColour = sandColour-.2;
}else if(fIntensity>0.3){
			myColour = sandColour-.25;
}else if(fIntensity>0.2){
			myColour = sandColour-.3;
}else if(fIntensity>0.1){
			myColour = sandColour-.35;
}else if(fIntensity>0.05){
			myColour = sandColour-.4;
}else if(fIntensity>0.03){
			myColour = sandColour-.45;
}




else{
			myColour = sandColour-.5;
}


	

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture){
		//vOutputColour = vTexColour*vec4(1,1,1,1);


			//vOutputColour = (vTexColour+vec4(.2,.2,.2,.1))*vec4(1,1,1,1);
			vOutputColour = vec4(myColour, 1.0f);	// Combine object colour and texture 
	
	}
	
	
}
