#pragma once

#include "BasicActors.h"
#include "CourseActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		Box* box;
		Box* box2;
		Sphere* playerBall;

	public:
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 2.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 2.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{			
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);

			CreateScene();
		}

		//Custom udpate function
		virtual void CustomUpdate() {

		}

		void CreateScene() {
			plane = new Plane();
			plane->Color(PxVec3(210.0f / 255.0f, 210.0f / 255.0f, 210.0f / 255.0f));
			Add(plane);



			playerBall = new Sphere(PxTransform(PxVec3(10.0f, 10.0f, 0.0f)), 0.3f, 1.0f);
			playerBall->Color(PxVec3(1.0f, 1.0f, 1.0f));
			playerBall->Name("player");
			//playerBall->SetKinematic(true);
			PxMaterial* ballMat = CreateMaterial(0.4f, 0.2f, 0.8f);
			playerBall->GetShape(0)->setMaterials(&ballMat, 1);

			PxRigidDynamic* playerBallRD = ((PxRigidDynamic*)playerBall->Get());
			playerBallRD->setAngularDamping(5.0f);

			Add(playerBall);

			//box = new Box(PxTransform(PxVec3(10.0f, 2.0f, 0.0f)));
			//box->Color(PxVec3(1.0f, 1.0f, 1.0f));
			//box->SetKinematic(true);
			//Add(box);

			PathStraight* ps = new PathStraight(PxTransform(PxVec3(0.0f, 1.0f, -8.0f)), S_NORMAL);
			Add(ps);

			PathStraight* ps2 = new PathStraight(PxTransform(PxVec3(0.0f, 1.0f, -16.0f)), S_SAND);
			Add(ps2);

			PathStraight* ps3 = new PathStraight(PxTransform(PxVec3(0.0f, 1.0f, -24.0f)), S_ICE);
			Add(ps3);

			PathHole* pH = new PathHole(PxTransform(PxVec3(0.0f, 1.0f, -32.0f)));
			Add(pH);

			PathStart* pst = new PathStart(PxTransform(PxVec3(0.0f, 1.0f, 0.0f)));
			Add(pst);

			PathCorner* crn = new PathCorner(PxTransform(PxVec3(10.0f, 1.0f, 0.0f)));
			Add(crn);

			PathSharpIncline* PSI = new PathSharpIncline(PxTransform(PxVec3(10.0f, 1.0f, -8.0f)));
			Add(PSI);

			PathWindMill* PWM = new PathWindMill(PxTransform(PxVec3(10.0f, 1.0f, -16.0f)));
			Add(PWM);

			WindMill* wm = new WindMill(PxTransform(10.0f, 4.9f, -15.1f));
			wm->Get()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			Add(wm);		
			RevoluteJoint WindmillJoint = RevoluteJoint(nullptr, PxTransform(PxVec3(10.0f, 4.9f, -15.1f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))), wm, PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
			WindmillJoint.DriveVelocity(1.0f);

			PathStraight* PSWM = new PathStraight(PxTransform(PxVec3(10.0f, 1.0f, -24.0f)));
			Add(PSWM);

			WindMill* wmPS = new WindMill(PxTransform(PxVec3(10.0f, 2.0f, -24.0f), PxQuat(1.5708f, PxVec3(1, 0, 0))));
			wmPS->Get()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			Add(wmPS);
			RevoluteJoint WindmillFlatJoint = RevoluteJoint(nullptr, PxTransform(PxVec3(10.0f, 1.6f, -24.0f), PxQuat(PxPi / 2, PxVec3(0, 0, 1))), wmPS, PxTransform(PxTransform(0.0f, 0.0f, 0.0f)));
			WindmillFlatJoint.DriveVelocity(1.0f);

			PathTunnel* PT = new PathTunnel(PxTransform(10.0f, 1.0f, -32.0f));
			Add(PT);
			
		}
	};
}
