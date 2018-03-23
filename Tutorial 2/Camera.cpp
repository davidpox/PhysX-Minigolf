#include "Camera.h"

namespace VisualDebugger
{
	using namespace physx;

	Camera::Camera(const PxVec3 &_eye, const PxVec3& _dir, PxReal _speed, PxRigidDynamic* targetTransform)
	{
		eye_init = _eye;
		dir_init = _dir.getNormalized();
		speed_init = _speed;
		this->target = targetTransform;

		Reset();
	}

	void Camera::Reset()
	{
		x = eye.y;
		y = eye.x;

		eye = eye_init;
		dir = dir_init;
		speed = speed_init;
	}

	void Camera::Motion(int dx, int dy, PxReal delta_time)
	{
		PxVec3 viewY = dir.cross(PxVec3(0,1,0)).getNormalized();

		PxQuat qx(PxPi * dx * speed * delta_time/ 180.0f, PxVec3(0,1,0));
		dir = qx.rotate(dir);
		PxQuat qy(PxPi * dy * speed * delta_time/ 180.0f, viewY);
		dir = qy.rotate(dir);

		dir.normalize();
	}

	void Camera::AnalogMove(float x, float y)
	{
		PxVec3 viewY = dir.cross(PxVec3(0,1,0)).getNormalized();
		eye += dir*y;
		eye += viewY*x;
	}

	PxVec3 Camera::getEye() const 
	{ 
		return eye; 
	}

	PxVec3 Camera::getDir() const 
	{ 
		return dir; 
	}

	PxTransform	Camera::getTransform() const
	{
		PxVec3 viewY = dir.cross(PxVec3(0,1,0));

		if(viewY.normalize()<1e-6f) 
			return PxTransform(eye);

		PxMat33 m(dir.cross(viewY), viewY, -dir);
		return PxTransform(eye, PxQuat(m));
	}

	void Camera::MoveForward(PxReal delta_time) 
	{ 
		eye += dir*speed*delta_time; 
	}

	void Camera::SetPosition(PxVec3 pos) {
		eye = pos;
	}

	void Camera::SetOrientation(PxVec3 rot) {
		dir = rot;
	}

	void Camera::MoveBackward(PxReal delta_time) 
	{ 
		eye -= dir*speed*delta_time; 
	}

	void Camera::MoveLeft(PxReal delta_time) 
	{
		PxVec3 viewY = dir.cross(PxVec3(0,1,0)).getNormalized();
		eye -= viewY*speed*delta_time; 
	}

	void Camera::MoveRight(PxReal delta_time) 
	{ 
		PxVec3 viewY = dir.cross(PxVec3(0,1,0)).getNormalized();
		eye += viewY*speed*delta_time; 
	}

	void Camera::MoveUp(PxReal delta_time)
	{
		eye += PxVec3(0,1,0)*speed*delta_time; 
	}

	void Camera::MoveDown(PxReal delta_time)
	{
		eye -= PxVec3(0,1,0)*speed*delta_time; 
	}


	// Attempted Port from http://wiki.unity3d.com/index.php/MouseOrbitImproved
	void Camera::UpdateCamera(float mouseX, float mouseY) {				// Moves the camera & rotates it based on mouse movement.
		x += mouseX * xMoveSpeed * distance * 0.02f;
		y -= mouseY * yMoveSpeed * 0.02f;							
		y = ClampAngle(y, yMinLimit, yMaxLimit);						// Ensures we do not go below or above certain Y level 

		PxQuat rot = Euler(PxVec3(y, x, 0));							// converts PxVec3 Euler Angle to a Quaternion

		distance = PxClamp(distance, distanceMin, distanceMax);			// Clamps the distance so we don't go too far from the object (unused for now as we can't dwelve away from the object)
		 
		PxVec3 negDist = PxVec3(0.0f, 0.0f, -distance);
		PxVec3 pos = rot.rotate(negDist) + target->getGlobalPose().p;

		eye = pos;
		dir = PxVec3(y, x, 0);
	}

	float Camera::ClampAngle(float angle, float minA, float maxA) {		// Angle Clamp
		if (angle < -360.0f) 
			angle += 360.0f;
		if (angle > 360.0f)
			angle -= 360.0f;

		return PxClamp(angle, minA, maxA);
	}

	PxQuat Camera::Euler(PxVec3 euler) {								// Converts Euler to Quaternion
		auto yaw = euler.x;
		auto pitch = euler.y;
		auto roll = euler.z;

		PxReal rollOver2 = roll * 0.5f;
		PxReal sinRollOver2 = (PxReal)sin((PxReal)rollOver2);
		PxReal cosRollOver2 = (PxReal)cos((PxReal)rollOver2);

		PxReal pitchOver2 = pitch * 0.5f;
		PxReal sinPitchOver2 = (PxReal)sin((PxReal)pitchOver2);
		PxReal cosPitchOver2 = (PxReal)cos((PxReal)pitchOver2);

		PxReal yawOver2 = yaw * 0.5f;
		PxReal sinYawOver2 = (PxReal)sin((PxReal)yawOver2);
		PxReal cosYawOver2 = (PxReal)cos((PxReal)yawOver2);

		PxQuat result;
		result.x = cosYawOver2 * cosPitchOver2 * cosRollOver2 + sinYawOver2 * sinPitchOver2 * sinRollOver2;
		result.y = cosYawOver2 * cosPitchOver2 * sinRollOver2 - sinYawOver2 * sinPitchOver2 * cosRollOver2;
		result.z = cosYawOver2 * sinPitchOver2 * cosRollOver2 + sinYawOver2 * cosPitchOver2 * sinRollOver2;
		result.w = sinYawOver2 * cosPitchOver2 * cosRollOver2 - cosYawOver2 * sinPitchOver2 * sinRollOver2;
		return result;
	}
}