/*
  ==============================================================================

    IntanThread.cpp
    Created: 9 Jun 2011 1:34:16pm
    Author:  jsiegle

  ==============================================================================
*/

#include "IntanThread.h"

IntanThread::IntanThread() : DataThread(),
			vendorID(0x0403),
			productID(0x6010),
			baudrate(115200),
			startCode(83),
			stopCode(115),
			ch(-1),
			isTransmitting(false)

{
	ftdi_init(&ftdic);
   	ftdi_set_interface(&ftdic, INTERFACE_ANY);
	ftdi_usb_open(&ftdic, vendorID, productID);
	ftdi_set_baudrate(&ftdic, baudrate);

	std::cout << "FTDI interface initialized." << std::endl;

	ftdi_write_data(&ftdic, &startCode, 1);

	dataBuffer = new DataBuffer(16,4096);
	//dataBuffer = new DataBuffer(16, 4096);

	startThread();

	isTransmitting = true;
}

IntanThread::~IntanThread() {

	stopThread(500);

	ftdi_write_data(&ftdic, &stopCode, 1);
	unsigned char buf[4097]; // has to be bigger than the on-chip buffer
	ftdi_read_data(&ftdic, buf, sizeof(buf));

	ftdi_usb_close(&ftdic);
    ftdi_deinit(&ftdic);
	std::cout << "FTDI interface destroyed." << std::endl;

	delete dataBuffer;
	dataBuffer = 0;
}


void IntanThread::updateBuffer()
{

	// Step 1: update buffer
	ftdi_read_data(&ftdic, buffer, sizeof(buffer));


	// Step 2: sort data
	int TTLval, channelVal;

    for (int index = 0; index < sizeof(buffer); index += 3) { 
           
          ++ch;
           
         for (int n = 0; n < 1; n++) { // 

         thisSample[ch%16+n*16] = float((buffer[index] & 127) + 
                     ((buffer[index+1] & 127) << 7) + 
                     ((buffer[index+2] & 3) << 14) - 32768)/32768;

         }
  

         TTLval = (buffer[index+2] & 4) >> 2; // extract TTL value (bit 3)
         channelVal = buffer[index+2] & 60;   // extract channel value

         if (channelVal == 60) {
         	dataBuffer->addToBuffer(thisSample,1);
         	ch = -1;
         }

    }
}

