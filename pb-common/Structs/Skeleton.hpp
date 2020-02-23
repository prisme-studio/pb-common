//
//  Skeleton.hpp
//  Protocol Bernardo
//
//  Created by Valentin Dufois on 2019-01-25.
//  Copyright © 2019 Prisme. All rights reserved.
//

#ifndef Skeleton_hpp
#define Skeleton_hpp

#include <array>
#include <stdexcept>

#include <common/log.hpp>
#include <common/misc.hpp>

#include "../Messages/body.pb.h"
#include "../Utils/maths.hpp"

#include "Joint.hpp"

namespace pb {

/**
 Represent a user skeleton/rig. Holds all the joints composing it
 Its values are irrelevant is the user state isn't USER_TRACKED
 */
struct Skeleton  {
	// MARK: - Properties

	/// Precise time of when the skeleton was created
	const long time = timing::millisSinceStart();

	/// Stores all the joints in the skeleton
	std::array<pb::Joint, 15> joints;

	/// The center of mass of the skeleton
	maths::vec3 centerOfMass;

	// Defines all the joints stored in a Skeleton
	enum JointID {
		head = 0,
		neck = 1,
		leftShoulder = 2,
		rightShoulder = 3,
		leftElbow = 4,
		rightElbow = 5,
		leftHand = 6,
		rightHand = 7,
		torso = 8,
		leftHip = 9,
		rightHip = 10,
		leftKnee = 11,
		rightKnee = 12,
		leftFoot = 13,
		rightFoot = 14
	};

	// MARK: - Constructors

	/// Default constructor
	Skeleton() = default;

	/// Copy constructor
	Skeleton(const Skeleton &);

	/// Build a skeleton from a message
	Skeleton(const messages::Skeleton &);

	// MARK: - Joints access

	/// Convenient access to the specified joint
	pb::Joint & operator [](JointID &jointID) {
		return joints[jointID];
	}

	/// Replace the value of the specified joint with the given one
	void set(const JointID &jointID, const pb::Joint &joint) {
		joints[jointID] = joint;
	}

	// MARK: - Forecast

	/// Calculate the next skeleton following the two given ones
	/// @param s1 The first skeleton
	/// @param s2 The second skeleton, should be older than the first one
	static Skeleton * forecastNext(const Skeleton * s1, const Skeleton * s2);

	/// Calculate the next skeleton following the three given ones
	/// @param s1 The first skeleton
	/// @param s2 The second skeleton, should be older than the first one
	/// @param s3 The third skeleton, should be older than the second one
	static Skeleton * forecastNext(const Skeleton * s1, const Skeleton * s2, const Skeleton * s3);

	// MARK: - Operators

	Skeleton & operator = (const Skeleton &a) {
		joints = a.joints;
		centerOfMass = a.centerOfMass;

		return *this;
	}

	/// Casts the Skeleton to its representing message
	operator messages::Skeleton * () const;

	// MARK: Mathematics operator

	Skeleton& operator += (const Skeleton &s2);

	Skeleton operator / (const SCALAR &div);
};

Skeleton operator - (const Skeleton &a, const Skeleton &b);

} /* ::pb */

#endif /* Skeleton_hpp */
