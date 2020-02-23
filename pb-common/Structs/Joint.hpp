//
//  Joint.h
//  Protocol Bernardo
//
//  Created by Valentin Dufois on 2019-01-25.
//  Copyright © 2019 Prisme. All rights reserved.
//

#ifndef Joint_h
#define Joint_h

#include "../Utils/maths.hpp"
#include "../Messages/body.pb.h"

namespace pb {

/**
 Represent a joint on a skeleton.
 */
struct Joint {
	// MARK: - Properties

    /** The current orientation of the joint */
    maths::vec3 orientation;
    
    /** The confidence of NiTE when giving the orientation */
    SCALAR orientationConfidence;
    
    /** The position in 3D space of the Joint */
	maths::vec3 position;

	/// The position in 2D space of the Joint
	///
	/// @warning: Position of the joint on the screen is a device-dependant feature
	/// that may be removed in the future
	maths::vec2 position2D;
    
    /** The confidence of NiTE when giving the position */
    SCALAR positionConfidence;

	// MARK: - Constructors

	/// Default constructor
	Joint() {}

#ifdef PB_TRACKER

	/// This sconstructor allow for creating a `Joint` from NiTE joint.
	/// @param joint The NiTE joint to use
	/// @param tracker The NiTE tracker used to convert coordinates
	Joint(const nite::SkeletonJoint &joint, const nite::UserTracker &tracker):
		orientation(maths::nQuatToVec3(joint.getOrientation())),
		orientationConfidence(joint.getOrientationConfidence()),
		position(maths::P3FToVec3(joint.getPosition())),
		positionConfidence(joint.getPositionConfidence()) {

		// Get 2D equivalent of the joint position
		float x2D, y2D;

		tracker.convertJointCoordinatesToDepth(position.x, position.y, position.z, &x2D, &y2D);

		position2D.x = x2D;
		position2D.y = y2D;
	}

#endif /* PB_TRACKER */

	// MARK: - Message I/O

	/// Build a Joint using the given message
	/// @param message The message holding the Joint's informations
	Joint(const messages::Joint &message);

	/// Casts the Joint to its equivalent message form.
	operator messages::Joint() const;

	
	// MARK: - Forecast

	/// Calculate the next joint following the two given ones
	/// @param j1 The first joint
	/// @param j2 The second joint, should be older than the first one
	static Joint forecastNext(const Joint &j1, const Joint &j2);

	/// Calculate the next joint following the three given ones
	/// @param j1 The first joint
	/// @param j2 The second joint, should be older than the first one
	/// @param j3 The third joint, should be older than the second one
	static Joint forecastNext(const Joint &j1, const Joint &j2, const Joint &j3);


	// MARK: - Operators

	/// Add the given joint to the current one, multypliying it by its confidence for
	/// each value.
	/// @warning: This method is made to be used when doing a weighted mean of
	/// multiple skeletons
	Joint& operator += (const Joint &j2);
};

/// substract all the properties of the two givn joints
Joint operator - (const Joint &a, const Joint &b);

/// Divide the positions and the orientation by the corresponding confidence,
/// then divide each confidence by the given value.
/// @warning: This method is made to be used when doing a weighted mean of
/// multiple skeletons
Joint operator / (const Joint &a, const SCALAR &div);

} /* ::pb */

#endif /* Joint_h */
