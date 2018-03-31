#include "VisualDebugger.h"
#include <vector>
#include "Camera.h"
#include "Extras\Renderer.h"
#include "Extras\HUD.h"



namespace VisualDebugger
{
	using namespace physx;

	enum RenderMode
	{
		DEBUG,
		NORMAL,
		BOTH
	};

	enum HUDState
	{
		EMPTY = 0,
		HELP = 1,
		PAUSE = 2,
		SCORE = 3
	};

	

	//function declarations
	void KeyHold();
	void KeySpecial(int key, int x, int y);
	void KeyRelease(unsigned char key, int x, int y);
	void KeyPress(unsigned char key, int x, int y);

	void motionCallback(int x, int y);
	void mouseCallback(int button, int state, int x, int y);
	void exitCallback(void);

	void RenderScene();
	void ToggleRenderMode();
	void HUDInit();
	void UpdateCamera();

	///simulation objects
	Camera* camera;
	PhysicsEngine::MyScene* scene;
	PxReal delta_time = 1.f/60.f;			// 1/60 if at uni, 1/150 if at home
	PxReal gForceStrength = 3;
	RenderMode render_mode = NORMAL;
	const int MAX_KEYS = 256;
	bool key_state[MAX_KEYS];
	bool hud_show = true;
	HUD hud;
	float shotstrength = 0.0f;
	bool freecam = false;

	int shotsTaken = 0;
	bool clearToShoot = true;
	bool ended = false;



	//Init the debugger
	void Init(const char *window_name, int width, int height)
	{
		///Init PhysX
		PhysicsEngine::PxInit();
		scene = new PhysicsEngine::MyScene();
		scene->Init();

		///Init renderer
		Renderer::BackgroundColor(PxVec3(178.0f / 255.f, 232.f / 255.f, 255.f / 255.f));
		Renderer::SetRenderDetail(20);
		Renderer::InitWindow(window_name, width, height);
		Renderer::Init();

		camera = new Camera(PxVec3(10.0f, 5.0f, 15.0f), scene->GetSelectedActor()->getGlobalPose().p, 20.0f, scene->GetSelectedActor());
		
		//initialise HUD
		HUDInit();

		///Assign callbacks
		//render
		glutDisplayFunc(RenderScene);

		//keyboard
		glutKeyboardFunc(KeyPress);
		glutSpecialFunc(KeySpecial);
		glutKeyboardUpFunc(KeyRelease);

		//mouse
		glutMouseFunc(mouseCallback);
		glutMotionFunc(motionCallback);

		//exit
		atexit(exitCallback);

		//init motion callback
		motionCallback(0,0);
		
	}

	void HUDInit()
	{
		//initialise HUD
		//add an empty screen
		hud.AddLine(EMPTY, "");
		//add a help screen
		hud.AddLine(HELP, " Simulation");
		hud.AddLine(HELP, "    F10 - pause");
		hud.AddLine(HELP, " Gameplay");
		hud.AddLine(HELP, " Right click to move camera");
		hud.AddLine(HELP, " Hold space to increase strength");
		hud.AddLine(HELP, "     Release to shoot");
		hud.AddLine(HELP, "");
		hud.AddLine(HELP, " Display");
		hud.AddLine(HELP, "    F5 - Help on/off");
		hud.AddLine(HELP, "    F6 - Shadows on/off");
		hud.AddLine(HELP, "    F7 - Render mode");
		hud.AddLine(HELP, "    F8 - Free cam on/off");
		hud.AddLine(HELP, "");
		//add a pause screen
		hud.AddLine(PAUSE, "");
		hud.AddLine(PAUSE, "");
		hud.AddLine(PAUSE, "");
		hud.AddLine(PAUSE, "   Simulation paused. Press F10 to continue.");
		//set font size for all screens
		hud.FontSize(0.018f);
		//set font color for all screens
		hud.Color(PxVec3(0.f,0.f,0.f));
	}

	//Start the main loop
	void Start()
	{ 
		glutMainLoop(); 
	}

	//Render the scene and perform a single simulation step
	void RenderScene()
	{
		//handle pressed keys
		KeyHold();

		//start rendering
		Renderer::Start(camera->getEye(), camera->getDir());

		if ((render_mode == DEBUG) || (render_mode == BOTH))
		{
			Renderer::Render(scene->Get()->getRenderBuffer());
		}

		if ((render_mode == NORMAL) || (render_mode == BOTH))
		{
			std::vector<PxActor*> actors = scene->GetAllActors();
			if (actors.size())
				Renderer::Render(&actors[0], (PxU32)actors.size());
		}

		//adjust the HUD state
		if (hud_show)
		{
			if (scene->Pause())
				hud.ActiveScreen(PAUSE);
			else
				hud.ActiveScreen(HELP);
		}
		else
			hud.ActiveScreen(EMPTY);

		//render HUD
		hud.Render();

		//finish rendering
		Renderer::Finish();

		//perform a single simulation step
		scene->Update(delta_time);

		if (!freecam) 
			camera->UpdatePosition(delta_time);
			
				
		
	}

	//user defined keyboard handlers
	void UserKeyPress(int key)
	{
		switch (toupper(key))
		{
		//implement your own
		case 'R':
			break;
		default:
			break;
		}
	}

	void UserKeyRelease(int key)
	{
		PxVec3 dir = camera->getDir();
		switch (toupper(key))
		{
		//implement your own
		case 'R':
		{
			hud.changeLine(HELP, "TEST", 0);	
			PxVec3 pos = scene->GetSelectedActor()->getGlobalPose().p;
			std::cout << pos.y << std::endl;
		}
			break;
		case ' ':
		{
			scene->GetSelectedActor()->addForce(PxVec3(dir.x, 0.0f, dir.z).getNormalized()*gForceStrength * shotstrength);
			std::cout << "Shot with: " << shotstrength << std::endl;
			shotstrength = 0.0f;
		}
		default:
			break;
		}
	}

	void UserKeyHold(int key)
	{
		switch (toupper(key)) {
			case 'W':
			{
				if (!freecam) break;
				camera->MoveForward(delta_time);
				break;
			}
			case 'S':
			{
				if (!freecam) break;
				camera->MoveBackward(delta_time);
				break;
			}
			case 'A':
			{
				if (!freecam) break;
				camera->MoveLeft(delta_time);
				break;
			}
			case 'D':
			{
				if (!freecam) break;
				camera->MoveRight(delta_time);
				break;
			}
			default:
				break;
		}
	}

	//handle force control keys
	void ForceInput(int key)
	{
		if (!scene->GetSelectedActor())
			return;
		switch (toupper(key))
		{
		case ' ': 
			shotstrength += 0.4f;
			break;
		default:
			break;
		}
	}

	///handle special keys
	void KeySpecial(int key, int x, int y)
	{
		//simulation control
		switch (key)
		{
			//display control
		case GLUT_KEY_F5:
			//hud on/off
			hud_show = !hud_show;
			break;
		case GLUT_KEY_F6:
			//shadows on/off
			Renderer::ShowShadows(!Renderer::ShowShadows());
			break;
		case GLUT_KEY_F7:
			//toggle render mode
			ToggleRenderMode();
			break;
			//simulation control
		case GLUT_KEY_F10:
			//toggle scene pause
			scene->Pause(!scene->Pause());
			break;
		case GLUT_KEY_F8:
			freecam = !freecam;
			break;
		case GLUT_KEY_F9:
			delta_time = 1.0f/1500.0f;
			break;
		case GLUT_KEY_F11:
			delta_time = 1.0/150.0f;
			break;
		default:
			break;
		}
	}

	//handle single key presses
	void KeyPress(unsigned char key, int x, int y)
	{
		//do it only once
		if (key_state[key] == true)
			return;

		key_state[key] = true;

		//exit
		if (key == 27)
			exit(0);

		UserKeyPress(key);
	}

	//handle key release
	void KeyRelease(unsigned char key, int x, int y)
	{
		key_state[key] = false;
		UserKeyRelease(key);
	}

	//handle holded keys
	void KeyHold()
	{
		for (int i = 0; i < MAX_KEYS; i++)
		{
			if (key_state[i]) // if key down
			{
				ForceInput(i);
				UserKeyHold(i);
			}
		}
	}

	///mouse handling
	int mMouseX = 0;
	int mMouseY = 0;

	void motionCallback(int x, int y) {
		int dx = mMouseX - x;
		int dy = mMouseY - y;

		if (!freecam) {
			camera->UpdateCamera(dx, dy);
		} 
		else {
			camera->Motion(dx, dy, delta_time);
		}

		mMouseX = x;
		mMouseY = y;
	}

	void mouseCallback(int button, int state, int x, int y)
	{
		//std::cout << "pressed " << button << "\t state: " << state << std::endl;

		//if (button == 2 && state == 0) {
		//	glutSetCursor(GLUT_CURSOR_NONE); 
		//} else { glutSetCursor(GLUT_CURSOR_INHERIT); }
		
		mMouseX = x;
		mMouseY = y;
	}

	void ToggleRenderMode()
	{
		if (render_mode == NORMAL)
			render_mode = DEBUG;
		else if (render_mode == DEBUG)
			render_mode = BOTH;
		else if (render_mode == BOTH)
			render_mode = NORMAL;
	}

	///exit callback
	void exitCallback(void)
	{
		delete camera;
		delete scene;
		PhysicsEngine::PxRelease();
	}
}

