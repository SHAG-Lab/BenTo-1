/*
  ==============================================================================

	BentoProp.cpp
	Created: 5 Mar 2019 11:01:56pm
	Author:  bkupe

  ==============================================================================
*/

#include "BentoProp.h"


BentoProp::BentoProp(StringRef name, StringRef family, var params) :
	Prop(name, family, params)
{
	updateRate = 50;
	remoteHost = ioCC.addStringParameter("Remote Host", "IP of the prop on the network", "192.168.0.100");

	oscSender.connect("127.0.0.1", 1024);
}

BentoProp::~BentoProp()
{
}

void BentoProp::onContainerParameterChangedInternal(Parameter * p)
{
	Prop::onContainerParameterChangedInternal(p);

	if (p == globalID)
	{
		OSCMessage m("/settings/propID");
		m.addInt32(globalID->intValue());
		oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
	}
}

void BentoProp::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	Prop::onControllableFeedbackUpdateInternal(cc, c);
	if (c == playBakeFile)
	{
		if (playBakeFile->boolValue())
		{
			OSCMessage m("/strip/play");
			m.addString(bakeFileName->stringValue());
			oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
		}
		else
		{
			OSCMessage m("/strip/mode");
			m.addInt32(0);
			oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
		}
		
	}
}

void BentoProp::sendColorsToPropInternal()
{
	const int numLeds = resolution->intValue();
	const int maxLedsPerPacket = 320;

	Array<uint8> data;

	int numPackets = 0;
	for (int i = 0; i < numLeds; i++)
	{
		data.add(jmin<int>(colors[i].getRed(), 254));
		data.add(jmin<int>(colors[i].getGreen(), 254));
		data.add(jmin<int>(colors[i].getBlue(), 254));

		if (i%maxLedsPerPacket == 0) numPackets++;
	}

	data.add(255);

	int dataSize = numLeds * 3 + 1;

	for (int i = 0; i < numPackets; i++)
	{
		int offset = i * maxLedsPerPacket * 3;
		int length = i < numPackets - 1 ? maxLedsPerPacket * 3 : dataSize - offset;
		sender.write(remoteHost->stringValue(), remotePort, data.getRawDataPointer() + offset, length);
		sleep(1);
	}
}

void BentoProp::uploadBakedData(BakeData data)
{
	String target = "http://" + remoteHost->stringValue() +"/upload";
	//String target = "http://benjamin.kuperberg.fr/chataigne/releases/uploadTest.php";
	
	NLOG(niceName, "Uploading " << target << " to " << data.name << " :\n > " << data.numFrames << " frames\n > " << data.data.getSize() << " bytes");


	MemoryBlock dataToSend = data.data;

	MemoryOutputStream os;

	URL url;

	if (sendCompressedFile->boolValue())
	{
		MemoryInputStream * iis = new MemoryInputStream(data.data, true);
		ZipFile::Builder builder;
		builder.addEntry(iis, 9, "data", Time::getCurrentTime());
		
		builder.writeToStream(os, nullptr);
		dataToSend = os.getMemoryBlock();
		
	
		//url = URL(target).withDataToUpload("fupload",, "application/zip");
	}
	
	url = URL(target).withDataToUpload("uploadData", data.name, dataToSend, sendCompressedFile->boolValue()?"application/zip":"text/plain");



	ScopedPointer<InputStream> stream = url.createInputStream(true, &BentoProp::uploadProgressCallback, this);// , "Content-Type: Text/plain");

	if (stream != nullptr)
	{
		String response = stream->readEntireStreamAsString();
		DBG("Got response : " << response);
		NLOG(niceName, "Upload complete");
	}
}

void BentoProp::exportBakedData(BakeData data)
{
	MemoryInputStream is(data.data, true);
	if (exportFile.existsAsFile()) exportFile.deleteFile();
	FileOutputStream fs(exportFile);
	fs.writeFromInputStream(is, is.getDataSize());
	fs.flush();

	ZipFile::Builder builder;
	builder.addFile(exportFile, 9, "data");
	File zf = exportFile.getSiblingFile(exportFile.getFileNameWithoutExtension() + "_compressed.zip");
	if (zf.existsAsFile()) zf.deleteFile();
	FileOutputStream fs2(zf);
	double progress = 0;
	builder.writeToStream(fs2, &progress);
}

bool BentoProp::uploadProgressCallback(void * context, int bytesSent, int totalBytes)
{
	BentoProp * prop = (BentoProp *)context;
	jassert(prop != nullptr);
	prop->uploadProgress->setValue(bytesSent * 1.0f / totalBytes);
	NLOG(prop->niceName, "Uploading... " << (int)(prop->uploadProgress->floatValue()*100) << "% (" << bytesSent << " / " << totalBytes << ")");
	return true;
}
