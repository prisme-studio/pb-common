//
//  common.hpp
//  pb-common
//
//  Created by Valentin Dufois on 2020-02-20.
//  Copyright © 2020 Prisme. All rights reserved.
//

#ifndef common_h
#define common_h

#include <string>

// MARK: - Type alias

using uint = unsigned int;
using NetworkPort = unsigned short int;

namespace pb {
using deviceUID = std::string;
using rawBodyUID = short int;
using bodyUID = std::string;

// MARK: - Discovery ports
constexpr NetworkPort discoveryPortTracker = 42001;
constexpr NetworkPort discoveryPortReceiver = 42002;
constexpr NetworkPort discoveryPortTerminal = 42003;
constexpr NetworkPort discoveryPortTrackerJSON = 42004;

// MARK: Server ports
constexpr NetworkPort serverPortTracker = 42010;
constexpr NetworkPort serverPortReceiver = 42020;
constexpr NetworkPort serverPortTerminal = 42030;
constexpr NetworkPort serverPortTrackerJSON = 42040;

// MARK: PB-related-but-not-used-directly ports
constexpr NetworkPort serverPortTalker = 42100;
constexpr NetworkPort discoveryPortTalker = 42101;

// MARK: - Advertiser
constexpr unsigned short int advertiserRate = 1; // Advertise every X seconds

}

// MARK: - Engines settings
#define TRACKING_ENGINE_RUN_SPEED 15.0 // Times per seconds
#define TRACKING_ENGINE_MERGE_DISTANCE 150.0 // mm
#define TRACKING_ENGINE_BODY_HISTORY_SIZE 2

#define POSE_ENGINE_RUN_SPEED 30.0

#endif /* common_h */
