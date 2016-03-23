//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "datapacket.h"

using boost::asio::ip::udp;

class ArteSpikeReceiver{
	public:
		ArteSpikeReceiver(boost::asio::io_service& io_service)
    		: socket(io_service, udp::endpoint(udp::v4(), 6303)){

    	printf("ArteSpikeReceived created\n");
		nPacket = 0;
	 	initPacketRx();
	}

private:

	int nPacket;

	udp::socket socket;
	udp::endpoint remoteEndpoint;
	boost::array<char, 512> rxBuffer;

	void initPacketRx(){

		socket.async_receive_from(
			boost::asio::buffer(rxBuffer),
			remoteEndpoint,
			boost::bind(&ArteSpikeReceiver::rxHandle,
			this,
			boost::asio::placeholders::error));
	}

	void rxHandle(const boost::system::error_code& error){

		nPacket++;
      	printf("rxHandle, packets received:%d \n", nPacket);

		if (!error || error == boost::asio::error::message_size){
	      	initPacketRx();
    	}
	}
};

int main()
{
  	try{
		boost::asio::io_service io_service;
		ArteSpikeReceiver server2(io_service);
		io_service.run();
	}
	catch (std::exception& e){
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
