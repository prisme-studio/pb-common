//
//  Skeleton.cpp
//  pb-common
//
//  Created by Valentin Dufois on 2020-02-22.
//  Copyright © 2020 Prisme. All rights reserved.
//

#include "Skeleton.hpp"

namespace pb {

Skeleton::Skeleton(const Skeleton &s):
	time(s.time),
	centerOfMass(s.centerOfMass),
	joints(s.joints) {}

Skeleton::Skeleton(const messages::Skeleton &message):
	centerOfMass(maths::fromMessage(message.centerofmass()))
{
	if (message.joints_size() != 15) {
		throw std::runtime_error("Malformed Skeleton message");
	}

	for(int i = 0; i < 15; ++i) {
		joints[i] = message.joints(i);
	}
}

Skeleton::operator messages::Skeleton * () const {
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

Skeleton * Skeleton::forecastNext(const Skeleton * s1, const Skeleton * s2) {
	Skeleton * s = new Skeleton(); // Our new skeleton

	s->centerOfMass = s1->centerOfMass * 2.0 - s2->centerOfMass;

	for(unsigned long i = 0; i < s->joints.size(); ++i) {
		s->joints[i] = Joint::forecastNext(s1->joints[i], s2->joints[i]);
	}

	return s;
}

Skeleton * Skeleton::forecastNext(const Skeleton * s1, const Skeleton * s2, const Skeleton * s3) {
	Skeleton * s = new Skeleton(); // Our new skeleton

	s->centerOfMass = s1->centerOfMass * 3.0 - s2->centerOfMass * 3.0 + s3->centerOfMass;

	for(unsigned long i = 0; i < s->joints.size(); ++i) {
		s->joints[i] = Joint::forecastNext(s1->joints[i], s2->joints[i], s3->joints[i]);
	}

	return s;
}

Skeleton& Skeleton::operator += (const Skeleton &s2) {
	// Sum all joints
	for(long unsigned int i = 0; i < joints.size(); ++i) {
		joints[i] += s2.joints[i];
	}

	// Sum the center of mass
	centerOfMass += s2.centerOfMass;

	return *this;
}

Skeleton Skeleton::operator / (const SCALAR &div) {
	Skeleton s;

	// Divide each joints
	for(unsigned long i = 0; i < joints.size(); ++i) {
		s.joints[i] = joints[i] / div;
	}

	// Divide the center of mass
	s.centerOfMass = centerOfMass / div;

	return s;
}

Skeleton operator - (const Skeleton &a, const Skeleton &b) {
	Skeleton s;

	// Substract all joints
	for(unsigned long i = 0; i < a.joints.size(); ++i) {
		s.joints[i] = a.joints[i] - b.joints[i];
	}

	// Substract the center of mass
	s.centerOfMass = a.centerOfMass - b.centerOfMass;

	return s;
}

}
