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

	/// Stores all the joints in the skeleton
	std::array<pb::Joint, 15> joints;

	/// The center of mass of the skeleton
	maths::vec3 centerOfMass;


	// MARK: - Enum

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

	// MARK: - Constructor

	/// Default constructor
	Skeleton() = default;

	/// Copy constructor
	Skeleton(const Skeleton &skel) {
		centerOfMass = skel.centerOfMass;

		for(int i = 0; i < 15; ++i) {
			joints[i] = skel.joints[i];
		}
	}

	/// Build a skeleton from a message
	Skeleton(const messages::Skeleton &message) {
		centerOfMass = maths::fromMessage(message.centerofmass());

		if (message.joints_size() != 15) {
			LOG_ERROR("Malformed Skeleton message");
			throw std::runtime_error("Malformed Skeleton message");
		}

		for(int i = 0; i < 15; ++i) {
			joints[i] = message.joints(i);
		}
	}


	// MARK: - Operators

	/// Convenient access to the specified joint
	pb::Joint & operator [](JointID &jointID) {
		return joints[jointID];
	}

	/// Replace the value of the specified joint with he given one
	void set(const JointID &jointID, const pb::Joint &joint) {
		joints[jointID] = joint;
	}

	operator messages::Skeleton * () const {
		messages::Skeleton * message = new messages::Skeleton();

		// Converts the joints
		for(const pb::Joint &joint: joints) {
			messages::Joint * jointMessage = message->mutable_joints()->Add();
			jointMessage->CopyFrom(joint);
		}

		// Fill in the message
		message->set_allocated_centerofmass(maths::asMessage(centerOfMass));

		return message;
	}

	// MARK: Mathematics operator

	Skeleton& operator += (const Skeleton &s2) {
		// Sum all joints
		for(long unsigned int i = 0; i < joints.size(); ++i) {
			joints[i] += s2.joints[i];
		}

		// Sum the center of mass
		centerOfMass += s2.centerOfMass;

		return *this;
	}

	Skeleton operator - (const Skeleton &s2) {
		Skeleton s;
		// Substract all joints
		for(long unsigned int i = 0; i < joints.size(); ++i) {
			s.joints[i] = joints[i] - s2.joints[i];
		}

		// Substract the center of mass
		s.centerOfMass = centerOfMass - s2.centerOfMass;

		return s;
	}

	Skeleton operator / (const SCALAR &div) {
		Skeleton s;

		// Divide each joints
		for(long unsigned int i = 0; i < joints.size(); ++i) {
			s.joints[i] = joints[i] / div;
		}

		// Divide the center of mass
		s.centerOfMass = centerOfMass / div;

		return s;
	}
};

} /* ::pb */

#endif /* Skeleton_hpp */
