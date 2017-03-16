#version 400 core

in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;
uniform float _t;
uniform float hitColour;//-0.6f
void main()
{

vec2 newTex = vTexCoord;

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, newTex);	
		//vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture){
		//vOutputColour = vTexColour*vec4(1,1,1,1);
			//vOutputColour = (vTexColour+vec4(.2,.2,.2,.1))*vec4(1,1,1,1);
			float alpha = 1.0f;
			vec3 myColour = vec3(1.0f);
			//myColour.r -= myColour.r-1.5-cos(vColour.z-_t/10);
			//myColour.b -= cos((myColour.b)-_t/10)-1;
			alpha =(1.1f+(cos(vColour.r-_t/10)/3));

			//-----------------------------time---effect amout
			myColour.g -=cos(myColour.g-_t/10)/8;
			myColour.b -=sin(myColour.b-_t/20)/2;

			vec3 damage = vec3(0,hitColour,0);
			

			myColour=myColour+damage;

			

			vOutputColour = vTexColour*vec4(myColour*vColour, alpha);	// Combine object colour and texture 
		}else{
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
			}
	}
	
	
}
