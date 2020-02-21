
//
//  CalibrationValues.hpp
//  Protocol Bernardo
//
//  Created by Valentin Dufois on 2019-11-21.
//

#ifndef CalibrationValues_hpp
#define CalibrationValues_hpp

#include "../Utils/maths.hpp"
#include "../messages.hpp"

namespace pb {

struct CalibrationValues {
	maths::vec3 angle;
	maths::vec3 position;

	CalibrationValues() = default;

	CalibrationValues(const messages::CalibrationValues &message):
		angle(maths::fromMessage(message.angle())),
		position(maths::fromMessage(message.position())) { }

	operator messages::CalibrationValues * () {
		messages::CalibrationValues * message = new messages::CalibrationValues();
		message->set_allocated_angle(maths::asMessage(angle));
		message->set_allocated_position(maths::asMessage(position));

		return message;
	}
};

} /* ::pb */

#endif /* CalibrationValues_hpp */
