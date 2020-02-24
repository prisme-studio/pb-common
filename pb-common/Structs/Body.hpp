//
//  Body.hpp
//  Protocol Bernardo
//
//  Created by Valentin Dufois on 2019-11-04.
//

#ifndef Body_h
#define Body_h

#include <string>
#include <list>
#include <unordered_set>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../common.hpp"
#include "../Messages/body.pb.h"

#include "Skeleton.hpp"
#include "RawBody.hpp"

namespace pb {

// A body is a collection of one or more skeletons who's coordinates have been calculated by the Tracking Engine using a layout. A Body has a current position as well as an history of positions.
struct Body {

	// MARK: Properties

	/// Unique identifier for this body
	pb::bodyUID uid;

	/// The current frame of the user
	unsigned int frame = 0;

	/// The skeletons for this user in the global coordinates space. The first one being the current one, and the last one the oldest one in memory
	std::list<Skeleton *> skeletons;

	/// UIDs of all the devices currently tracking this body
	std::unordered_set<pb::deviceUID> devicesUID;

	/// List of RawBody uid paired with their device UID pair from which this body is taking data from
	std::map<pb::deviceUID, pb::rawBodyUID> rawBodiesUID;

	/// All the skeleton composing the current frame of the user
	/// This is used by the TrackingEngine.
	std::vector<Skeleton *> rawSkeletons;

	/// How many cycles since the last time we received information on this body >?
	unsigned int inactivityCount = 0;

	/// Tell if the body is valid. An invalid body does not have new meaningful skeletons.
	/// As of 2019-11-27, an invalid body will always be removed on the next frame.
	bool isValid = true;

	// MARK: - Constructors

	/// Instanciate a Body using the informations provided by the given RawBody
	/// @param rawBody A RawBody sent by a tracker
	Body(RawBody * rawBody);

	/// Instanciate a Body using the informations provided by the given PartialBody
	/// A PartialBody sent by the master
	Body(const messages::PartialBody &body);

	~Body() {
		for(Skeleton * skeleton: skeletons) {
			delete skeleton;
		}

		clearRawSkeletons();
	}

	// MARK: - Accessors

	inline bool hasSkeleton() const { return skeletons.size() > 0; }

	/// Gives the current skeleton of this body, if any
	inline Skeleton * skeleton() const { return hasSkeleton() ? skeletons.front() : nullptr; }

	// MARK: - Manipulations

	Body & insertPartial(const messages::PartialBody &partialBody);

	/// Calculate the weighted mean of all the raw skeletons matching the current body
	/// @returns True if the body has changed or false otherwise
	bool updatePosition();

private:

	Skeleton * mergeRawSkeletons();

	Skeleton * predictSkeleton();

public:

	void clearRawSkeletons();
};

} /* ::pb */

#endif /* Body_h */
