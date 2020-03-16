//
//  Joint.cpp
//  pb-common
//
//  Created by Valentin Dufois on 2020-02-22.
//  Copyright © 2020 Prisme. All rights reserved.
//

#include "Joint.hpp"

namespace pb {

Joint::Joint(const messages::Joint &message):
	orientation(maths::fromMessage(message.orientation())),
	orientationConfidence(message.orientationconfidence()),
	position(maths::fromMessage(message.position())),
	position2D(maths::fromMessage(message.position2d())),
	positionConfidence(message.positionconfidence())
{
	// Check for malformed joins comming from he network
	if(abs(position.x) > 100000 ||
	   abs(position.y) > 100000 ||
	   abs(position.z) > 100000 ) {
		positionConfidence = 0;
	}
}

Joint::operator messages::Joint() const {
	messages::Joint message;

	// Fill in the message
	message.set_allocated_orientation(maths::asMessage(orientation));
	message.set_orientationconfidence(orientationConfidence);
	message.set_allocated_position(maths::asMessage(position));
	message.set_allocated_position2d(maths::asMessage(position2D));
	message.set_positionconfidence(positionConfidence);

	return message;
}

Joint Joint::forecastNext(const Joint &j1, const Joint &j2) {
	Joint j;

	// Update values
	j.orientation = j1.orientation * 2.0 - j2.orientation;
	j.position =    j1.position * 2.0 - j2.position;
	j.position2D =  j1.position2D * 2.0 - j2.position2D;

	// Update confidences. We take the lowest ones
	j.orientationConfidence = std::min(j1.orientationConfidence, j2.orientationConfidence);
	j.positionConfidence = std::min(j1.positionConfidence, j2.positionConfidence);

	return j;
}

Joint Joint::forecastNext(const Joint &j1, const Joint &j2, const Joint &j3) {
	Joint j;

	// j1 + (j1 - j2) + ((j1 - j2) - (j2 - j3)) => 3 * j1 - 3 * j2 + j3

	// Update values
	j.orientation = j1.orientation * 3.0 - j2.orientation * 3.0 + j3.orientation;
	j.position =   j1.position  * 3.0 -   j2.position * 3.0 + j3.position;
	j.position2D =  j1.position2D * 3.0 -  j2.position2D * 3.0 + j3.position2D;

	// Update confidences. We take the lowest ones
	j.orientationConfidence = std::min({j1.orientationConfidence,
		j2.orientationConfidence,
		j3.orientationConfidence});
	j.positionConfidence = std::min({j1.positionConfidence,
		j2.positionConfidence,
		j3.positionConfidence});
	return j;
}


Joint& Joint::operator += (const Joint &j2) {
	orientation += j2.orientation * j2.orientationConfidence;
	orientationConfidence += j2.orientationConfidence;

	position += j2.position * j2.positionConfidence;
	position2D += j2.position2D * j2.positionConfidence;
	positionConfidence += j2.positionConfidence;

	return *this;
}

Joint operator - (const Joint &a, const Joint &b) {
	Joint j;
	j.orientation = a.orientation - b.orientation;
	j.orientationConfidence = a.orientationConfidence - b.orientationConfidence;
	j.position = a.position - b.position;
	j.position2D = a.position2D - b.position2D;
	j.positionConfidence = a.positionConfidence - b.positionConfidence;

	return j;
}

Joint operator / (const Joint &a, const SCALAR &div) {
	Joint j;

	if(a.orientationConfidence == 0) {
		// prevent division by 0
		j.orientation = maths::vec3(0, 0, 0);
	} else {
		j.orientation = a.orientation / a.orientationConfidence;
	}

	if(a.positionConfidence == 0) {
		// prevent division by 0
		j.position = maths::vec3(0, 0, 0);
		j.position2D = maths::vec2(0, 0);
	} else {
		j.position = a.position / a.positionConfidence;
		j.position2D = a.position2D / a.positionConfidence;
	}

	j.orientationConfidence = round((a.orientationConfidence / div) * 10.0) / 10.0;
	j.positionConfidence = round((a.positionConfidence / div) * 10.0) / 10.0;

	return j;
}

}
