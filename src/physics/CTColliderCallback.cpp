#include "CTColliderCallback.h"
#include "physics.h"
#include <string>
#include <iostream>
#include <PxScene.h>

void CTColliderCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs){}

void CTColliderCallback::onTrigger(PxTriggerPair* pairs, PxU32 count) {

	for (PxU32 i = 0; i < count; i++) {

		//ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		if (strcmp(pairs[i].otherActor->getName(), "bike") == 0 && 
			strcmp(pairs[i].triggerActor->getName(), "wall") == 0) {

			wallUserData* w = (wallUserData*)pairs[i].triggerActor->userData;

			bikeUserData* bikeData = (bikeUserData*)pairs[i].otherActor->userData;

			bikeData->collided = true;
			auto b = getVehicle(bikeData->bikeNumber)->getRigidDynamicActor();
			b->setAngularVelocity(b->getAngularVelocity() * 0.5);
			b->setLinearVelocity(b->getLinearVelocity() * 0.8);
				
			markWallBroken(w->bikeNumber, w->wallIndex);
			

		}
	}
}

void CTColliderCallback::onSleep(PxActor** actors, PxU32 count){}

void CTColliderCallback::onWake(PxActor** actors, PxU32 count) {}

void CTColliderCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}

void CTColliderCallback::onAdvance(
	const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}