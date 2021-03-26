//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2019 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.

// ****************************************************************************
// This snippet illustrates simple use of PxVehicleDrive4W.
//
// It creates a vehicle on a plane and then controls the vehicle so that it performs a
// number of choreographed manoeuvres such as accelerate, reverse, brake, handbrake, and turn.

// It is a good idea to record and playback with pvd (PhysX Visual Debugger).
// ****************************************************************************

#include <ctype.h>

#include "PxPhysicsAPI.h"

#include "CTColliderCallback.h"
#include "CTVehicleCreate.h"
#include "CTVehicleFilterShader.h"
#include "CTVehicleSceneQuery.h"
#include "CTVehicleTireFriction.h"
#include "physics.h"
#include "vehicle/PxVehicleUtil.h"

#include "CTPVD.h"
#include "CTPrint.h"
#include "CTUtils.h"
#include <GLFW/glfw3.h>

#include "OBJ_Loader.h"

#include <glm/common.hpp>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace physx;
using namespace snippetvehicle;

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxCooking* gCooking = NULL;

PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;

std::vector<PxVehicleDrive4W*> CTbikes;
std::vector<std::vector<wallSegment>> walls;
std::vector<PxVehicleDrive4WRawInputData> inputDatas;
std::vector<bool> isVehicleInAir;
std::vector<wallSpawnInfo> wallSpawnTimers;

float impulseBase = 2500;

PxF32 gSteerVsForwardSpeedData[2 * 8] = {0.0f,       0.75f,      5.0f,       0.75f,      30.0f,      0.125f,
										 120.0f,     0.1f,       PX_MAX_F32, PX_MAX_F32, PX_MAX_F32, PX_MAX_F32,
										 PX_MAX_F32, PX_MAX_F32, PX_MAX_F32, PX_MAX_F32};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData = {
	{
		6.0f, // rise rate eANALOG_INPUT_ACCEL
		6.0f, // rise rate eANALOG_INPUT_BRAKE
		6.0f, // rise rate eANALOG_INPUT_HANDBRAKE
		2.5f, // rise rate eANALOG_INPUT_STEER_LEFT
		2.5f, // rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f, // fall rate eANALOG_INPUT_ACCEL
		10.0f, // fall rate eANALOG_INPUT_BRAKE
		10.0f, // fall rate eANALOG_INPUT_HANDBRAKE
		5.0f,  // fall rate eANALOG_INPUT_STEER_LEFT
		5.0f   // fall rate eANALOG_INPUT_STEER_RIGHT
	}};

PxVehiclePadSmoothingData gPadSmoothingData = {
	{
		6.0f, // rise rate eANALOG_INPUT_ACCEL
		6.0f, // rise rate eANALOG_INPUT_BRAKE
		6.0f, // rise rate eANALOG_INPUT_HANDBRAKE
		2.5f, // rise rate eANALOG_INPUT_STEER_LEFT
		2.5f, // rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f, // fall rate eANALOG_INPUT_ACCEL
		10.0f, // fall rate eANALOG_INPUT_BRAKE
		10.0f, // fall rate eANALOG_INPUT_HANDBRAKE
		5.0f,  // fall rate eANALOG_INPUT_STEER_LEFT
		5.0f   // fall rate eANALOG_INPUT_STEER_RIGHT
	}};

// PxVehicleDrive4WRawInputData gVehicleInputData;

enum DriveMode {
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

DriveMode gDriveModeOrder[] = {
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_NONE};

PxF32 gVehicleModeLifetime = 4.0f;
PxF32 gVehicleModeTimer = 0.0f;
PxU32 gVehicleOrderProgress = 0;
bool gVehicleOrderComplete = false;
bool gMimicKeyInputs = false;

VehicleDesc initVehicleDesc() {
	// Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	// The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	// Center of mass offset is 0.65m above the base of the chassis and 0.55m towards the front.
	const PxF32 chassisMass = 250.0f;
	const PxVec3 chassisDims(1.0f, 1.2f, 4.63f);
	const PxVec3 chassisMOI(
		(chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.55f);

	// Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	// Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 15.0f;
	const PxF32 wheelRadius = 0.3f;
	const PxF32 wheelWidth = 0.3f;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = gMaterial;
	vehicleDesc.wheelSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

/*

void startAccelerateForwardsMode() {
	if (gMimicKeyInputs) {
		gVehicleInputData.setDigitalAccel(true);
	} else {
		gVehicleInputData.setAnalogAccel(1.0f);
	}
}

void startAccelerateReverseMode() {

	if (gMimicKeyInputs) {
		gVehicleInputData.setDigitalAccel(true);
	} else {
		gVehicleInputData.setAnalogAccel(1.0f);
	}
}

void startBrakeMode() {
	if (gMimicKeyInputs) {
		gVehicleInputData.setDigitalBrake(true);
	} else {
		gVehicleInputData.setAnalogBrake(1.0f);
	}
}

void startTurnHardLeftMode() {
	if (gMimicKeyInputs) {
		gVehicleInputData.setDigitalAccel(true);
		gVehicleInputData.setDigitalSteerLeft(true);
	} else {
		gVehicleInputData.setAnalogAccel(true);
		gVehicleInputData.setAnalogSteer(-1.0f);
	}
}

void startTurnHardRightMode() {
	if (gMimicKeyInputs) {
		gVehicleInputData.setDigitalAccel(true);
		gVehicleInputData.setDigitalSteerRight(true);
	} else {
		gVehicleInputData.setAnalogAccel(1.0f);
		gVehicleInputData.setAnalogSteer(1.0f);
	}
}

void startHandbrakeTurnLeftMode() {
	if (gMimicKeyInputs) {
		gVehicleInputData.setDigitalSteerLeft(true);
		gVehicleInputData.setDigitalHandbrake(true);
	} else {
		gVehicleInputData.setAnalogSteer(-1.0f);
		gVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void startHandbrakeTurnRightMode() {
	if (gMimicKeyInputs) {
		gVehicleInputData.setDigitalSteerRight(true);
		gVehicleInputData.setDigitalHandbrake(true);
	} else {
		gVehicleInputData.setAnalogSteer(1.0f);
		gVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void releaseAllControls() {
	if (gMimicKeyInputs) {
		gVehicleInputData.setDigitalAccel(false);
		gVehicleInputData.setDigitalSteerLeft(false);
		gVehicleInputData.setDigitalSteerRight(false);
		gVehicleInputData.setDigitalBrake(false);
		gVehicleInputData.setDigitalHandbrake(false);
	} else {
		gVehicleInputData.setAnalogAccel(0.0f);
		gVehicleInputData.setAnalogSteer(0.0f);
		gVehicleInputData.setAnalogBrake(0.0f);
		gVehicleInputData.setAnalogHandbrake(0.0f);
	}
}

*/

void makeWallSeg(int i, PxTransform a, PxTransform b) {
	PxTransform wallSeg;

	// direction vector
	PxVec3 dir = b.p - a.p;

	// up vector
	PxVec3 up = a.q.getBasisVector1();

	// length
	float length = dir.magnitude();

	// calculate matrix for conversion to quaternion
	PxVec3 sid = up.cross(dir);
	PxVec3 nup = dir.cross(sid);

	nup.normalize();
	dir.normalize();
	sid.normalize();

	PxMat33 rotMat = PxMat33(sid, nup, dir);

	PxQuat wallQuat = PxQuat(rotMat);

	// set position and rotation of wall segment
	wallSeg.p = (a.p + b.p) / 2.0f;
	wallSeg.q = wallQuat;

	// make wall segment
	PxShape* wallShape = gPhysics->createShape(PxBoxGeometry(0.1f, 0.6f, length / 2.0f), *gMaterial);

	// set query filter data of the wall so that the vehicle raycasts can hit the wall
	PxFilterData qryFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	wallShape->setQueryFilterData(qryFilterData);

	// set simulation filter data of the wall so that the vehicle collides with the wall
	PxFilterData wallSimFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	wallShape->setSimulationFilterData(wallSimFilterData);

	// turn off simulation for walls (disable collisions)
	wallShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);

	// make walls trigger volumes
	wallShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	PxRigidStatic* aWall = gPhysics->createRigidStatic(wallSeg);
	aWall->attachShape(*wallShape);

	// set the actor name
	aWall->setName("wall");

	// wall segment data
	wallSegment segment = {i, aWall, b, a};
	walls[i].push_back(segment);

	// wall user data
	wallUserData* wallData = new wallUserData{segment,(int) walls[i].size() - 1, (int)0};
	aWall->userData = wallData;

	// add actor to scene
	gScene->addActor(*aWall);
}

void deleteWallSeg(int i, int j) { 
	walls[i][j].wall->release(); 
	walls[i].erase(walls[i].begin() + j);
}

// basic wall generation
void spawnWall(PxF32 timestep, int i) {
	PxVehicleDrive4W* vehicle = CTbikes[i];

	wallSpawnTimers[i].timer += timestep;

	if (vehicle->computeForwardSpeed() >= 10.0f &&
		vehicle->mDriveDynData.getCurrentGear() != PxVehicleGearsData::eREVERSE) {
		if (wallSpawnTimers[i].timer >= wallSpawnTimers[i].wallTime) {
			if (wallSpawnTimers[i].wallFront.p.x != NULL) {
				wallSpawnTimers[i].wallBack = wallSpawnTimers[i].wallFront;
				wallSpawnTimers[i].wallFront = vehicle->getRigidDynamicActor()->getGlobalPose();

				makeWallSeg(i, wallSpawnTimers[i].wallBack, wallSpawnTimers[i].wallFront);
			}
			wallSpawnTimers[i].wallFront = vehicle->getRigidDynamicActor()->getGlobalPose();
			wallSpawnTimers[i].timer = 0.0f;
		}
	} else {
		wallSpawnTimers[i].wallFront.p.x = NULL;
	}
}

// cast a ray from a bike in a given direction and at a given range
PxRaycastBuffer castRay(int bike, int dir, int range) { 
	int FRONT = 0;
	int BACK = 1;
	int LEFT = 2;
	int RIGHT = 3;

	PxVehicleDrive4W* vehicle = CTbikes[bike];
	auto origin = vehicle->getRigidDynamicActor()->getGlobalPose().p;
	auto quat = vehicle->getRigidDynamicActor()->getGlobalPose().q;

	PxVec3 heading;

	// https://www.gamedev.net/forums/topic/56471-extracting-direction-vectors-from-quaternion/1273785

	if (dir == FRONT) {
		heading = getBikeTransform(bike).q.getBasisVector2();
	}else if (dir == BACK) {
		heading = -getBikeTransform(bike).q.getBasisVector2();
	}else if (dir == LEFT) {
		heading = getBikeTransform(bike).q.getBasisVector0();
	}else if (dir == RIGHT) {
		heading = -getBikeTransform(bike).q.getBasisVector0();
	}

	heading.normalize();

	const PxU32 bufferSize = 1;                 // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];         // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize); // [out] Blocking and touching hits stored here

	gScene->raycast(origin, heading, range, buf);
	
	// https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/SceneQueries.html?highlight=ray#multiple-hits
		
	for (PxU32 i = 0; i < buf.nbTouches; i++) {
		std::cout << "RAY CAST RESULTS ON OBJECT: " << buf.touches[i].actor->getName() << "\n";
	}

	return buf.touches;
}

physx::PxTransform trackTransform;
physx::PxTransform getTrackTransform() { return trackTransform; }

// return number of bikes
int getNumBikes() { return CTbikes.size(); }

// get bike transforms (i = bike number)
PxTransform getBikeTransform(int i) { return CTbikes[i]->getRigidDynamicActor()->getGlobalPose(); }

// getter methods for bike walls (i = bike number, j = wall segment number)
physx::PxTransform getWallPos(int i, int j) { return walls[i][j].wall->getGlobalPose(); };

physx::PxTransform getWallFront(int i, int j) { return walls[i][j].front; };

physx::PxTransform getWallBack(int i, int j) { return walls[i][j].back; };

int getNumWalls(int i) { return walls[i].size(); }

// accelerate function used for input
void bikeAccelerate(int i) {
	if (CTbikes[i]->mDriveDynData.getCurrentGear() == PxVehicleGearsData::eREVERSE) {
		CTbikes[i]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	} else {
		inputDatas[i].setAnalogAccel(1.0f);
	}
}

void bikeAcceleratePrecise(int i, float n) {
	if (CTbikes[i]->mDriveDynData.getCurrentGear() == PxVehicleGearsData::eREVERSE) {
		CTbikes[i]->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	} else {
		inputDatas[i].setAnalogAccel(n);
	}
}

// reverse function used for input
void bikeReverse(int i) {
	// if (CTbikes[i]->mDriveDynData.getCurrentGear() != PxVehicleGearsData::eREVERSE &&
	//	CTbikes[i]->computeForwardSpeed() > 0.f) {
	//	bikeBreak(i);
	//}
	// else
	if (CTbikes[i]->mDriveDynData.getCurrentGear() != PxVehicleGearsData::eREVERSE) {
		CTbikes[i]->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
	} else {
		inputDatas[i].setAnalogAccel(1.0f);
	}
}

// brake function for input
void bikeBreak(int i) { inputDatas[i].setAnalogBrake(1.0f); }

// handbrake function for input
void bikeHandbrake(int i) { inputDatas[i].setAnalogHandbrake(0.5f); }

// turn functions used for input
void bikeTurnRight(int i) { inputDatas[i].setAnalogSteer(glm::max(inputDatas[i].getAnalogSteer() - 0.05f, -1.0f)); }

void bikeTurnLeft(int i) { inputDatas[i].setAnalogSteer(glm::min(inputDatas[i].getAnalogSteer() + .05f, 1.f)); }

void bikeTurnPrecise(int i, float n) { inputDatas[i].setAnalogSteer(n); }

// gas/turn/brake release functions used for input
void bikeReleaseGas(int i) { inputDatas[i].setAnalogAccel(0.0f); }

void bikeReleaseSteer(int i) { inputDatas[i].setAnalogSteer(0.0f); }

void bikeReleaseBrake(int i) { inputDatas[i].setAnalogBrake(0.0f); }

void bikeReleaseHandbrake(int i) { inputDatas[i].setAnalogHandbrake(0.f); }

void bikeReleaseAll(int i) {
	inputDatas[i].setAnalogAccel(0.0f);
	inputDatas[i].setAnalogSteer(0.0f);
	inputDatas[i].setAnalogBrake(0.0f);
	inputDatas[i].setAnalogHandbrake(0.0f);
}

// bikeBooster provides a powerful impulse either to jump up, or strafe to the left or right of the bike's heading
void bikeBooster(int bike, int keyPressed) {
	if (keyPressed == 265) { // up
		physx::PxVec3 up = getBikeTransform(bike).q.getBasisVector1() * impulseBase;
		CTbikes[bike]->getRigidDynamicActor()->addForce(up, PxForceMode::eIMPULSE);
	} else if (keyPressed == 263) { // left
		physx::PxVec3 left;
		if (isVehicleInAir[bike]) {
			left = getBikeTransform(bike).q.getBasisVector0() * .5 * impulseBase;
		} else {
			left = getBikeTransform(bike).q.getBasisVector0() * 2 * impulseBase;
		}
		CTbikes[bike]->getRigidDynamicActor()->addForce(left, PxForceMode::eIMPULSE);
	} else if (keyPressed == 262) { // right
		physx::PxVec3 right;
		if (isVehicleInAir[bike]) {
			right = getBikeTransform(bike).q.getBasisVector0() * -.5 * impulseBase;
		} else {
			right = getBikeTransform(bike).q.getBasisVector0() * -2 * impulseBase;
		}
		CTbikes[bike]->getRigidDynamicActor()->addForce(right, PxForceMode::eIMPULSE);
	}
}

/* keep bike in euler angle terms between
-0.5 and +0.5 rads in the x (pitch)
don't care about y (yaw)
pi/3 and 2*pi/3 rads (roll)
*/
void bikeControl(int bike) {

	// std::cout << getBikeTransform(bike).q.getBasisVector1().dot(physx::PxVec3{0,1,0}) << std::endl;
	physx::PxTransform quat = getBikeTransform(bike);
	// std::cout << quat.q.getAngle() << std::endl;
	float angleRadians;
	PxVec3 unitAxis;
	quat.q.toRadiansAndUnitAxis(angleRadians, unitAxis);
	std::cout << angleRadians << std::endl;
	std::cout << unitAxis.x << " " << unitAxis.y << " " << unitAxis.z << std::endl;
}

PxTransform startPos;

//reset bike to start position
void resetBikePos(int bike, PxTransform location) { 
	CTbikes[bike]->getRigidDynamicActor()->setGlobalPose(location);
	CTbikes[bike]->setToRestState();
	
}

// fetch bike gear
PxU32 getBikeGear(int bike) {
	// std::cout << PxVehicleGearsData::eGEARSRATIO_COUNT << std::endl;
	// std::cout << CTbikes[bike]->mDriveDynData.getCurrentGear() << std::endl;
	return CTbikes[bike]->mDriveDynData.getCurrentGear();
}

float spawnOffset = 0.0f;

void initVehicle() {
	// Data each bike needs
	// input data
	PxVehicleDrive4WRawInputData gVehicleInputData;
	inputDatas.push_back(gVehicleInputData);

	// wall arrays
	std::vector<wallSegment> bikeWalls;
	walls.push_back(bikeWalls);

	// is vehicle in air
	bool gIsVehicleInAir = true;
	isVehicleInAir.push_back(gIsVehicleInAir);

	wallSpawnInfo wallInfo;
	wallInfo.timer = 0.0f;
	wallInfo.wallTime = 1.0f / 6.0f;

	wallSpawnTimers.push_back(wallInfo);

	PxVehicleDrive4W* gVehicle4W;

	VehicleDesc vehicleDesc = initVehicleDesc();
	gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);
	PxTransform startTransform(
		PxVec3(-175.0f - spawnOffset, (vehicleDesc.chassisDims.y * 1.f + vehicleDesc.wheelRadius + 3.0f), -110.0f),
		PxQuat(0.0f, 0.999f, 0.0f, -0.052f));

	if (CTbikes.size() == 0) {
		startPos = startTransform;
	}

	spawnOffset += 5.0f;

	CTbikes.push_back(gVehicle4W);

	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	
	// set the actor name
	gVehicle4W->getRigidDynamicActor()->setName("bike");

	gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	// Set the vehicle to rest in first gear.
	// Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);

	gVehicleModeTimer = 0.0f;
	gVehicleOrderProgress = 0;
	bikeBreak(CTbikes.size() - 1);
	bikeReleaseAll(CTbikes.size() - 1);
}

physx::PxTriangleMesh* cookTrack() {

	objl::Loader loader;

	loader.LoadFile("assets/The_Coffin_cooked.obj");

	std::vector<objl::Vertex> verts = loader.LoadedMeshes[0].Vertices;
	std::vector<PxVec3> verts2;

	for (int i = 0; i < verts.size(); i++) {
		verts2.push_back(PxVec3(verts[i].Position.X, verts[i].Position.Y, verts[i].Position.Z) * 2.0f);
	}

	std::vector<unsigned int> indices = loader.LoadedMeshes[0].Indices;
	std::cout << "indices: " << indices.size() << std::endl;

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = verts2.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = &verts2[0];

	meshDesc.triangles.count = indices.size() / 3;
	meshDesc.triangles.stride = 3 * sizeof(unsigned int);
	meshDesc.triangles.data = &indices[0];

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;

	bool status = gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status) {
		return NULL;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	return gPhysics->createTriangleMesh(readBuffer);
}

void initPhysics() {
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;

	// set collision callback in order to use trigger volumes
	CTColliderCallback* colliderCallback = new CTColliderCallback();
	sceneDesc.simulationEventCallback = colliderCallback;

	gScene = gPhysics->createScene(sceneDesc);
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.25f, 0.25f, 0.6f);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	/////////////////////////////////////////////

	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	// Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData =
		VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	// Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	// Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gGroundPlane->setName("Ground Plane");
	gScene->addActor(*gGroundPlane);

	// Cook track
	PxTriangleMesh* trackMesh = cookTrack();
	PxShape* trackShape = gPhysics->createShape(PxTriangleMeshGeometry(trackMesh), *gMaterial);

	PxFilterData trackFilterData;
	setupDrivableSurface(trackFilterData);
	trackShape->setQueryFilterData(trackFilterData);

	PxFilterData trackSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	trackShape->setSimulationFilterData(trackSimFilterData);

	PxRigidStatic* track = gPhysics->createRigidStatic(trackShape->getLocalPose());
	track->setName("Track");
	track->attachShape(*trackShape);
	gScene->addActor(*track);

	// Create vehicle that will drive on the plane. (This one is the player)
	initVehicle();
}


void stepPhysics() {
	const PxF32 timestep = 1.0f / 60.0f;

	for (int i = 0; i < CTbikes.size(); i++) {
		spawnWall(timestep, i);
	}

	for (int i = 0; i < CTbikes.size(); i++) {
		if (gMimicKeyInputs) {
			PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(
				gKeySmoothingData, gSteerVsForwardSpeedTable, inputDatas[i], timestep, isVehicleInAir[i], *CTbikes[i]);
		} else {
			PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(
				gPadSmoothingData, gSteerVsForwardSpeedTable, inputDatas[i], timestep, isVehicleInAir[i], *CTbikes[i]);
		}
	}

	for (int i = 0; i < CTbikes.size(); i++) {
		// Raycasts.
		PxVehicleWheels* vehicles[1] = {CTbikes[i]};
		PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

		// Vehicle update.
		const PxVec3 grav = gScene->getGravity();
		PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		PxVehicleWheelQueryResult vehicleQueryResults[1] = {
			{wheelQueryResults, CTbikes[i]->mWheelsSimData.getNbWheels()}};
		PxVehicleUpdates(timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

		// Work out if the vehicle is in the air.
		isVehicleInAir[i] =
			CTbikes[i]->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
	}

	// Scene update.
	gScene->simulate(timestep);
	gScene->fetchResults(true);
}

void cleanupPhysics() {
	for (int i = 0; i < CTbikes.size(); i++) {
		CTbikes[i]->getRigidDynamicActor()->release();
		CTbikes[i]->free();
	}

	PX_RELEASE(gGroundPlane);
	PX_RELEASE(gBatchQuery);
	gVehicleSceneQueryData->free(gAllocator);
	PX_RELEASE(gFrictionPairs);
	PxCloseVehicleSDK();

	PX_RELEASE(gMaterial);
	PX_RELEASE(gCooking);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd) {
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();
		gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);

	printf("SnippetVehicle4W done.\n");
}

void keyPress(unsigned char key, const PxTransform& camera) {
	PX_UNUSED(camera);
	PX_UNUSED(key);
}
