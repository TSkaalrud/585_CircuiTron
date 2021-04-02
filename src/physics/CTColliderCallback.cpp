#include "CTColliderCallback.h"
#include "physics.h"
#include <string>
#include <iostream>

void CTColliderCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs){}

void CTColliderCallback::onTrigger(PxTriggerPair* pairs, PxU32 count) {

	for (PxU32 i = 0; i < count; i++) {

		//ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		if (strcmp(pairs[i].otherActor->getName(), "bike") == 0 && 
			strcmp(pairs[i].triggerActor->getName(), "wall") == 0) {

			wallUserData* w = (wallUserData*)pairs[i].triggerActor->userData;

			if (w->collisions < 5) {	//bike triggers wall up to 4 times on bike creation so this is just to ignore that
				w->collisions++;
			} else {					//if wall is triggered otherwise, do this stuff
				std::cout << "bike #" << w->bikeNumber << " wall #" << w->wallIndex << " broken!"  << std::endl;
				bikeUserData* bikeData = (bikeUserData*)pairs[i].otherActor->userData;
				std::cout << bikeData->bikeNumber << std::endl;
				//deleteWallSeg(w->segmentInfo.bikeNumber, w->wallIndex);
			}

		}
	}
}

void CTColliderCallback::onSleep(PxActor** actors, PxU32 count){}

void CTColliderCallback::onWake(PxActor** actors, PxU32 count) {}

void CTColliderCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}

void CTColliderCallback::onAdvance(
	const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}