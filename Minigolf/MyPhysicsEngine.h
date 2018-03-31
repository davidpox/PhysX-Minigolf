#pragma once

#include "BasicActors.h"
#include "CourseActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	class CollisionCallback : public PxSimulationEventCallback {
	public:
		//an example variable that will be checked in the main simulation loop
		bool trigger;

		bool endGame = false;

		CollisionCallback() : trigger(false) {}

		///Method called when the contact with the trigger object is detected.
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) {
			//you can read the trigger information here
			for (PxU32 i = 0; i < count; i++) {
				//filter out contact with the planes
				if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE) {
					//check if eNOTIFY_TOUCH_FOUND trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
						cerr << "onTrigger::eNOTIFY_TOUCH_FOUND" << endl;
						endGame = true;
						trigger = true;
					}
					//check if eNOTIFY_TOUCH_LOST trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST) {
						cerr << "onTrigger::eNOTIFY_TOUCH_LOST" << endl;
						trigger = false;
					}
				}
			}
		}

		///Method called when the contact by the filter shader is detected.
		virtual void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {
			string actor0n = pairHeader.actors[0]->getName();
			string actor1n = pairHeader.actors[1]->getName();

			cerr << "Contact found between " << actor0n << " and " << actor1n << endl;
			

			//check all pairs
			for (PxU32 i = 0; i < nbPairs; i++) {
				//check eNOTIFY_TOUCH_FOUND
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
					cerr << "onContact::eNOTIFY_TOUCH_FOUND" << endl;

					if (actor0n == "playerball") {		
						((PxRigidDynamic*)pairHeader.actors[0])->setLinearVelocity(PxVec3(0.0f));
						pairHeader.actors[0]->setGlobalPose(PxTransform(PxVec3(0.0f, 10.0f, 0.0f)));
					}
				}
				//check eNOTIFY_TOUCH_LOST
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
					cerr << "onContact::eNOTIFY_TOUCH_LOST" << endl;
				}
			}
		}

		virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {}
		virtual void onWake(PxActor **actors, PxU32 count) {}
		virtual void onSleep(PxActor **actors, PxU32 count) {}
	};

	//A simple filter shader based on PxDefaultSimulationFilterShader - without group filtering
	static PxFilterFlags CustomFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {

		// let triggers through
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		//enable continous collision detection
		pairFlags |= PxPairFlag::eCCD_LINEAR;


		//customise collision filtering here
		//e.g.

		// trigger the contact callback for pairs (A,B) where 
		// the filtermask of A contains the ID of B and vice versa.
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)) {
			//trigger onContact callback for this pair of objects
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
			//pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}

		return PxFilterFlags();
	};


	///Custom scene class
	class MyScene : public Scene
	{
	public:

		struct FilterGroup {
			enum Enum {
				ePLAYERBALL			= (1 << 0),
				ePLANE				= (1 << 1)
			};

		};

		CollisionCallback* cCallback;
		bool hasGameEnded;

		MyScene() : Scene() {
			filter_shader = CustomFilterShader;
		};

		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 2.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 2.0f);

			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_HORIZONTAL, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_VERTICAL, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_BENDING, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_SHEARING, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{			
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);

			cCallback = new CollisionCallback();
			px_scene->setSimulationEventCallback(cCallback);

			CreateScene();
		}

		void CustomUpdate() {
			if (cCallback->endGame) {
				hasGameEnded = true;
			}
		}

		void CreateScene() {
			Plane* plane = new Plane();
			plane->Color(PxVec4(171.0f / 255.0f, 226.0f / 255.0f, 158.0f / 255.0f, 1.0f));
			plane->Name("plane");
			Add(plane);

			Sphere* playerBall = new Sphere(PxTransform(PxVec3(0.0f, 1.7f, 0.0f)), 0.3f, 1.0f);
			playerBall->Color(PxVec4(1.0f, 1.0f, 1.0f, 1.0f));
			PxMaterial* ballMat = CreateMaterial(0.4f, 0.2f, 0.8f);
			playerBall->GetShape(0)->setMaterials(&ballMat, 1);
			playerBall->Name("playerball");
			((PxRigidBody*)playerBall->Get())->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

			PxRigidDynamic* playerBallRD = ((PxRigidDynamic*)playerBall->Get());
			playerBallRD->setAngularDamping(2.0f);

			Add(playerBall);						

			/* COURSE STARTS HERE*/

			// TEE 
			PathStart* tee = new PathStart(PxTransform(PxVec3(0.0f, 1.0f, 0.0f)));
			Add(tee);

			// STRAIGHT PATH WITH SLIDING d6
			PathStraight* straight1 = new PathStraight(PxTransform(PxVec3(0.0f, 1.0f, -8.0f)));
			Add(straight1);

			Box* d6Box = new Box(PxTransform(3.4f, 1.9f, -12.0f), PxVec3(0.2f, 0.2f, 4.0f));	
			D6Joint* d6joint1 = new D6Joint(nullptr, PxTransform(3.4f, 1.6f, -12.0f), d6Box->Get(), PxTransform(0.0f, 0.0f, 0.0f));
			((PxD6Joint*)d6joint1->Get())->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);

			PxD6JointDrive drive = PxD6JointDrive(PX_MAX_REAL, 0.0f, 1.0f);

			((PxD6Joint*)d6joint1->Get())->setDrive(PxD6Drive::eX, drive);

			((PxD6Joint*)d6joint1->Get())->setDrivePosition(PxTransform(-3.4f, 1.6f, -12.0f));
			((PxD6Joint*)d6joint1->Get())->setDriveVelocity(PxVec3(1.0f, 0.0f, 0.0f), PxVec3(0.0f, 0.0f, 0.0f));
			Add(d6Box);

			// EMPTY STRAIGHT PATH

			PathStraight* straight2 = new PathStraight(PxTransform(PxVec3(0.0f, 1.0f, -16.0f)));
			Add(straight2);

			// SLANTED CORNER

			PathCorner* slantedCorner1 = new PathCorner(PxTransform(PxVec3(0.0f, 1.0f, -24.0f)), true);
			Add(slantedCorner1);

			// EMPTY STRAIGHT ROT 90

			PathStraight* straight3 = new PathStraight(PxTransform(PxVec3(8.0f, 1.0f, -24.0f), PxQuat(1.5708f, PxVec3(0, 1, 0))));
			Add(straight3);

			// STRAIGHT ROT 90 w/ FLAT WINDMILL

			PathStraight* straight4 = new PathStraight(PxTransform(PxVec3(16.0f, 1.0f, -24.0f), PxQuat(1.5708f, PxVec3(0, 1, 0))));
			Add(straight4);

			WindMill* wmPS = new WindMill(PxTransform(PxVec3(16.0f, 1.6f, -24.0f), PxQuat(1.5708f, PxVec3(1, 0, 0))));
			wmPS->Get()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			Add(wmPS);
			RevoluteJoint WindmillFlatJoint = RevoluteJoint(nullptr, PxTransform(PxVec3(16.0f, 1.6f, -24.0f), PxQuat(PxPi / 2, PxVec3(0, 0, 1))), wmPS, PxTransform(PxTransform(0.0f, 0.0f, 0.0f)));
			WindmillFlatJoint.DriveVelocity(1.0f);

			// EMPTY STRAIGHT ROT 90

			PathStraight* straight5 = new PathStraight(PxTransform(PxVec3(24.0f, 1.0f, -24.0f), PxQuat(1.5708f, PxVec3(0, 1, 0))));
			Add(straight5);

			// CORNER ROT -90 w/ PRISMATIC SLIDER
			PathCorner* corner1 = new PathCorner(PxTransform(PxVec3(32.0f, 1.0f, -24.0f), PxQuat(-1.5708f, PxVec3(0, 1, 0))));
			Add(corner1);

			Box* prismaticSlider = new Box(PxTransform(28.0f, 1.65f, -24.0f), PxVec3(0.2f, 0.2f, 3.55f));
			PrismaticJoint* prismaticSliderJoint = new PrismaticJoint(nullptr, PxTransform(28.0f, 1.65f, -24.0f), prismaticSlider->Get(), PxTransform(0.0f, 0.0f, 0.0f));
			Add(prismaticSlider);

			// EMPTY STRAIGHT 

			PathStraight* straight6 = new PathStraight(PxTransform(PxVec3(32.0f, 1.0f, -16.0f)));
			Add(straight6);

			// STRAIGHT WITH D6JOINT DOORS

			PathStraight* straight7 = new PathStraight(PxTransform(PxVec3(32.0f, 1.0f, -8.0f)));
			Add(straight7);

			Box* doorright = new Box(PxTransform(33.8f, 3.2f, -8.0f), PxVec3(1.60f, 1.8f, 0.2f), 0.01f);	
			Box* doorleft = new Box(PxTransform(30.2f, 3.2f, -8.0f), PxVec3(1.60f, 1.8f, 0.2f), 0.01f);

			FixedJoint* doorFixedJoint = new FixedJoint(doorleft->Get(), PxTransform(0.0f, 0.0f, 0.0f), doorright->Get(), PxTransform(0.0f, 0.0f, 0.0f));
			((PxFixedJoint*)doorFixedJoint->Get())->setBreakForce(500.0f, 500.0f);

			D6Joint* doorrightd6 = new D6Joint(nullptr, PxTransform(35.5f, 3.2f, -8.0f), doorright->Get(), PxTransform(1.8f, 0.0f, 0.0f));
			((PxD6Joint*)doorrightd6->Get())->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);

			D6Joint* doorleftd6 = new D6Joint(nullptr, PxTransform(28.5f, 3.2f, -8.0f), doorleft->Get(), PxTransform(-1.8f, 0.0f, 0.0f));
			((PxD6Joint*)doorleftd6->Get())->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);

			Add(doorright);
			Add(doorleft);

			// EMPTY STRAIGHT

			PathStraight* straight8 = new PathStraight(PxTransform(PxVec3(32.0f, 1.0f, 0.0f)));
			Add(straight8);

			// STRAIGHT w/ TALL WINDMILL

			PathWindMill* windmillPath = new PathWindMill(PxTransform(PxVec3(32.0f, 1.0f, 8.0f)));
			Add(windmillPath);

			WindMill* wm = new WindMill(PxTransform(32.0f, 4.9f, 7.9f));
			wm->Get()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			Add(wm);
			RevoluteJoint WindmillJoint = RevoluteJoint(nullptr, PxTransform(PxVec3(32.0f, 4.9f, 7.35f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))), wm, PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
			WindmillJoint.DriveVelocity(1.0f);

			// SHARP CORNER

			PathCorner* corner2 = new PathCorner(PxTransform(PxVec3(32.0f, 1.0f, 16.0f), PxQuat(1.5708f, PxVec3(0, 1, 0))));
			Add(corner2);

			// STRAIGHT w/ DANGLING SPHERICAL JOINTS

			PathStraight* straight9 = new PathStraight(PxTransform(PxVec3(40.0f, 1.0f, 16.0f), PxQuat(1.5708f, PxVec3(0, 1, 0))));
			Add(straight9);

			Box* dangle1 = new Box(PxTransform(40.0f, 3.4f, 16.0f), PxVec3(0.2f, 2.0f, 0.2f));
			Box* dangle2 = new Box(PxTransform(40.0f, 3.4f, 17.0f), PxVec3(0.2f, 2.0f, 0.2f));
			Box* dangle3 = new Box(PxTransform(40.0f, 3.4f, 15.0f), PxVec3(0.2f, 2.0f, 0.2f));
			Box* dangle4 = new Box(PxTransform(40.0f, 3.4f, 14.0f), PxVec3(0.2f, 2.0f, 0.2f));
			Box* dangle5 = new Box(PxTransform(40.0f, 3.4f, 18.0f), PxVec3(0.2f, 2.0f, 0.2f));

			SphericalJoint(nullptr, PxTransform(40.0f, 5.5f, 16.0f), dangle1->Get(), PxTransform(0.0f, 2.0f, 0.0f));
			SphericalJoint(nullptr, PxTransform(40.0f, 5.5f, 17.0f), dangle2->Get(), PxTransform(0.0f, 2.0f, 0.0f));
			SphericalJoint(nullptr, PxTransform(40.0f, 5.5f, 15.0f), dangle3->Get(), PxTransform(0.0f, 2.0f, 0.0f));
			SphericalJoint(nullptr, PxTransform(40.0f, 5.5f, 14.0f), dangle4->Get(), PxTransform(0.0f, 2.0f, 0.0f));
			SphericalJoint(nullptr, PxTransform(40.0f, 5.5f, 18.0f), dangle5->Get(), PxTransform(0.0f, 2.0f, 0.0f));

			Add(dangle1);
			Add(dangle2);
			Add(dangle3);
			Add(dangle4);
			Add(dangle5);

			// EMPTY STRAIGHT 

			PathStraight* straight10 = new PathStraight(PxTransform(PxVec3(48.0f, 1.0f, 16.0f), PxQuat(1.5708f, PxVec3(0, 1, 0))));
			Add(straight10);

			// STRAIGHT w/ BOUNCY MIDDLE

			Trampoline* tramp1 = new Trampoline(PxTransform(56.0f, 0.0f, 16.0f), PxVec3(3.90f, 2.0f, 4.0f), 4.0f, 0.01f, 0.1f);
			tramp1->AddToScene(this);
			
			// EMPTY STRAIGHT

			PathStraight* straight11 = new PathStraight(PxTransform(PxVec3(64.0f, 1.0f, 16.0f), PxQuat(1.5708f, PxVec3(0, 1, 0))));
			Add(straight11);

			// SPLIT WEDGE

			PathSplitWedge* splitWedgePath = new PathSplitWedge(PxTransform(PxVec3(72.0f, 1.0f, 16.0f), PxQuat(-1.5708f, PxVec3(0, 1, 0))));
			Add(splitWedgePath);

			// EMPTY STRAIGHT GOING UP

			PathStraight* straight12 = new PathStraight(PxTransform(PxVec3(72.0f, 1.0f, 8.0f)));
			Add(straight12);

			// EMPTY STRAIGHT GOING DOWN

			PathStraight* straight13 = new PathStraight(PxTransform(PxVec3(72.0f, 1.0f, 24.0f)));
			Add(straight13);

			// TUNNEL GOING UP

			PathTunnel* pathTunnelUp = new PathTunnel(PxTransform(72.0f, 1.0f, 0.0f));
			Add(pathTunnelUp);

			// TUNNEL GOING DOWN

			PathTunnel* pathTunnelDown = new PathTunnel(PxTransform(72.0f, 1.0f, 32.0f));
			Add(pathTunnelDown);

			// ICE UP 1

			PathStraight* straight14 = new PathStraight(PxTransform(PxVec3(72.0f, 1.0f, -8.0f)), S_ICE);
			Add(straight14);

			// ICE UP 2

			PathStraight* straight15 = new PathStraight(PxTransform(PxVec3(72.0f, 1.0f, -16.0f)), S_ICE);
			Add(straight15);

			// ICE UP 3

			PathStraight* straight16 = new PathStraight(PxTransform(PxVec3(72.0f, 1.0f, -24.0f)), S_ICE);
			Add(straight16);

			// SAND DOWN 1

			PathStraight* straight17 = new PathStraight(PxTransform(PxVec3(72.0f, 1.0f, 40.0f)), S_SAND);
			Add(straight17);

			// SAND DOWN 2

			PathStraight* straight18 = new PathStraight(PxTransform(PxVec3(72.0f, 1.0f, 48.0f)), S_SAND);
			Add(straight18);

			// SAND DOWN 3

			PathStraight* straight19 = new PathStraight(PxTransform(PxVec3(72.0f, 1.0f, 56.0f)), S_SAND);
			Add(straight19);

			Box* sandStopper = new Box(PxTransform(72.0f, 1.0f, 59.8f), PxVec3(3.6f, 1.0f, 0.2f));
			sandStopper->SetKinematic(true);
			sandStopper->Color(PxVec4(156.f / 255.f, 84.f / 255.f, 16.f / 255.f, 1.0f));
			Add(sandStopper);

			// SAND TOP TRAP

			PathStraight* straight20 = new PathStraight(PxTransform(PxVec3(72.0f, 1.0f, -32.0f)), S_SAND);
			Add(straight20);

			// INCLINE PATH

			PathSharpIncline* sharpInclinePath = new PathSharpIncline(PxTransform(PxVec3(72.0f, 1.0f, -40.0f)));
			Add(sharpInclinePath);

			// FINISH HOLE

			PathHole* hole = new PathHole(PxTransform(PxVec3(72.0f, 1.0f, -48.0f)));
			Add(hole);

			// FINISH FLAG 

			Capsule* pole = new Capsule(PxTransform(PxVec3(72.0f, 4.0f, -48.0f), PxQuat(1.5708f, PxVec3(0, 0, 1))), PxVec2(0.05f, 4.0f));
			pole->SetKinematic(true);
			pole->Color(PxVec4(0.9f, 0.0f, 0.0f, 1.0f));
			Add(pole);

			Cloth* flag = new Cloth(PxTransform(PxVec3(72.0f, 6.5f, -48.f), PxQuat(1.5708f, PxVec3(0, 0, 1))), PxVec2(1.5f, 1.5f), 10, 10);
			flag->Color(PxVec4(1, 1, 1, 1));
			((PxCloth*)flag->Get())->setExternalAcceleration(PxVec3(-10.0f, 3.0f, 5.0f));
			Add(flag);

			// FINISH TRIGGER

			Box* holeTrigger = new Box(PxTransform(PxVec3(72.0f, 0.5f, -48.0f)), PxVec3(0.35f, 0.45f, 0.35f));
			holeTrigger->SetKinematic(true);
			holeTrigger->SetTrigger(true, 0);
			holeTrigger->Color(PxVec4(1.0f, 1.0f, 1.0f, 0.0f));
			Add(holeTrigger);

			// TWO SPHERES ADJOINED BY FIXED JOINT

			Sphere* sp1 = new Sphere(PxTransform(68.0f, 1.9f, 15.0f), 0.5f, 0.1f);
			Sphere* sp2 = new Sphere(PxTransform(68.0f, 1.9f, 17.0f), 0.5f, 0.1f);
			Add(sp1);
			Add(sp2);

			FixedJoint* fj = new FixedJoint(sp1->Get(), PxTransform(68.0f, 1.9f, 15.0f), sp2->Get(), PxTransform(68.0f, 1.9f, 17.0f));
			((PxFixedJoint*)fj->Get())->setBreakForce(500.0f, 500.0f);

			// COLLISION MECHANICS

			playerBall->SetupFiltering(FilterGroup::ePLAYERBALL, FilterGroup::ePLANE);
			plane->SetupFiltering(FilterGroup::ePLANE, FilterGroup::ePLAYERBALL);

			pole->GetShape(0)->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		}
	};
}
