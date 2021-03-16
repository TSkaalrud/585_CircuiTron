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

		for (int j = 0; j < getNumBikes(); j++) {
			std::string numIDString = std::to_string(j);
			char* numIDArray = new char[numIDString.length() + 1];
			strcpy(numIDArray, numIDString.c_str());

			if (strcmp(pairs[i].otherActor->getName(), numIDArray) == 0 &&
				strcmp(pairs[i].triggerActor->getName(), numIDArray) != 0) {

				std::cout << "Bike " << numIDString << " has hit another racers wall!\n" << std::endl;
			
			}
		}
	}
}

void CTColliderCallback::onSleep(PxActor** actors, PxU32 count){}

void CTColliderCallback::onWake(PxActor** actors, PxU32 count) {}

void CTColliderCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}

void CTColliderCallback::onAdvance(
	const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}