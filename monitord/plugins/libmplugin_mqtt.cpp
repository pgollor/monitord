/*
 * libmplugin_mqtt.cpp
 *
 *  Created on: 06.01.2017
 *      Author: kalle
 */


#include "libmplugin_mqtt.h"
#include <iostream>


#ifndef DD
	#define DD(msg) std::cout << "[" << __FUNCTION__ << "]: " << msg << std::endl
#endif
#ifndef nullptr
	#define nullptr NULL
#endif



MonitorPlugInMQTT::MonitorPlugInMQTT()
{
	DD("start");

	// init vars, TODO: use c++11 declaration in header file
	bConnected = false;
	pMQTT = nullptr;
	strTopic = "";
}


MonitorPlugInMQTT::~MonitorPlugInMQTT()
{
	DD("start");
}


bool MonitorPlugInMQTT::initProcessing(class MonitorConfiguration* configPtr, XMLNode config)
{
	std::string hostname;
	unsigned int port;
	int ret = 0;


	hostname = getNodeText(config, "hostname", "localhost");
	port = getNodeInt(config, "port", 3366);
	strTopic = getNodeText(config, "topic", "monitord");

	LOG_INFO("using mqtt host: " << hostname << " at port: " << port << " trying to connect");

	if (pMQTT == nullptr)
	{
		pMQTT = new mosqpp::mosquittopp(NULL, true);
	}

	// connect to mqtt server
	ret = pMQTT->connect_async(hostname.c_str(), port);
	ret = pMQTT->loop_start();

	if (ret != MOSQ_ERR_SUCCESS)
	{
		MonitorException("Could not connect to MQTT server!");
	}

	LOG_INFO("connected successfull");

	return true;
}


bool MonitorPlugInMQTT::processResult(class ModuleResultBase *pRes)
{
	std::string msg = "";
	std::string typ = (*pRes)["typ"];
	std::string topic = strTopic;
	LOG_DEBUG("MQTT processing result");

	if (pMQTT == nullptr)
	{
		LOG_ERROR("disconected from MQTT server");
		return false;
	}

	topic += "/";
	topic += typ;
	msg += "{";

	if (typ == "fms")
	{
		msg += "";
	}
	else if (typ == "pocsag")
	{
		msg += "";
	}
	else if (typ == "zvei")
	{
		msg += "\"zvei\":\"" + (*pRes)["zvei"] + "\"";
	}

	msg += "}";

	// transmit message
	pMQTT->publish(NULL, topic.c_str(), msg.length(), msg.c_str());


	return true;
}


bool MonitorPlugInMQTT::quitProcessing()
{
	// todo: check for failure

	if (pMQTT == nullptr)
	{
		return true;
	}

	// disable loop
	pMQTT->loop_stop();

	// disconnect from server
	pMQTT->disconnect();

	LOG_INFO("disconnecting");

	return true;
}


void MonitorPlugInMQTT::Show()
{
	DD("start");
}

