//
//  Body.cpp
//  pb-common
//
//  Created by Valentin Dufois on 2020-02-22.
//  Copyright © 2020 Prisme. All rights reserved.
//

#include "Body.hpp"
#include <glm/gtx/string_cast.hpp>

namespace pb {

// MARK: - Constructors

Body::Body(RawBody * rawBody):
uid(boost::uuids::to_string(boost::uuids::random_generator()())),
rawBodiesUID({{rawBody->deviceUID, rawBody->uid}}),
rawSkeletons({new Skeleton(rawBody->skeleton)}),
devicesUID({rawBody->deviceUID}) {}

Body::Body(const messages::PartialBody &body):
	uid(body.uid()),
	isValid(body.isvalid()),
	frame(body.frame()),
	devicesUID({body.devicesuid().begin(), body.devicesuid().end()}),
	skeletons({new Skeleton(body.skeleton())}) {}


// MARK: - Accessors

Body & Body::insertPartial(const messages::PartialBody &partialBody)  {

	// Verify

	frame = partialBody.frame();

	skeletons.push_back(new Skeleton(partialBody.skeleton()));

	if(skeletons.size() > 5) {
		skeletons.pop_back();
	}


	// Update devices UID list
	devicesUID.clear();
	devicesUID.insert(partialBody.devicesuid().begin(), partialBody.devicesuid().end());

	inactivityCount = 0;

	return *this;
}


// MARK: - Manipulations

bool Body::updatePosition() {
	if(!isValid)
		return true;

	Skeleton * newSkeleton;

	// Is there any rawSkeleton to work with ?
	if(rawSkeletons.size() == 0) {

		if(++inactivityCount > TRACKING_ENGINE_INACTIVITY_THRESHOLD) {
			isValid = false;
			return true;
		}

		if(skeletons.size() == 0) {
			return false;
		}

		// Calculate a position for the body using previous data
		newSkeleton = predictSkeleton();
	} else {
		// Reset body status
		inactivityCount = 0;
		isValid = true;

		newSkeleton = mergeRawSkeletons();
	}

	skeletons.push_front(newSkeleton);

	// Keep history size
	if(skeletons.size() > TRACKING_ENGINE_BODY_HISTORY_SIZE) {
		delete skeletons.back();
		skeletons.back() = nullptr;
		skeletons.pop_back();
	}

	// Increment the frame
	++frame;

	return true;
}

Skeleton * Body::mergeRawSkeletons() {
	Skeleton skeleton;

	// Add all the rawSkeletons
	for(long unsigned int i = 0; i < rawSkeletons.size(); ++i) {
		// TODO: Make sure the skeletons are in the same direction (front back, check the hands)

		skeleton += *rawSkeletons[i];
	}

	// Divide them (weighted mean)
	Skeleton * newSkeleton = new Skeleton(skeleton / rawSkeletons.size());

	// If we already have a skeleton from before, we can use it to fill missing joints, if any
	if(hasSkeleton()) {

		// Check for missing joints
		for(int i = 0; i < newSkeleton->joints.size(); ++i) {
			if(newSkeleton->joints[i].positionConfidence != 0)
				continue; // Joint is ok

			// Joint is missing, did we have it before ?
			if(this->skeleton()->joints[i].positionConfidence == 0)
				continue; // Previous skeleton didn't had it either

			newSkeleton->joints[i] = this->skeleton()->joints[i];
		}
	}

	// Clear the raw skeletons
	clearRawSkeletons();

	return newSkeleton;
}

Skeleton * Body::predictSkeleton() {
	// Select which prediction method to use based on the body's history size
	if(skeletons.size() >= 1) {
		// Only one skeleton in history, just perform a simple copy
		return new Skeleton(*skeleton());
	}

//	if(skeletons.size() >= 2) {
//		// Two skeletons are available, we calculate the new one using the difference between the two.
//
//		LOG_DEBUG(glm::to_string(skeletons.front()->centerOfMass));
//		LOG_DEBUG(glm::to_string((*(++skeletons.begin()))->centerOfMass));
//		return Skeleton::forecastNext(skeletons.front(), *(++skeletons.begin()));
//	}

//	if(skeletons.size() >= 3) {
//		// Three skeletons are available, we calculate the new one using them
//
//		Skeleton * s1 = skeletons.front(); // Current skeleton
//		auto it = ++skeletons.begin();
//		Skeleton * s2 = *it; // Current skeleton - 1
//		++it;
//		Skeleton * s3 = *it; // Current skeleton - 2
//
//		LOG_DEBUG(glm::to_string(skeletons.front()->centerOfMass));
//		return Skeleton::forecastNext(s1, s2, s3); // Our new skeleton
//	}


	return nullptr;
}

void Body::clearRawSkeletons() {
	// Clear the rawSkeletons
	for (Skeleton * skeleton: rawSkeletons) {
		delete skeleton;
		skeleton = nullptr;
	}

	rawSkeletons.clear();
}

} /* ::pb */
