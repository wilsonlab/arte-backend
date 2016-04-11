//THIS FILE POLLS A SPECIFIED COMPUTER RUNNING OAT FOR JSON POSITION DATA
//ONCE THE MAIN CLOCK HAS STARTED (specified by running). IT SENDS A REQUEST
//IN SPECIFIED INTERVALS (interval) USING UDP AS IMPLEMENTED BY THE ZMQ
//LIBRARY. AFTER RECEIVING THE DATA, IT ADDS A FIELD FOR COUNTED TIME AS
//DETERMINED BY ARTE'S CLOCK

//TODO:
//...nothing
#include <iostream>
#include <fstream>
#include <ctime>
#include <zmq.hpp>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

//includes used for Arte
#include "timer.h"
#include "util.h"
#include "arteopt.h"
#include "neural_daq.h"
#include <time.h>

void *oate(void*)
{
	//generate date + time and create data file
	
	time_t rawtime;
	struct tm * timeinfo;
	char timeBuf1[80];
	char timeBuf2[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	strftime(timeBuf1,80,"%Y-%m-%d",timeinfo);
	strftime(timeBuf2,80,"%I-%M",timeinfo);
	std::string timeStr1(timeBuf1);
	std::string timeStr2(timeBuf2);
//	std::string path = ("../../Data/" + timeStr1);
//	const char * cpath = path.c_str();
//	boost::filesystem::create_directories(cpath);
	std::string fileName = ("../../Data/" + timeStr1 + "-" + timeStr2 + "-Position.txt");
	const char * cctime = fileName.c_str();

	std::ofstream positionData;
        positionData.open(cctime, std::ios_base::app);
	//positionData << "hello" << std::endl;
	
	//initialize sampling period, address and port of oat computer, 
	int interval = 1;
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);
	socket.connect("tcp://192.168.2.13:5555");
	zmq::message_t request(5);
	memcpy(request.data(), "Hello", 5);
	
	//Testing Timer Stuff
	extern Timer arte_timer;
	#define EVERY_N_TS_BUFFSIZE 10	
	extern uint32_t   ts_buffer[EVERY_N_TS_BUFFSIZE];
	extern int        ts_buff_cursor;

	//start sampling loop
	while(true) 
	{
		int currentTime = ts_buffer[ts_buff_cursor];
		//int currentTime = arte_timer.getTimestamp();
		//int currentTime = 10;
		if(currentTime%interval == 0)
		{
			//ping Oat for data
			socket.send(request);

			//recieve data from Oat
			zmq::message_t reply;
			socket.recv(&reply);

			//process data
			std::string replyData = std::string(static_cast<char*>(reply.data()), reply.size());
			//std::cout << replyData << std::endl;

			//into rapidjson document
			rapidjson::Document positions;
			positions.Parse(rapidjson::StringRef(replyData.c_str(),reply.size()));
			positions.AddMember("time", currentTime, positions.GetAllocator());	//add new field with arte counted time
			//convert back to string
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			positions.Accept(writer);

			//std::cout << buffer.GetString() << std::endl;

			//write to file

			positionData << buffer.GetString() << '\n';
		}
	}
	return 0;
}


