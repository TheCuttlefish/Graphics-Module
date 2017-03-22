#version 400 core

in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied

in vec3 worldPosition;
uniform float _t;
uniform bool moveTexture;
uniform bool calm;
uniform bool rays;
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




vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;

	vec3 tempMa = material1.Ma;
	vec3 tempMd = material1.Md;
	vec3 tempMs = material1.Ms;

	if(calm){
	//temp Ma and Ms
	tempMa=tempMs=vec3(1);
	//parsing light A as temp Mat D (to change catmullrom in day/night system)
	tempMd = light1.La;
	}

	vec3 ambient = light1.La * tempMa;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * tempMd * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * tempMs * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}





uniform float ambientControl;//1 to 0.2 day night







//fog start! - from OpenGL 4.0 Cookbook - page 100
uniform LightInfo Light;

struct FogInfo {
	float maxDist;
	float minDist;
	vec3 colour;
};

uniform FogInfo Fog;
vec3 Kd = light1.Ld; // Diffuse reflectivity
vec3 Ka = light1.La; // Ambient reflectivity
vec3 Ks = light1.Ls; // Specular reflectivity
float Shininess = 0.5f; // Specular shininess factor

vec3 ads( )
{
	vec3 s = normalize( Light.position.xyz - p.xyz );
	vec3 v = normalize(vec3(-p));
	vec3 h = normalize( v + s );
	vec3 ambient = Ka;
	vec3 diffuse = Kd * max(0.0, dot(s, n) );
	vec3 spec = Ks * pow( max( 0.0, dot( h, n) ), Shininess );
	return ambient + diffuse + spec;
}
//fog end!










void main(){

vec2 newTex = vTexCoord;
vec2 newTex2 = vTexCoord;
	if(rays){
		newTex.x-=cos(newTex.x/2+_t/50)/3;
		newTex.y-=sin(newTex.y/2+_t/50)/3;

		newTex2.x-=sin(newTex2.x/2+_t/30)/4;
		newTex2.y-=cos(newTex2.y/2+_t/30)/4;


	}


	//!fog!

float dist = abs(p.z);
float fogFactor = (Fog.maxDist - dist) / (Fog.maxDist - Fog.minDist);
fogFactor = clamp( fogFactor, 0.0, 1.0 );
vec3 shadeColour = ads();

//parsing all colours
vec3 allColours = BlinnPhongSpotlightModel(p, normalize(n),spotLight1) + 
				BlinnPhongSpotlightModel(p, normalize(n),spotLight2) +
				PhongModel(p, normalize(n));

//vec3 fogAndLight = mix( Fog.colour, allColours, fogFactor );

vec3 vColour = allColours;
//vec3 vColour = fogAndLight;
//!!fog!!


		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, newTex);	
		vec4 vTexColour2 = texture(sampler0, newTex2);
		//vec4 vTexColour = texture(sampler0, vTexCoord);	
		vec3 fogAndLight;
		if (bUseTexture)
			if(rays){
			vec4 rayColours = ((vTexColour*vTexColour2*10))*vec4(vColour, 1.0f*(.5f+ambientControl));
			fogAndLight = mix( Fog.colour, allColours*rayColours.rgb, fogFactor );
				vOutputColour = vec4(fogAndLight,rayColours.w*ambientControl*.7f);	
			}else{

			vec4 preColours =  vTexColour*vec4(vColour, 1.0f);	
			fogAndLight = mix( Fog.colour, allColours*preColours.rgb, fogFactor );
				vOutputColour =vec4(fogAndLight,preColours.w);
			}
			
		else
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
	
	
	
}
