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
		PAUSE = 2
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
	PxReal delta_time = 1.f/120.f;
	PxReal gForceStrength = 3;
	RenderMode render_mode = NORMAL;
	const int MAX_KEYS = 256;
	bool key_state[MAX_KEYS];
	bool hud_show = true;
	HUD hud;
	int mMouseX = 0;
	int mMouseY = 0;
	int dx = 0;
	int dy = 0;

	//Init the debugger
	void Init(const char *window_name, int width, int height)
	{
		///Init PhysX
		PhysicsEngine::PxInit();
		scene = new PhysicsEngine::MyScene();
		scene->Init();

		///Init renderer
		Renderer::BackgroundColor(PxVec3(150.0f / 255.0f, 150.0f / 255.0f, 150.0f / 255.0f));
		Renderer::SetRenderDetail(40);
		Renderer::InitWindow(window_name, width, height);
		Renderer::Init();

		camera = new Camera(PxVec3(0.0f, 5.0f, 15.0f), PxVec3(0.0f, -0.1f, -1.0f), 5.0f, scene->GetSelectedActor());
		
		//initialise HUD
		HUDInit();

		//PxVec3 ballPos = scene->GetSelectedActor()->getGlobalPose().p;
		//PxVec3 camPos = camera->getEye();

		//gluLookAt(camPos.x, camPos.y, camPos.z, ballPos.x, ballPos.y, ballPos.z, 0, 1, 0);

		///Assign callbacks
		//render
		glutDisplayFunc(RenderScene);

		//keyboard
		glutKeyboardFunc(KeyPress);
		glutSpecialFunc(KeySpecial);
		glutKeyboardUpFunc(KeyRelease);

		//mouse
		glutMouseFunc(mouseCallback);
		glutPassiveMotionFunc(motionCallback);

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
		hud.AddLine(HELP, "    F9 - select next actor");
		hud.AddLine(HELP, "    F10 - pause");
		hud.AddLine(HELP, "    F12 - reset");
		hud.AddLine(HELP, "");
		hud.AddLine(HELP, " Display");
		hud.AddLine(HELP, "    F5 - help on/off");
		hud.AddLine(HELP, "    F6 - shadows on/off");
		hud.AddLine(HELP, "    F7 - render mode");
		hud.AddLine(HELP, "");
		hud.AddLine(HELP, " Camera");
		hud.AddLine(HELP, "    W,S,A,D,Q,Z - forward,backward,left,right,up,down");
		hud.AddLine(HELP, "    mouse + click - change orientation");
		hud.AddLine(HELP, "    F8 - reset view");
		hud.AddLine(HELP, "");
		hud.AddLine(HELP, " Force (applied to the selected actor)");
		hud.AddLine(HELP, "    I,K,J,L,U,M - forward,backward,left,right,up,down");
		hud.AddLine(HELP, "");
		hud.AddLine(HELP, " Made by David Puetter");
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
		Renderer::Start(camera->getEye(), camera->getDir()); // camera->getDir();

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
		UpdateCamera();
		
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
		switch (toupper(key))
		{
		//implement your own
		case 'R':
		{
			std::cout << "Rotating!" << std::endl;
			PxTransform pose = scene->GetSelectedActor()->getGlobalPose();
			pose.q *= PxQuat(0.174533f, PxVec3(0.0f, 1.0f, 0.0f));
			scene->GetSelectedActor()->setGlobalPose(pose);

			std::cout << "Calculated rotation:" << std::endl;
			PxReal rads;
			PxVec3 axis;
			pose.q.toRadiansAndUnitAxis(rads, axis);
			std::cout << "\tRads: " << rads << std::endl;
			std::cout << "\tx:" << axis.x << " | y: " << axis.y << " | z: " << axis.z << std::endl;
			
		}
			break;
		default:
			break;
		}
	}

	void UserKeyHold(int key)
	{
		switch (toupper(key)) {
			case 'A':
				{
				PxTransform pose = scene->GetSelectedActor()->getGlobalPose();
				pose.q *= PxQuat(-0.0174533f, PxVec3(0.0f, 1.0f, 0.0f));
				scene->GetSelectedActor()->setGlobalPose(pose);
				}
				break;
			case 'D':
				{
				PxTransform pose = scene->GetSelectedActor()->getGlobalPose();
				pose.q *= PxQuat(0.0174533f, PxVec3(0.0f, 1.0f, 0.0f));
				scene->GetSelectedActor()->setGlobalPose(pose);
				}
				break;
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
			// Force controls on the selected actor
		case 'I': //forward
			scene->GetSelectedActor()->addForce(PxVec3(0,0,-1)*gForceStrength);
			break;
		case 'K': //backward
			scene->GetSelectedActor()->addForce(PxVec3(0,0,1)*gForceStrength);
			break;
		case 'J': //left
			scene->GetSelectedActor()->addForce(PxVec3(-1,0,0)*gForceStrength);
			break;
		case 'L': //right
			scene->GetSelectedActor()->addForce(PxVec3(1,0,0)*gForceStrength);
			break;
		case 'U': //up
			scene->GetSelectedActor()->addForce(PxVec3(0,1,0)*gForceStrength);
			break;
		case 'M': //down
			scene->GetSelectedActor()->addForce(PxVec3(0,-1,0)*gForceStrength);
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
		case GLUT_KEY_F8:
			//reset camera view
			camera->Reset();
			break;

			//simulation control
		case GLUT_KEY_F10:
			//toggle scene pause
			scene->Pause(!scene->Pause());
			break;
		case GLUT_KEY_F12:
			//resect scene
			scene->Reset();
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

	void motionCallback(int x, int y)
	{
		dx = mMouseX - x;
		dy = mMouseY - y;

		//camera->Motion(dx, dy, delta_time);
		//camera->UpdateCamera(dx, dy);

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

	void UpdateCamera() {
		//PxTransform pose = scene->GetSelectedActor()->getGlobalPose();
		//PxReal rads;
		//PxVec3 axis;
		//PxVec3 CurrOrientation = camera->getDir();
		//pose.q.toRadiansAndUnitAxis(rads, axis);

		camera->SetPosition(scene->GetSelectedActor()->getGlobalPose().p + PxVec3(0, 5, 10));
		//camera->SetOrientation(PxVec3(axis.x, axis.y, axis.z));
	}
}

