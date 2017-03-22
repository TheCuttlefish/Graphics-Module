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

in vec3 n;
in vec4 p;

struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	//spotlight!
	vec3 direction;
	float exponent;
	float cutoff;
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
uniform LightInfo spotLight1; 
uniform LightInfo spotLight2; 
uniform MaterialInfo material1; 


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
	

	return ambient + diffuse + specular;

}


//spotlight!
vec3 BlinnPhongSpotlightModel(vec4 p, vec3 n, LightInfo light)
{
vec3 s = normalize(vec3(light.position - p));
float angle = acos(dot(-s, light.direction));
float cutoff = radians(clamp(light.cutoff, 0.0, 90.0));
vec3 ambient = light.La * material1.Ma;
if (angle < cutoff) {
float spotFactor = pow(dot(-s, light.direction), light.exponent);
vec3 v = normalize(-p.xyz);
vec3 h = normalize(v + s);
float sDotN = max(dot(s, n), 0.0);
vec3 diffuse = light.Ld * material1.Md * sDotN;
vec3 specular = vec3(0.0);
if (sDotN > 0.0)
specular = light.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
return ambient + spotFactor * (diffuse + specular);
} else
return ambient;
}


void main()
{

vec3 vColour =	BlinnPhongSpotlightModel(p, normalize(n),spotLight1) + 
				BlinnPhongSpotlightModel(p, normalize(n),spotLight2) +
				PhongModel(p, normalize(n));


	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	} else {

		// Get the texel colour from the texture sampler
			
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture){
		//vOutputColour = vTexColour*vec4(1,1,1,1);
			//vOutputColour = (vTexColour+vec4(.2,.2,.2,.1))*vec4(1,1,1,1);
			float alpha = 0.95f;
			vec3 myColour = vec3(1.0f);
			//myColour.r -= myColour.r-1.5-cos(vColour.z-_t/10);
			//myColour.b -= cos((myColour.b)-_t/10)-1;
			alpha =(0.95f+(cos(vColour.r-_t/10)/4));

			//-----------------------------time---effect amout
			myColour.g -=cos(myColour.g-_t/10)/8;
			myColour.b -=sin(myColour.b-_t/20)/2;

			vec3 damage = vec3(0,hitColour,0);
			

			myColour=myColour+damage;

			//if (vTexColour.g < fract(_t/50))
			//discard;

		//refl	
			vec3 ReflectDir = reflect(-n,p.xyz);
			vec4 cubeMapColor = normalize(texture(CubeMapTex,ReflectDir)+vec4(1,1,.9,-.5f));
			float ReflectFactor = 0.1f;
			vec4 FragColor = mix(vTexColour*vec4(myColour*vColour, alpha),cubeMapColor+vec4(.5,.5,.5,-.5f), ReflectFactor);
			//FragColor = mix(normalize(cubeMapColor+vec4(.5,.5,.5,-.5f)),vTexColour,alpha);
			
			vOutputColour = vTexColour*vec4(myColour*vColour-cubeMapColor.xyz, alpha);	// Combine object colour and texture 
			
		}else{
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
			}
	}
	
	
}
