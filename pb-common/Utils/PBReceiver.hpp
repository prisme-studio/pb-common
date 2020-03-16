//
//  PBReceiver.hpp
//  Talkers
//
//  Created by Valentin Dufois on 2019-12-03.
//

#ifndef PBReceiver_hpp
#define PBReceiver_hpp

#include <iostream>
#include <vector>
#include <mutex>

#include <common/thread.hpp>
#include <network/Socket/Socket.hpp>
#include <network/Socket/SocketDelegate.hpp>
#include <network/Discovery/Browser.hpp>

#include <pb-common/messages.hpp>

#include "Arena.hpp"
#include "../Structs/Body.hpp"

using namespace network;

namespace pb {

class PBReceiver;

class PBReceiverObserver {
public:
	virtual void receiverDidConnect(PBReceiver *) = 0;
	virtual void receiverDidUpdate(PBReceiver *) = 0;
	virtual void receiverDidClose(PBReceiver *) = 0;
};

class PBReceiver: public SocketDelegate {
public:

	PBReceiver() {
		_socket.delegate = this;

		_browser.onReceive = [&] (const Endpoint &remote) {
			if(_socket.getStatus() != SocketStatus::ready)
				_socket.connectTo(remote.ip, serverPortReceiver);
		};
	}

	~PBReceiver() {
		delete _internalThread;
	}

	// MARK: - Observers

	inline void addObserver(PBReceiverObserver * observer) {
		_observers.push_back(observer);
	}

	inline void removeObserver(PBReceiverObserver * observer) {
		std::remove(_observers.begin(), _observers.end(), observer);
	}

	// MARK: - Lifecycle

	inline void open(const std::string &masterIP = "") {
		_internalThread = new std::thread(&PBReceiver::parseBodies, this);

		if(masterIP.size() == 0) {
			// Browse for a pb-master
			_browser.startBrowsing(discoveryPortReceiver);
			return;
		}

		// Connect to the specified master
		_socket.connectTo(masterIP, serverPortReceiver);
	}

	void close() {
		_browser.stopBrowsing();
		_socket.close();
	}

	// MARK: - Accessors

	inline bool isConnected() const {
		return _socket.getStatus() == SocketStatus::ready;
	}

	inline Arena * arena() {
		return &_arena;
	}

	// MARK: - Socket Delegate

	inline virtual void socketDidOpen(BaseSocket *) override {
		_browser.stopBrowsing();

		for (PBReceiverObserver * observer: _observers) {
			if(observer != nullptr)
				observer->receiverDidConnect(this);
		}
	}

	inline virtual void socketDidReceive(BaseSocket *, const protobuf::Message * aMessage) override {
		const network::messages::Datagram * datagram = dynamic_cast<const network::messages::Datagram *>(aMessage);

		if(datagram->type() != DatagramType::partialBody) {
			// Discard
			delete datagram;
			return;
		}

		// Unpack the datagram
		messages::PartialBody partialBody = messages::PartialBody();
		datagram->data().UnpackTo(&partialBody);
		const pb::bodyUID bodyUID = partialBody.uid();

		delete datagram;
		_arenaMutex.lock();

		// Do we have already a body for the current UID ?
		if(_bodies.find(bodyUID) != _bodies.end()) {
			// We already have this body.
			// Check if body is valid
			if(!partialBody.isvalid()) {
				// Erase body
				_bodies.erase(bodyUID);

				_arenaMutex.unlock();

				for (PBReceiverObserver * observer: _observers) {
					if(observer != nullptr)
						observer->receiverDidUpdate(this);
				}
				return;
			}

			// Body is valid, merge the partial body in the body
			_bodies[bodyUID]->insertPartial(partialBody);

			_arenaMutex.unlock();

			for (PBReceiverObserver * observer: _observers) {
				if(observer != nullptr)
					observer->receiverDidUpdate(this);
			}
			return;
		}

		// This looks like a new body, is it valid ?
		if(!partialBody.isvalid()) {
			// No, ignore
			delete datagram;

			_arenaMutex.unlock();

			for (PBReceiverObserver * observer: _observers) {
				if(observer != nullptr)
					observer->receiverDidUpdate(this);
			}
			return;
		}

		// Body is good, insert it
		_bodies[bodyUID] = new Body(partialBody);

		_arenaMutex.unlock();

		for (PBReceiverObserver * observer: _observers) {
			if(observer != nullptr)
				observer->receiverDidUpdate(this);
		}
	}

	inline virtual void socketDidClose(BaseSocket *) override {
		_browser.startBrowsing(discoveryPortReceiver);

		for (PBReceiverObserver * observer: _observers) {
			if(observer != nullptr)
				observer->receiverDidClose(this);
		}
	}

	// MARK: - Properties
private:

	// MARK: Observers

	std::vector<PBReceiverObserver *> _observers;

	// MARK: Network

	Browser _browser;

	Socket<network::messages::Datagram> _socket;

	// MARK: Storage

	std::mutex _arenaMutex;

	std::map<bodyUID, Body *> _bodies;

	Arena _arena = Arena(&_bodies, &_arenaMutex);

	std::thread * _internalThread = nullptr;

	void parseBodies() {
		thread::setName("pb.receiver.internal");

		while(true) {
			std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

			_arenaMutex.lock();

			// Check for outdated bodies
			for(auto it = _bodies.begin(); it != _bodies.end();) {
				it->second->inactivityCount += 1;

				if(it->second->inactivityCount > 30)
					it = _bodies.erase(it);
				else
					++it;
			}

			_arenaMutex.unlock();

			thread::cadence(std::chrono::system_clock::now() - start, 15.0);
		}
	}
};

} /* ::pb */

#endif /* PBReceiver_hpp */
