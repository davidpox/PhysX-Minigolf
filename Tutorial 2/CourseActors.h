#pragma once

#include "PhysicsEngine.h"
#include <iostream>
#include <iomanip>
#include "BasicActors.h"

namespace PhysicsEngine {

	enum SURFACE_TYPES {
		S_NORMAL,
		S_SAND,
		S_ICE
	};

	static const PxVec3 color_palette[] = {
		PxVec3(66.f / 255.f, 134.f / 255.f, 244.f / 255.f),	// soft blue
		PxVec3(82.f / 255.f, 175.f / 255.f, 35.f / 255.f),	// course green
		PxVec3(156.f / 255.f, 84.f / 255.f, 16.f / 255.f),	// wall brown
		PxVec3(1.0f, 1.0f, 1.0f),							// hwhite
		PxVec3(0.3f, 0.3f, 0.3f),							// dark hwhite
		PxVec3(214.f / 255.f, 184.f / 255.f, 109.f / 255.f),// sand
		PxVec3(167.f / 255.f, 201.f / 255.f, 232.f / 255.f)	// ice
	};

	const std::vector<PxVec3> wedge_verticies = {
		PxVec3(-1.8f, 0.3f, -1.8f),
		PxVec3(1.8f, 0.3f, 1.8f),
		PxVec3(-1.8f, -0.3f, -1.8f),
		PxVec3(1.8f, -0.3f, 1.8f),
		PxVec3(-1.8f, 0.3f, 1.8f),
		PxVec3(-1.8f, -0.3f, 1.8f)
	};


	// Done
	class PathStraight : public StaticActor {
	public: 
		PathStraight(const PxTransform& pose = PxTransform(PxIdentity), SURFACE_TYPES surface = S_NORMAL) 
			: StaticActor(pose) {
			// SHAPE CREATION

			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.7f, 4.0f)));			 // BASE
			GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, -0.3f, 0.0f)));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// LEFT WALL
			GetShape(1)->setLocalPose(PxTransform(-3.8f, 0.0f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// RIGHT WALL
			GetShape(2)->setLocalPose(PxTransform(3.8f, 0.0f, 0.0f));

			Color(color_palette[1], 0);
			Color(color_palette[2], 1);
			Color(color_palette[2], 2);

			if (surface == S_SAND) {
				Color(color_palette[5], 0);
				PxMaterial* sandMaterial = CreateMaterial(PX_MAX_F32, PX_MAX_F32, 0.0f);
				GetShape(0)->setMaterials(&sandMaterial, 1);
			}
			else if (surface == S_ICE) {
				Color(color_palette[6], 0);
				PxMaterial* iceMaterial = CreateMaterial(0.0f, 0.0f, 0.1f);
				GetShape(0)->setMaterials(&iceMaterial, 1);
			}
			else {
				Color(color_palette[1], 0);
				PxMaterial* normalMaterial = CreateMaterial(0.4f, 2.1f, 0.1f);
				GetShape(0)->setMaterials(&normalMaterial, 1);
			}
		}
	};

	// Done
	class PathHole : public StaticActor {
	public:
		PathHole(const PxTransform& pose = PxTransform(PxIdentity))
			: StaticActor(pose) {

			CreateShape(PxBoxGeometry(PxVec3(3.6f, 1.0f, 0.2f)));			// BACK WALL
			GetShape(0)->setLocalPose(PxTransform(0.0f, 0.0f, -3.8f));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// LEFT WALL
			GetShape(1)->setLocalPose(PxTransform(-3.8f, 0.0f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// RIGHT WALL
			GetShape(2)->setLocalPose(PxTransform(3.8f, 0.0f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(2.0f, 0.7f, 1.6f)));			// BASE TOP LEFT
			GetShape(3)->setLocalPose(PxTransform(-1.6f, -0.3f, -2.0f));
			CreateShape(PxBoxGeometry(PxVec3(1.6f, 0.7f, 2.0f)));			// BASE TOP RIGHT
			GetShape(4)->setLocalPose(PxTransform(2.0f, -0.3f, -1.6f));
			CreateShape(PxBoxGeometry(PxVec3(2.0f, 0.7f, 1.8f)));			// BASE BOTTOM RIGHT
			GetShape(5)->setLocalPose(PxTransform(1.6f, -0.3f, 2.2f));
			CreateShape(PxBoxGeometry(PxVec3(1.6f, 0.7f, 2.2f)));			// BASE BOTTOM LEFT
			GetShape(6)->setLocalPose(PxTransform(-2.0f, -0.3f, 1.8f));
			
			Color(color_palette[2], 0);
			Color(color_palette[2], 1);
			Color(color_palette[2], 2);
			Color(color_palette[1], 3);
			Color(color_palette[1], 4);
			Color(color_palette[1], 5);
			Color(color_palette[1], 6);
		}
	};

	// Done
	class PathStart : public StaticActor {
	public:
		PathStart(const PxTransform& pose = PxTransform(PxIdentity))
			: StaticActor(pose) {

			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.7f, 3.8f)));			 // BASE
			GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, -0.3f, -0.2f)));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// LEFT WALL
			GetShape(1)->setLocalPose(PxTransform(-3.8f, 0.0f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// RIGHT WALL
			GetShape(2)->setLocalPose(PxTransform(3.8f, 0.0f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(3.6f, 1.0f, 0.2f)));			// FRONT WALL
			GetShape(3)->setLocalPose(PxTransform(0.0f, 0.0f, 3.8f));

			Color(color_palette[1], 0);
			Color(color_palette[2], 1);
			Color(color_palette[2], 2);
			Color(color_palette[2], 3);
		}
	};

	// Done
	class PathCorner : public StaticActor {
	public: 
		PathCorner(const PxTransform& pose = PxTransform(PxIdentity), bool slanted = false)
			: StaticActor(pose) {

			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// LEFT WALL
			GetShape(0)->setLocalPose(PxTransform(-3.8f, 0.0f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(3.8f, 1.0f, 0.2f)));			// TOP WALL
			GetShape(1)->setLocalPose(PxTransform(0.2f, 0.0f, -3.8f));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 0.2f)));			// INSIDER CORNER NOOK
			GetShape(2)->setLocalPose(PxTransform(3.8f, 0.0f, 3.8f));
			CreateShape(PxBoxGeometry(PxVec3(3.8f, 0.7f, 3.6f)));			// BASE LARGE
			GetShape(3)->setLocalPose(PxTransform(0.2f, -0.3f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.7f, 0.2f)));			// BASE SMALL
			GetShape(4)->setLocalPose(PxTransform(0.0f, -0.3f, 3.8f));			

			if (slanted) {
				std::cout << "MAKING SLANTED SHAPE" << std::endl;
				PxShape* wedge = ConvexMesh(wedge_verticies).GetShape(0);
				PxConvexMeshGeometry wedge2;
				if (wedge->getConvexMeshGeometry(wedge2)) {
					CreateShape(wedge2);
					GetShape(5)->setLocalPose(PxTransform(PxVec3(-1.8f, 0.7f, -1.8f), PxQuat(-1.5708f, PxVec3(0, 1, 0))));
					Color(color_palette[2], 5);
				}
			}

			Color(color_palette[2], 0);
			Color(color_palette[2], 1);
			Color(color_palette[2], 2);
			Color(color_palette[1], 3);
			Color(color_palette[1], 4);	
		}
	};

	// Done
	class PathSharpIncline : public StaticActor {
	public: 
		PathSharpIncline(const PxTransform& pose = PxTransform(PxIdentity))
			: StaticActor(pose) {

			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// LEFT WALL
			GetShape(0)->setLocalPose(PxTransform(-3.8f, 0.0f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// RIGHT WALL
			GetShape(1)->setLocalPose(PxTransform(3.8f, 0.0f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.7f, 1.2f)));			// FRONT BASE
			GetShape(2)->setLocalPose(PxTransform(0.0f, -0.3f, 2.8f));
			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.7f, 1.2f)));			// BACK BASE
			GetShape(3)->setLocalPose(PxTransform(0.0f, -0.3f, -2.8f));
			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.65f, 1.2f)));			// FRONT ROTATED
			GetShape(4)->setLocalPose(PxTransform(PxVec3(0.0f, 0.7f, -0.39f), PxQuat(-0.785398, PxVec3(1, 0, 0))));
			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.65f, 1.2f)));			// BACK ROTATED
			GetShape(5)->setLocalPose(PxTransform(PxVec3(0.0f, 0.7f, 0.39f), PxQuat(0.785398, PxVec3(1, 0, 0))));

			Color(color_palette[2], 0);
			Color(color_palette[2], 1);
			Color(color_palette[1], 2);
			Color(color_palette[1], 3);
			Color(color_palette[1], 4);
			Color(color_palette[1], 5);
		}
	};

	// Done
	class WindMill : public DynamicActor {
	public:
		WindMill(const PxTransform& pose = PxTransform(PxIdentity))
			: DynamicActor(pose) {
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 0.2f, 3.3f)), 1.0f);
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 3.3f, 0.2f)), 1.0f);

			Color(color_palette[2], 0);
			Color(color_palette[2], 1);

		}
	};
	
	// Done
	class PathWindMill : public StaticActor {
	public:
		PathWindMill(const PxTransform& pose = PxTransform(PxIdentity))
			: StaticActor(pose) {
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// LEFT WALL
			GetShape(0)->setLocalPose(PxTransform(-3.8f, 0.0f, 0.0f));

			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// RIGHT WALL
			GetShape(1)->setLocalPose(PxTransform(3.8f, 0.0f, 0.0f));

			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.7f, 4.0f)));			 // BASE
			GetShape(2)->setLocalPose(PxTransform(0.0f, -0.3f, 0.0f));

			CreateShape(PxBoxGeometry(PxVec3(0.15f, 1.8f, 0.4f)));			// LEFT POST
			GetShape(3)->setLocalPose(PxTransform(-3.45f, 2.2f, 0.0f));

			CreateShape(PxBoxGeometry(PxVec3(0.15f, 1.8f, 0.4f)));			// RIGHT POST
			GetShape(4)->setLocalPose(PxTransform(3.45f, 2.2f, 0.0f));

			CreateShape(PxBoxGeometry(PxVec3(3.3f, 0.2f, 0.4f)));			// TOP POST
			GetShape(5)->setLocalPose(PxTransform(0.0f, 3.8f, 0.0f));

			CreateShape(PxBoxGeometry(PxVec3(1.4f, 0.2f, 0.4f)));			// BOTTOM LEFT POST
			GetShape(6)->setLocalPose(PxTransform(-1.9f, 0.6f, 0.0f));

			CreateShape(PxBoxGeometry(PxVec3(1.4f, 0.2f, 0.4f)));			// BOTTOM RIGHT POST
			GetShape(7)->setLocalPose(PxTransform(1.9f, 0.6f, 0.0f));		

			Color(color_palette[2], 0);
			Color(color_palette[2], 1);
			Color(color_palette[1], 2);
			Color(color_palette[4], 3);
			Color(color_palette[4], 4);
			Color(color_palette[4], 5);
			Color(color_palette[4], 6);
			Color(color_palette[4], 7);
		}
	};
	
	// Done
	class PathTunnel : public StaticActor {
	public:
		PathTunnel(const PxTransform& pose = PxTransform(PxIdentity))
			: StaticActor(pose) {

			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.7f, 4.0f)));			 // BASE
			GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, -0.3f, 0.0f)));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// LEFT WALL
			GetShape(1)->setLocalPose(PxTransform(-3.8f, 0.0f, 0.0f));
			CreateShape(PxBoxGeometry(PxVec3(0.2f, 1.0f, 4.0f)));			// RIGHT WALL
			GetShape(2)->setLocalPose(PxTransform(3.8f, 0.0f, 0.0f));

			CreateShape(PxBoxGeometry(PxVec3(1.6f, 0.4f, 4.0f)));			// TUNNEL LEFT WALL
			GetShape(3)->setLocalPose(PxTransform(-2.0f, 0.8f, 0.0));

			CreateShape(PxBoxGeometry(PxVec3(1.6f, 0.4f, 4.0f)));			// TUNNEL RIGHT WALL
			GetShape(4)->setLocalPose(PxTransform(2.0f, 0.8f, 0.0f));

			CreateShape(PxBoxGeometry(PxVec3(3.6f, 0.4f, 4.0f)));			// TUNNEL TOP WALL
			GetShape(5)->setLocalPose(PxTransform(0.0f, 1.6f, 0.0f));

			Color(color_palette[1], 0);
			Color(color_palette[2], 1);
			Color(color_palette[2], 2);
			Color(color_palette[4], 3);
			Color(color_palette[4], 4);
			Color(color_palette[4], 5);

			

		}
	};
}