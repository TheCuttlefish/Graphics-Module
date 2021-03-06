/*
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting,
 different camera controls, different shaders, etc.

 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk)
*/


#include "game.h"
//z
#include "CatmullRom.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "Cube.h"//added my cube
#include "Tetrahedron.h"//added my tetrahedron
#include "Icosahedron.h"// Icosahedron

glm::vec3 p0 = glm::vec3(-500, 10, -200);
glm::vec3 p1 = glm::vec3(0, 10, -200);
glm::vec3 p2 = glm::vec3(0, 10, 200);
glm::vec3 p3 = glm::vec3(-500, 10, 200);

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	//z
	m_grid = NULL;
	m_pJellyFish = NULL;
	m_pSeaweed = NULL;
	m_pRocks = NULL;
	m_pCave = NULL;
	m_pUrchin = NULL;
	m_pPlayer = NULL;

	m_pSphere = NULL;
	m_pCube = NULL;
	m_pTetrahedron = NULL;
	m_pIcosahedron = NULL;

	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
}

// Destructor
Game::~Game()
{
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	//zhan- import
	delete m_grid;

	delete m_pJellyFish;
	delete m_pSeaweed;
	delete m_pRocks;
	delete m_pCave;
	delete m_pUrchin;
	delete m_pPlayer;

	delete m_pSphere;
	delete m_pCube;
	delete m_pTetrahedron;
	delete m_pIcosahedron;

	delete m_pAudio;
	delete m_pCatmullRom;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise()
{

	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	//zhan- import
	m_grid = new CPlane;

	m_pJellyFish = new COpenAssetImportMesh;
	m_pSeaweed = new COpenAssetImportMesh;
	m_pRocks = new COpenAssetImportMesh;
	m_pCave = new COpenAssetImportMesh;
	m_pUrchin = new COpenAssetImportMesh;
	m_pPlayer = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pCube = new CCube;
	m_pTetrahedron = new CTetrahedron;
	m_pIcosahedron = new CIcosahedron;

	m_pAudio = new CAudio;

	//cat
	m_pCatmullRom = new CCatmullRom;
	//m_pCatmullRom->SetTexture("resources\\textures\\seafloor2.png");
	//m_pCatmullRom->SetTexture("resources\\textures\\flow.png");
	m_pCatmullRom->SetTexture("resources\\textures\\flow5.png");
	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();

	//current distance
	m_currentDistance = 0.0f;
	//cam view
	m_camViewType = false; //cam types
	m_inputSpeed = 0.0f; //speed of the player
	m_limitInput = false;// timer to limit input
	m_playerScale = 1.0f;// player's scale
	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height);
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	//my shader ( order is important!)
	sShaderFileNames.push_back("jellyShader.vert");
	sShaderFileNames.push_back("jellyShader.frag");
	sShaderFileNames.push_back("toonShader.vert");
	sShaderFileNames.push_back("toonShader.frag");
	sShaderFileNames.push_back("player.vert");
	sShaderFileNames.push_back("player.frag");
	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	//Adding my jelly shader!
	CShaderProgram *pJellyProgram = new CShaderProgram;
	pJellyProgram->CreateProgram();
	pJellyProgram->AddShaderToProgram(&shShaders[4]);
	pJellyProgram->AddShaderToProgram(&shShaders[5]);
	pJellyProgram->LinkProgram();
	m_pShaderPrograms->push_back(pJellyProgram);

	//Adding my toon shader!
	CShaderProgram *pToonProgram = new CShaderProgram;
	pToonProgram->CreateProgram();
	pToonProgram->AddShaderToProgram(&shShaders[6]);
	pToonProgram->AddShaderToProgram(&shShaders[7]);
	pToonProgram->LinkProgram();
	m_pShaderPrograms->push_back(pToonProgram);

	//Adding my player shader!
	CShaderProgram *pPlayerProgram = new CShaderProgram;
	pPlayerProgram->CreateProgram();
	pPlayerProgram->AddShaderToProgram(&shShaders[8]);
	pPlayerProgram->AddShaderToProgram(&shShaders[9]);
	pPlayerProgram->LinkProgram();
	m_pShaderPrograms->push_back(pPlayerProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);

	// Create the planar terrain                      "seafloor3.png"
	m_pPlanarTerrain->Create("resources\\textures\\", "seafloor3.png", 2000.0f, 2000.0f, 20.0f); // Texture made myself (25 Feb 2017)
	m_grid->Create("resources\\textures\\", "sun_rays.png", 2000.0f, 2000.0f, 100.0f);// Texture made myself (19 Mar 2017)

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	//Zhan's Objects
	m_pJellyFish->Load("resources\\models\\Jelly\\jellyfish3.obj");// Made myself (25 Feb 2017)
	m_pSeaweed->Load("resources\\models\\Seaweed\\seaweed7.obj");// Made myself (25 Feb 2017)
	m_pRocks->Load("resources\\models\\Rocks\\rocks.obj");// Made myself (25 Feb 2017)
	m_pCave->Load("resources\\models\\Cave\\cave.obj");// Made myself (25 Feb 2017)
	m_pUrchin->Load("resources\\models\\SeaUrchin\\seaUrchin.obj");// Made myself (06 Mar 2017)
	m_pPlayer->Load("resources\\models\\Jelly\\player.obj");// Made myself (13 Mar 2017)
	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013


	// my cube
	m_pCube->Create("resources\\textures\\seafloor3.png");// Made myself (11 Mar 2017)

	// my Tetrahedron
	m_pTetrahedron->Create("resources\\textures\\seafloor3.png");// Made myself (11 Mar 2017)

	//my Icosahedron
	m_pIcosahedron->Create("resources\\textures\\seafloor3.png");// Made myself (11 Mar 2017)

	//m_pCatmullRom->Create


	glEnable(GL_CULL_FACE);


	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("Resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("Resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//z - i removed sound
	//m_pAudio->PlayMusicStream();





}






// Render method runs repeatedly in a loop
void Game::Render()
{
	glEnable(GL_MULTISAMPLE);
	//glDisable(GL_MULTISAMPLE);
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();


	// Use the main shader program
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10;
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack.
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);


	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(0,100,0, 1); // Position of light source *in world coordinates*
		pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
		pMainProgram->SetUniform("light1.La", glm::vec3(m_globalLight));	//1	// Ambient colour of light
		pMainProgram->SetUniform("light1.Ld", glm::vec3(m_globalLight));	//1	// Diffuse colour of light
		pMainProgram->SetUniform("light1.Ls", glm::vec3(m_globalLight));	//1	// Specular colour of light

		pMainProgram->SetUniform("material1.Ma", glm::vec3(m_globalLight-0.1f));	// 0 Ambient material reflectance
		pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f-m_globalLight*0.9));	// 1 Diffuse material reflectance
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.5f));	// 0.5 Specular material reflectance

		pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
		//spot light things!
		pMainProgram->SetUniform("light1.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
		pMainProgram->SetUniform("light1.exponent", 20.0f);// - strengths 1 =very , 1000000 not
		pMainProgram->SetUniform("light1.cutoff", 30.0f);//30 - angle

		
	glm::vec4 spotLightPos1 = glm::vec4(100, 120, 0, 1);//spotlight1
		pMainProgram->SetUniform("spotLight1.position", viewMatrix*spotLightPos1); // Position of light source *in eye coordinates*
		pMainProgram->SetUniform("spotLight1.La", glm::vec3(m_colour1));	//1	// Ambient colour of light
		pMainProgram->SetUniform("spotLight1.Ld", glm::vec3(m_colour1));	//1	// Diffuse colour of light
		pMainProgram->SetUniform("spotLight1.Ls", glm::vec3(m_colour1));	//1	// Specular colour of light
		pMainProgram->SetUniform("spotLight1.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
		pMainProgram->SetUniform("spotLight1.exponent", 10.0f);// - strengths 1 =very , 1000000 not
		pMainProgram->SetUniform("spotLight1.cutoff", 150.0f);//30 - angle

	glm::vec4 spotLightPos2 = glm::vec4(-100, 180, 0, 1);//spotlight2
		pMainProgram->SetUniform("spotLight2.position", viewMatrix*spotLightPos2); // Position of light source *in eye coordinates*
		pMainProgram->SetUniform("spotLight2.La", glm::vec3(m_colour2));	//1	// Ambient colour of light
		pMainProgram->SetUniform("spotLight2.Ld", glm::vec3(m_colour2));	//1	// Diffuse colour of light
		pMainProgram->SetUniform("spotLight2.Ls", glm::vec3(m_colour2));	//1	// Specular colour of light
		pMainProgram->SetUniform("spotLight2.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
		pMainProgram->SetUniform("spotLight2.exponent", 10.0f);// - strengths 1 =very , 1000000 not
		pMainProgram->SetUniform("spotLight2.cutoff", 100.0f);//30 - angle
		//fog ---from OpenGL 4.0 cookbook
		pMainProgram->SetUniform("Fog.maxDist", m_fogDist);//where to add full fog
		pMainProgram->SetUniform("Fog.minDist", 0.0f);//when to start fog
		pMainProgram->SetUniform("Fog.colour", m_fogColour);// fog colour
	//--my shader!!





	// Render the skybox and terrain with full ambient reflectance
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		//I moved skybox up! + 500
		modelViewMatrixStack.Translate(vEye+glm::vec3(0,500,0));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	 //Render the planar terrain
		 modelViewMatrixStack.Push();
	 	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	 	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));

	 	m_pPlanarTerrain->Render();
	 modelViewMatrixStack.Pop();


	 /*
	 horses
	for(int i = 0; i<20 ; i++){
	// Render the horse
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 5.0f*i));
	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
	modelViewMatrixStack.Scale(2.5f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();

	}
	*/





	// Render the barrel
	// modelViewMatrixStack.Push();
	// //using normal sahder again!
	// pMainProgram->UseProgram();
	// 	modelViewMatrixStack.Translate(glm::vec3(100.0f, 0.0f, 0.0f));
	// 	modelViewMatrixStack.Scale(5.0f);
	// 	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	// 	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// 	m_pBarrelMesh->Render();
	// modelViewMatrixStack.Pop();


	// Render the sphere
	// modelViewMatrixStack.Push();
	// 	modelViewMatrixStack.Translate(glm::vec3(0.0f, 80.0f, 0.0f));
	// 	modelViewMatrixStack.Scale(5.0f);
	// 	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	// 	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// 	// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
	// 	pMainProgram->SetUniform("bUseTexture", false);
	//
	// 	m_pSphere->Render();
	// modelViewMatrixStack.Pop();

	//cube

	// Render the cube
	 modelViewMatrixStack.Push();
	 	modelViewMatrixStack.Translate(glm::vec3(110.0f, 5.0f, 50.0f));
	 	modelViewMatrixStack.Scale(5.0f);
	 	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	 	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	 	m_pCube->Render();
	 modelViewMatrixStack.Pop();

	//ICOSAHEDRON
	 modelViewMatrixStack.Push();
	 modelViewMatrixStack.Translate(glm::vec3(130.0f, 5.0f, 50.0f));
	 modelViewMatrixStack.Scale(5.0f);
	 pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	 pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	 m_pIcosahedron->Render();
	 modelViewMatrixStack.Pop();



	//TERTRAHEDRON
	 modelViewMatrixStack.Push();
	 	modelViewMatrixStack.Translate(glm::vec3(150.0f, 5.0f, 50.0f));
	 	modelViewMatrixStack.Scale(5.0f);
	 	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	 	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	 	m_pTetrahedron->Render();
	 modelViewMatrixStack.Pop();

	//--original pos of catmullrom

	//render my objects!
	CShaderProgram *pToonProgram = (*m_pShaderPrograms)[3];
	pToonProgram->UseProgram();
	pToonProgram->SetUniform("_t", m_time);
	pToonProgram->SetUniform("bUseTexture", true);
	pToonProgram->SetUniform("sampler0", 0);
	pToonProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	pToonProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	pToonProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pToonProgram->SetUniform("light1.La", glm::vec3(m_globalLight));		// Ambient colour of light
	pToonProgram->SetUniform("light1.Ld", glm::vec3(m_globalLight));		// Diffuse colour of light
	pToonProgram->SetUniform("light1.Ls", glm::vec3(m_globalLight));		// Specular colour of light
	pToonProgram->SetUniform("material1.Ma", glm::vec3(m_globalLight/2));	// Ambient material reflectance
	pToonProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pToonProgram->SetUniform("material1.Ms", glm::vec3(0.5f));	// Specular material reflectance
	pToonProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
	pToonProgram->SetUniform("setColour", glm::vec3(.5, .5, .6));
	pToonProgram->SetUniform("wobble", true);


	//spotlight1
	pToonProgram->SetUniform("spotLight1.position", viewMatrix*spotLightPos1); // Position of light source *in eye coordinates*
	pToonProgram->SetUniform("spotLight1.La", glm::vec3(m_colour1));	//1	// Ambient colour of light
	pToonProgram->SetUniform("spotLight1.Ld", glm::vec3(m_colour1));	//1	// Diffuse colour of light
	pToonProgram->SetUniform("spotLight1.Ls", glm::vec3(m_colour1));	//1	// Specular colour of light
	pToonProgram->SetUniform("spotLight1.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
	pToonProgram->SetUniform("spotLight1.exponent", 10.0f);// - strengths 1 =very , 1000000 not
	pToonProgram->SetUniform("spotLight1.cutoff", 150.0f);//30 - angle

															//spotlight2
	pToonProgram->SetUniform("spotLight2.position", viewMatrix*spotLightPos2); // Position of light source *in eye coordinates*
	pToonProgram->SetUniform("spotLight2.La", glm::vec3(m_colour2));	//1	// Ambient colour of light
	pToonProgram->SetUniform("spotLight2.Ld", glm::vec3(m_colour2));	//1	// Diffuse colour of light
	pToonProgram->SetUniform("spotLight2.Ls", glm::vec3(m_colour2));	//1	// Specular colour of light
	pToonProgram->SetUniform("spotLight2.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
	pToonProgram->SetUniform("spotLight2.exponent", 10.0f);// - strengths 1 =very , 1000000 not
	pToonProgram->SetUniform("spotLight2.cutoff", 100.0f);//30 - angle

	//fog ---from OpenGL 4.0 cookbook
	pToonProgram->SetUniform("Fog.maxDist", m_fogDist);//where to add full fog
	pToonProgram->SetUniform("Fog.minDist", 0.0f);//when to start fog
	pToonProgram->SetUniform("Fog.colour", m_fogColour);// fog colour

	//urchin


	 //set that to p!!!
	 glm::vec3 pos = glm::vec3(0);
	 glm::vec3 move = glm::vec3(0,-2+cos(m_time/3)*4, 0);


	 for (int i = 0; i < m_enemyCount-1; i++) {
		 m_enemyPos[i] = pos + move;
		 modelViewMatrixStack.Push();
		 m_pCatmullRom->Sample(40.0f*i, pos);
		 modelViewMatrixStack.Translate(pos + move);
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 60.0f);

		 modelViewMatrixStack.Scale(m_enemyScale);
		 pToonProgram->SetUniform("setColour", glm::vec3(.3, 0, 0));
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));

		 float dist = glm::distance(m_currentPlayerPos, pos+move);
		 CheckDistance(dist);

		 m_pUrchin->Render();
		modelViewMatrixStack.Pop();

	 }

	 pToonProgram->SetUniform("wobble", false);
	//cave
	 modelViewMatrixStack.Push();
	 	modelViewMatrixStack.Translate(glm::vec3(100.0f, 0.0f, -10.0f));
	 	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 119.0f);
	 	modelViewMatrixStack.Scale(0.5f);                 //.9, .85, .8
		pToonProgram->SetUniform("setColour", glm::vec3(.85, .822, .8 ));
	 	pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	 	pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	 	m_pCave->Render();
	 modelViewMatrixStack.Pop();


	 modelViewMatrixStack.Push();
	 modelViewMatrixStack.Translate(glm::vec3(-30.0f, 0.0f, -95.0f));
	 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), -128.0f);
	 modelViewMatrixStack.Scale(0.65f);                 //.9, .85, .8
	 pToonProgram->SetUniform("setColour", glm::vec3(.85, .822, .8));
	 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	 m_pCave->Render();
	 modelViewMatrixStack.Pop();

	// rocks
	 modelViewMatrixStack.Push();
	 	modelViewMatrixStack.Translate(glm::vec3(50.0f, 0.0f, 100.0f));
	 	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
	 	modelViewMatrixStack.Scale(2.0f);
	 	pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	 	pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	 	m_pRocks->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-100.0f, 0.0f, -200.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90.0f);
		 modelViewMatrixStack.Scale(2.0f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();


	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-50.0f, 0.0f, -50.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 50.0f);
		 modelViewMatrixStack.Scale(1.3f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(300.0f, 0.0f, 210.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 190.0f);
		 modelViewMatrixStack.Scale(4.3f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(300.0f, 0.0f, -210.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);
		 modelViewMatrixStack.Scale(3.3f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(350.0f, 0.0f, 0.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90.0f);
		 modelViewMatrixStack.Scale(2.3f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();


	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-300.0f, 0.0f, 210.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 190.0f);
		 modelViewMatrixStack.Scale(4.3f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-300.0f, 0.0f, -210.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);
		 modelViewMatrixStack.Scale(3.3f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-350.0f, 0.0f, 0.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90.0f);
		 modelViewMatrixStack.Scale(2.3f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-220.0f, 0.0f, 0.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
		 modelViewMatrixStack.Scale(1.5f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-200.0f, 0.0f, -100.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 60.0f);
		 modelViewMatrixStack.Scale(0.5f);
		 pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pRocks->Render();
	 modelViewMatrixStack.Pop();

	//glCullFace(GL_FRONT_AND_BACK);



	//glEnable(GL_BLEND);



	CShaderProgram *pJellyProgram = (*m_pShaderPrograms)[2];
	pJellyProgram->UseProgram();
	pJellyProgram->SetUniform("_t", m_time);
	pJellyProgram->SetUniform("bUseTexture", true);
	pJellyProgram->SetUniform("sampler0", 0);
	pJellyProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	pJellyProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	pJellyProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pJellyProgram->SetUniform("light1.La", glm::vec3(m_globalLight));		// Ambient colour of light
	pJellyProgram->SetUniform("light1.Ld", glm::vec3(m_globalLight));		// Diffuse colour of light
	pJellyProgram->SetUniform("light1.Ls", glm::vec3(m_globalLight));		// Specular colour of light
														//(.2f+m_globalLight*0.2f));
	pJellyProgram->SetUniform("material1.Ma", glm::vec3(.2f + m_globalLight*0.8f));	// Ambient material reflectance
	pJellyProgram->SetUniform("material1.Md", glm::vec3(1- m_globalLight*0.9f));	// Diffuse material reflectance
	pJellyProgram->SetUniform("material1.Ms", glm::vec3(0.5f));	// Specular material reflectance


		// Ambient material reflectance
	//pPlayerProgram->SetUniform("material1.Md", glm::vec3(.5f + m_globalLight*0.5f));	// Diffuse material reflectance
	//pPlayerProgram->SetUniform("material1.Ms", glm::vec3(0.5f));






	pJellyProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
	pJellyProgram->SetUniform("rays", false);
	pJellyProgram->SetUniform("calm", true);
	pJellyProgram->SetUniform("ambientControl", m_globalLight);


	//spotlight1
	pJellyProgram->SetUniform("spotLight1.position", viewMatrix*spotLightPos1); // Position of light source *in eye coordinates*
	pJellyProgram->SetUniform("spotLight1.La", glm::vec3(m_colour1));	//1	// Ambient colour of light
	pJellyProgram->SetUniform("spotLight1.Ld", glm::vec3(m_colour1));	//1	// Diffuse colour of light
	pJellyProgram->SetUniform("spotLight1.Ls", glm::vec3(m_colour1));	//1	// Specular colour of light
	pJellyProgram->SetUniform("spotLight1.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
	pJellyProgram->SetUniform("spotLight1.exponent", 10.0f);// - strengths 1 =very , 1000000 not
	pJellyProgram->SetUniform("spotLight1.cutoff", 150.0f);//30 - angle

	//spotlight2
	pJellyProgram->SetUniform("spotLight2.position", viewMatrix*spotLightPos2); // Position of light source *in eye coordinates*
	pJellyProgram->SetUniform("spotLight2.La", glm::vec3(m_colour2));	//1	// Ambient colour of light
	pJellyProgram->SetUniform("spotLight2.Ld", glm::vec3(m_colour2));	//1	// Diffuse colour of light
	pJellyProgram->SetUniform("spotLight2.Ls", glm::vec3(m_colour2));	//1	// Specular colour of light
	pJellyProgram->SetUniform("spotLight2.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
	pJellyProgram->SetUniform("spotLight2.exponent", 10.0f);// - strengths 1 =very , 1000000 not
	pJellyProgram->SetUniform("spotLight2.cutoff", 100.0f);//30 - angle

	 //fog ---from OpenGL 4.0 cookbook
	pJellyProgram->SetUniform("Fog.maxDist", m_fogDist);//where to add full fog
	pJellyProgram->SetUniform("Fog.minDist", 0.0f);//when to start fog
	pJellyProgram->SetUniform("Fog.colour", m_fogColour);// fog colour



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_SRC1_RGB);
	//Render grid
	pJellyProgram->SetUniform("rays", true);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0, 1, 0));
		//modelViewMatrixStack.Rotate(glm::vec3(0.0f, 0.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 0.1f, 0.0f), 180.0f);
		pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_grid->Render();
	modelViewMatrixStack.Pop();
	pJellyProgram->SetUniform("rays", false);


	//Render a jellyFish
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0, 5, 0));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pJellyFish->Render();
	modelViewMatrixStack.Pop();


	pJellyProgram->SetUniform("calm", false);

	//Render my seaweed
	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(110.0f, 0.0f, 0.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.2f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(190.0f, 0.0f, 0.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.3f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(40.0f, 0.0f, 40.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.2f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();


	modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-100.0f, 0.0f, 100.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.2f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-40.0f, 0.0f, 90.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.3f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-30.0f, 0.0f, 70.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.1f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(40.0f, 0.0f, 0.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.3f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-30.0f, 0.0f, 10.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.3f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-60.0f, 0.0f, 0.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.3f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();

	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(glm::vec3(-90.0f, 0.0f, 20.0f));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f);
		 modelViewMatrixStack.Scale(0.4f);
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		 m_pSeaweed->Render();
	 modelViewMatrixStack.Pop();


	//to show back faces of the wave
	 glDisable(GL_CULL_FACE);
	 //catmullrom
	 pJellyProgram->SetUniform("calm", true);
	 modelViewMatrixStack.Push();
		 pJellyProgram->SetUniform("bUseTexture", true); // turn off texturing
		 pJellyProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pJellyProgram->SetUniform("matrices.normalMatrix",
			 m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));

		 m_pCatmullRom->RenderCentreline();
		 m_pCatmullRom->RenderOffsetCurves();
	 modelViewMatrixStack.Pop();



	 CShaderProgram *pPlayerProgram = (*m_pShaderPrograms)[4];
	 pPlayerProgram->UseProgram();
	 pPlayerProgram->SetUniform("_t", m_time);
	 pPlayerProgram->SetUniform("bUseTexture", true);
	 pPlayerProgram->SetUniform("sampler0", 0);
	 pPlayerProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	 pPlayerProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	 pPlayerProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	 pPlayerProgram->SetUniform("light1.La", glm::vec3(0.5f+m_globalLight*0.5f));		// Ambient colour of light
	 pPlayerProgram->SetUniform("light1.Ld", glm::vec3(m_globalLight));		// Diffuse colour of light
	 pPlayerProgram->SetUniform("light1.Ls", glm::vec3(m_globalLight));		// Specular colour of light
	 pPlayerProgram->SetUniform("material1.Ma", glm::vec3(.5f + m_globalLight*0.5f));	// Ambient material reflectance
	 pPlayerProgram->SetUniform("material1.Md", glm::vec3(.5f+m_globalLight*0.5f));	// Diffuse material reflectance
	 pPlayerProgram->SetUniform("material1.Ms", glm::vec3(0.5f));	// Specular material reflectance
	 pPlayerProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
	 pPlayerProgram->SetUniform("renderSkybox", false);
	 pPlayerProgram->SetUniform("jellyScale", m_playerScale);
	 pPlayerProgram->SetUniform("hitColour", m_hitColour);


	 //spotlight1
	 pPlayerProgram->SetUniform("spotLight1.position", viewMatrix*spotLightPos1); // Position of light source *in eye coordinates*
	 pPlayerProgram->SetUniform("spotLight1.La", glm::vec3(m_colour1));	//1	// Ambient colour of light
	 pPlayerProgram->SetUniform("spotLight1.Ld", glm::vec3(m_colour1));	//1	// Diffuse colour of light
	 pPlayerProgram->SetUniform("spotLight1.Ls", glm::vec3(m_colour1));	//1	// Specular colour of light
	 pPlayerProgram->SetUniform("spotLight1.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
	 pPlayerProgram->SetUniform("spotLight1.exponent", 10.0f);// - strengths 1 =very , 1000000 not
	 pPlayerProgram->SetUniform("spotLight1.cutoff", 150.0f);//30 - angle

															//spotlight2
	 pPlayerProgram->SetUniform("spotLight2.position", viewMatrix*spotLightPos2); // Position of light source *in eye coordinates*
	 pPlayerProgram->SetUniform("spotLight2.La", glm::vec3(m_colour2));	//1	// Ambient colour of light
	 pPlayerProgram->SetUniform("spotLight2.Ld", glm::vec3(m_colour2));	//1	// Diffuse colour of light
	 pPlayerProgram->SetUniform("spotLight2.Ls", glm::vec3(m_colour2));	//1	// Specular colour of light
	 pPlayerProgram->SetUniform("spotLight2.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
	 pPlayerProgram->SetUniform("spotLight2.exponent", 10.0f);// - strengths 1 =very , 1000000 not
	 pPlayerProgram->SetUniform("spotLight2.cutoff", 100.0f);//30 - angle




	 //render player
	 modelViewMatrixStack.Push();
		 modelViewMatrixStack.Translate(m_currentPlayerPos + glm::vec3(0, 1, 0));
		 modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.025f);
		 pPlayerProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		 pPlayerProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlayer->Render();
		
	 modelViewMatrixStack.Pop();



	//to hide back faces of other objects
	 glEnable(GL_CULL_FACE);

	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());

}
//zhan code



void Game::CheckDistance(float dist) {
	//if enemy distance is closer than 3 ->hit the player
	if (dist < 3) {
		hit();
	}
	//if enemy distance is closer than 50 -> set regular enmey scale
	else if (dist < 50) {
		m_enemyScale = 0.3f;
	}
	//else diminish enely size
	else {

		m_enemyScale = 0.3f - (dist - 50)*0.01f;
		if (m_enemyScale< 0) {
			m_enemyScale = 0;
		}
	}

}


void Game::hit() {

		m_gotHit = true;
		m_hitWait = 100;
		m_inputSpeed = 0;
		m_hitColour = -0.5f;
		m_playerScale = 1;
}


void Game::Cam1() {
	m_pCamera->Update(m_dt);
}

float drifting = 0.004f;
void Game::RailCam(int _type) {

	
	



	//original lerp
	if(!m_gotHit){
	m_currentDistance += ((float)(drifting) + m_inputSpeed)*(float)m_dt;
	m_inputSpeed -= ((m_inputSpeed - 0) / 400)*(float)m_dt;
	m_playerScale -= ((m_playerScale - 1.0f)/100)*(float)m_dt;//player scale
	
	m_hitColour -= ((m_hitColour- 0)/400)*(float)m_dt;


	}
	else {

		if (m_hitWait > 0) {
			m_hitWait -= 1 * (int)m_dt;
		}
		else {
			m_gotHit = false;
		}

	}
	//allow input
	if (m_inputSpeed<0.01)m_limitInput = false;

	glm::vec3 p;
	glm::vec3 pNext;
	m_pCatmullRom->Sample(m_currentDistance, p);
	m_pCatmullRom->Sample(m_currentDistance + 1.0f, pNext);

	//Frenet
	glm::vec3 t;
	t = glm::normalize(pNext - p);
	glm::vec3 n;
	n = glm::normalize(glm::cross(t, glm::vec3(0, 1, 0)));
	glm::vec3 b;
	b = glm::normalize(glm::cross(n, t));

	glm::vec3 pPos;//player pos
	m_pCatmullRom->Sample(m_currentDistance + 5.0f, pPos);
	m_currentPlayerPos = pPos;

	if (_type == 0) {
		//normal
		m_pCamera->Set(p + glm::vec3(0, 2, 0), p + 10.0f*t, glm::vec3(0, 1, 0));
	}
	if (_type == 1) {
		//side
		m_pCamera->Set(m_currentPlayerPos - n*10.0f + b*1.0f, m_currentPlayerPos, glm::vec3(0, 1, 0));
	}
	if (_type == 2) {
		//top down
		m_pCamera->Set(m_currentPlayerPos+b*10.0f, m_currentPlayerPos, glm::vec3(0, 0, 1));
	}
	//----------------------------a little up on y axes



}

void Game::FogToggle() {

	if (m_fogActive) {
		m_fogDist -= (m_fogDist - m_fogDistMin) / 500 * (float)m_dt;
	}
	else {
		m_fogDist -= (m_fogDist - m_fogDistMax) / 500 * (float)m_dt;
	}

}

void Game::DayNightSystem() {
	if (m_dayTime) {
		//turn on global
		m_globalLight -= ((m_globalLight - 1.0f) / 500)*(float)m_dt;
		//spotlight 1 off
		m_colour1 -= ((m_colour1 - glm::vec3(0.0f)) / glm::vec3(500))*(float)m_dt;
		m_colour2 -= ((m_colour2 - glm::vec3(0.0f)) / glm::vec3(500))*(float)m_dt;
		//fog ->day
		m_fogColour -=((m_fogColour-m_fogDayColour) / glm::vec3(500))*(float)m_dt;
	}
	else {
		//turn off global
		m_globalLight -= ((m_globalLight - 0.2f) / 500)*(float)m_dt;
		//spotlight 1 on
		m_colour1 -= ((m_colour1 - glm::vec3(0.0f, 0.8f, 0.8f)) / glm::vec3(500))*(float)m_dt;
		m_colour2 -= ((m_colour2 - glm::vec3(0.5f, 0.9, 0.5f)) / glm::vec3(500))*(float)m_dt;
		//fog ->night
		m_fogColour -= ((m_fogColour - m_fogNightColour) / glm::vec3(500))*(float)m_dt;
	}


	

}

// Update method runs repeatedly with the Render method
void Game::Update()
{
	//time
	m_time+=(float)(0.01f*m_dt);
	//cam toggle
	m_camViewType == 3 ? Cam1() : RailCam(m_camViewType);

	m_pAudio->Update();

	FogToggle();
	DayNightSystem();


}





void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/


	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();


}


WPARAM Game::Execute()
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();


	MSG msg;

	while(1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		}
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param)
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_camViewType = 0;
			break;
		case '2':
			m_camViewType = 2;
			break;
		case '3':
			m_camViewType = 1;
			break;
		case '4':
			m_camViewType = 3;
			break;
		case '5':
			m_dayTime = !m_dayTime;
			break;
		case '6':
			m_fogActive = !m_fogActive;
			break;
		case 0x45://E
			hit();
			break;
		case ' ':
			if (m_limitInput)break;
			m_inputSpeed = 0.05f;
			m_playerScale = 0.6f;
			m_limitInput = true;

			break;
		case '9':
			m_pAudio->PlayEventSound();

			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance()
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance)
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int)
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
