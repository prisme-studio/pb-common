//
//  messages.hpp
//  pb-common
//
//  Created by Valentin Dufois on 2019-09-20.
//

#ifndef messages_h
#define messages_h

#include <network/Messages/network.pb.h>

#include "Messages/body.pb.h"
#include "Messages/layout.pb.h"
#include "Messages/maths.pb.h"

#ifdef SERVO
#include "Messages/talkers.pb.h"
#endif

/// Convenient namespace alias
namespace protobuf = google::protobuf;

namespace pb {

using Datagram = ::network::messages::Datagram;

enum DatagramType: unsigned int {
	// Network lib datagram types here for expliciteness
	undefined 			= 0,
	ping 				= 5,
	pong 				= 6,
	close 				= 9,

	status 				= 50,

	// 1XX: Tracker types -
	rawBody 			= 110,	// A Body sent through the stream

	// 2XX: Master types -
	// Layout
	layoutFile			= 205,  // Holds a layout
	layoutList	 		= 210,  // Asks/Hold the list of layouts
	layoutCreate 		= 211,  // Create a new layout
	layoutOpen			= 212,  // Open a layout on the master
	layoutRename 		= 213,	// Rename a layout
	layoutUpdate 		= 214,  // Update a layout
	layoutClose 		= 215,  // Close the opened layout
	layoutDelete 		= 216,  // Delete the specified layout

	calibrationSet		= 220,	// Set the calibration devices
	calibationValues    = 221,	// Calibration values for the current set

	trackedBodies		= 250,	// List of all tracked bodies
	partialBody			= 255, 	// A Partial body
};

/// Convenient datagram builder
/// @param type The type of the datagram to build
/// @param data The data to put in the datagram
inline Datagram * makeDatagram(const DatagramType &type, const protobuf::Message &data) {
	protobuf::Any * anyMessage = new protobuf::Any();
	anyMessage->PackFrom(data);

	Datagram * datagram = new Datagram();
	datagram->set_type(type);
	datagram->set_allocated_data(anyMessage);

	return datagram;
}

/// Convenient datagram builder
inline Datagram * makeDatagram(const DatagramType &type) {
	Datagram * datagram = new Datagram();
	datagram->set_type(type);

	return datagram;
}

} /* pb */

#endif /* messages_h */
