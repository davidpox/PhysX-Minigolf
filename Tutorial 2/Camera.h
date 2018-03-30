#pragma once


#include "foundation/PxTransform.h"
#include "foundation/PxMat33.h"
#include "PhysicsEngine.h"
#include <iostream>

namespace VisualDebugger
{
	using namespace physx;

	///Camera class
	class Camera
	{
	private:
		PxVec3	eye, eye_init;
		PxVec3	dir, dir_init;
		PxReal speed, speed_init;
		PxReal x = 0.0f;
		PxReal y = 0.0f;

		PxRigidDynamic* targetActor;


		float mouseSensitivity = 0.2f;
		float distance = 10.0f;

		float theta;
		float azimuth;

		float camx;
		float camy;
		float camz;

	public:
		///constructor
		Camera(const PxVec3& _eye, const PxVec3& _dir, const PxReal _speed, PxRigidDynamic* targetActor);

		

		///reset view
		void Reset();

		///handle camera motion
		void Motion(int dx, int dy, PxReal delta_time);

		///handle analog move
		void AnalogMove(float x, float y);

		///get camera position
		PxVec3 getEye() const;

		///get camera direction
		PxVec3 getDir() const;

		///get camera transformation
		PxTransform	getTransform() const;


		///Set Position of camera
		void SetPosition(PxVec3 pos);

		void SetOrientation(PxVec3 rot);

		///move camera forward
		void MoveForward(PxReal delta_time);

		///move camera backward
		void MoveBackward(PxReal delta_time);

		///move camera left
		void MoveLeft(PxReal delta_time);

		///move camera right
		void MoveRight(PxReal delta_time);

		///move camera up
		void MoveUp(PxReal delta_time);

		///move camera down
		void MoveDown(PxReal delta_time);

		void UpdateCamera(float mouseX, float mouseY);

		void UpdatePosition(float deltatime);

		PxVec3 Lerp(PxVec3 start, PxVec3 end, float percent);

	};
}