/*
  ==============================================================================

	SerialDevice.cpp
	Created: 15 Mar 2017 10:15:01am
	Author:  Ben-Portable

  ==============================================================================
*/

#include "SerialDevice.h"
#include "SerialManager.h"

#if SERIALSUPPORT
SerialDevice::SerialDevice(Serial * _port, SerialDeviceInfo  * _info, PortMode _mode) :
thread(_info->port, this),
port(_port),
info(_info),
mode(_mode)
{
	DBG("NEW SERIAL DEVICE");
	open();

	
}
#else
SerialDevice::SerialDevice(SerialDeviceInfo  * _info, PortMode _mode) :
	info(_info),
	mode(_mode),
	thread(_info->port, this)
{
	open();
}
#endif

SerialDevice::~SerialDevice()
{
	//cleanup
	close();
	listeners.call(&SerialDeviceListener::portRemoved, this);
}

void SerialDevice::setMode(PortMode _mode)
{
	if (mode == _mode) return; //do nothing if the same

	if (_mode == LINES) //must restart to make sure thread is not hanging in readLine
	{
		thread.signalThreadShouldExit();
		thread.stopThread(1000);
		thread.startThread();
	}

	mode = _mode;
}

void SerialDevice::setBaudRate(int baudRate)
{
	if (port != nullptr)
	{
		DBG("Port is null here, not setting baudrate");
		if ((uint32_t)baudRate == port->getBaudrate()) return;

		port->setBaudrate(baudRate);
	}
}

void SerialDevice::open(int baud)
{
#if SERIALSUPPORT
	if (port == nullptr) return;

	
	port->setBaudrate(baud); 
	if (!port->isOpen())  port->open();

	
	port->setDTR();
	port->setRTS();
	

	if (!thread.isThreadRunning())
	{
		thread.startThread();
#ifdef SYNCHRONOUS_SERIAL_LISTENERS
		thread.addSerialListener(this);
#else
		thread.addAsyncSerialListener(this);
#endif
		listeners.call(&SerialDeviceListener::portOpened, this);
	}
#endif
}

void SerialDevice::close()
{
#if SERIALSUPPORT
	if (port->isOpen())
	{
#ifdef SYNCHRONOUS_SERIAL_LISTENERS
		thread.removeSerialListener(this);
#else
		thread.removeAsyncSerialListener(this);
#endif

		thread.signalThreadShouldExit();
		while (thread.isThreadRunning());

		port->close();
		listeners.call(&SerialDeviceListener::portClosed, this);

	}
#endif
}

bool SerialDevice::isOpen() {
#if SERIALSUPPORT
	if (port == nullptr) return false;
	return port->isOpen();
#else
	return false;
#endif
}

int SerialDevice::writeString(String message, bool endLine)
{
#if SERIALSUPPORT
	if (!port->isOpen()) return 0;

	//DBG("Write string : " << message << " -- endline ? " << String(endLine));
	String m = message;
	if (endLine) m += "\n";
	try
	{
		return (int)port->write(m.toStdString());
	}
	catch (std::exception e)
	{
		LOGWARNING("Error writing to serial : " << e.what());
		return 0;
	}
	
#else
	return 0;
#endif
}

int SerialDevice::writeBytes(Array<uint8_t> data)
{
#if SERIALSUPPORT
	return (int)port->write(data.getRawDataPointer(), data.size());
#else
	return 0;
#endif
}

void SerialDevice::dataReceived(const var & data) {
	listeners.call(&SerialDeviceListener::serialDataReceived, data);
}

void SerialDevice::addSerialDeviceListener(SerialDeviceListener * newListener) { listeners.add(newListener); }

void SerialDevice::removeSerialDeviceListener(SerialDeviceListener * listener) {
	listeners.remove(listener);
	if (listeners.size() == 0) SerialManager::getInstance()->removePort(this);
}

SerialReadThread::SerialReadThread(String name, SerialDevice * _port) :
	Thread(name + "_thread"),
	port(_port)
{
}

SerialReadThread::~SerialReadThread()
{
	signalThreadShouldExit();
	while (isThreadRunning());
}

void SerialReadThread::run()
{
#if SERIALSUPPORT
	
	std::vector<uint8_t> byteBuffer; //for cobs and data255


	while (!threadShouldExit())
	{
		sleep(10); //100fps

		if (port == nullptr) return;
		if (!port->isOpen()) return;

		try
		{
			size_t numBytes = (int)port->port->available();
			if (numBytes == 0) continue;

			switch (port->mode)
			{

			case SerialDevice::PortMode::LINES:
			{

				std::string line = port->port->readline();
				if (line.size() > 0)
				{
					serialThreadListeners.call(&SerialThreadListener::dataReceived, var(line));
				}

			}
			break;

			case SerialDevice::PortMode::RAW:
			{
				std::vector<uint8_t> data;
				port->port->read(data,numBytes);
				//for (int i = 0; i < data.size(); i++) DBG("Data " << data[i]);
				serialThreadListeners.call(&SerialThreadListener::dataReceived, var(data.data(),numBytes));
			}
			break;

			case SerialDevice::PortMode::DATA255:
			{
				while (port->port->available())
				{
					uint8_t b = port->port->read(1)[0];
					if (b == 255)
					{
						serialThreadListeners.call(&SerialThreadListener::dataReceived, var(byteBuffer.data(),byteBuffer.size()));
						byteBuffer.clear();
					}
					else
					{
						byteBuffer.push_back(b);
					}
				}
			}
			break;

			case SerialDevice::PortMode::COBS:
			{
				while (port->port->available())
				{
					uint8_t b = port->port->read(1)[0];
					byteBuffer.push_back(b);
					if (b == 0)
					{
						uint8_t decodedData[255];
						size_t numDecoded = cobs_decode(byteBuffer.data(), byteBuffer.size(), decodedData); 
						serialThreadListeners.call(&SerialThreadListener::dataReceived, var(decodedData, numDecoded));
						byteBuffer.clear();
					}
				}				
			}
			break;
			}
		}
		catch (...)
		{
			DBG("### Serial Problem ");
		}

		
	}

	DBG("END SERIAL THREAD");
#endif

}

SerialDeviceInfo::SerialDeviceInfo(String _port, String _description, String _hardwareID) :
	port(_port), description(_description), hardwareID(_hardwareID)
{
#if JUCE_WINDOWS
	vid = hardwareID.substring(8, 12).getHexValue32();
	pid = hardwareID.substring(17, 21).getHexValue32();
	deviceID = description;
	uniqueDescription = description; //COM port integrated in description
#else
	vid = hardwareID.substring(16, 20).getHexValue32();
	pid = hardwareID.substring(23, 27).getHexValue32();
	deviceID = hardwareID;
	uniqueDescription = description + "(SN : " + deviceID.substring(35) + ")";
#endif
}
