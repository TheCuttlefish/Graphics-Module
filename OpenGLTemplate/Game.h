#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp,
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to
// delete the object in the destructor.
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CCube;
class CTetrahedron;
class CIcosahedron;



class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	//--zhan import
	COpenAssetImportMesh *m_pJellyFish;
	COpenAssetImportMesh *m_pSeaweed;
	COpenAssetImportMesh *m_pRocks;
	COpenAssetImportMesh *m_pCave;
	COpenAssetImportMesh *m_pUrchin;
	COpenAssetImportMesh *m_pPlayer;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom *m_pCatmullRom;
	//--adding cube pointer
	CCube *m_pCube;
	CTetrahedron *m_pTetrahedron;
	CIcosahedron *m_pIcosahedron;
	CPlane *m_grid;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;
	//my time
	float m_time;
	float m_currentDistance;
	int m_camViewType;
	float m_inputSpeed;
	bool m_limitInput;
	glm::vec3 m_currentPlayerPos;
	float m_playerScale;
	int m_enemyCount = 30;
	bool m_gotHit = false;//check if player got hit
	int m_hitWait=0;//timer for waiting when player gets hit
	float m_hitColour = 0;//colour of player's damage;
	float m_enemyScale = 0.3f;//scale of the enemies
	float m_globalLight = 1.0f;
	bool m_dayTime = true;//daytime - night time
	bool m_fogActive = true;//activisation of fog


	//0.23f .7f 0.89f - day
	//night - .02f, .12f, .14f
	//0.1f, 0.3f, 0.3f - original
	glm::vec3 m_fogColour = glm::vec3(.23f, .7f, .89f);
	glm::vec3 m_fogDayColour = glm::vec3(.23f, .7f, .89f);
	glm::vec3 m_fogNightColour = glm::vec3(.02f, .12f, .14f);
	float m_fogDist = 200.0f;
	float m_fogDistMin = 200.0f;
	float m_fogDistMax = 2000.0f;
	
	glm::vec3 m_colour1 = glm::vec3(0.0f, 0.8f, 0.8f);//dark turquoise
	glm::vec3 m_colour2 = glm::vec3(0.5f, 0.9, 0.5f);//spring green
	//float m_globalAmbience = 1.0f;
	vector<glm::vec3> m_enemyPos{ m_enemyCount };//pos of enemies
public:
	Game();
	~Game();
	//z
	void Cam1();
	void RailCam(int type);
	void hit();
	void CheckDistance(float dist);
	void DayNightSystem();
	void FogToggle();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;


};
