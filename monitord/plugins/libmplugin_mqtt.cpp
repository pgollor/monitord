/*
 * libmplugin_mqtt.cpp
 *
 *  Created on: 06.01.2017
 *      Author: kalle
 */


#include "libmplugin_mqtt.h"
#include <iostream>

#include "../MonitorLogging.h"
#include "../MonitorExceptions.h"


#ifndef DD
	#define DD(msg) std::cout << "[" << __FUNCTION__ << "]: " << msg << std::endl
#endif
#ifndef nullptr
	#define nullptr NULL
#endif



MonitorPlugInMQTT::MonitorPlugInMQTT()
{
	DD("start");
}


MonitorPlugInMQTT::~MonitorPlugInMQTT()
{
	DD("stop");
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
		pMQTT = new myMQTT();
	}

	// connect to mqtt server
	check(pMQTT->connect_async(hostname.c_str(), port), "Could not connect to MQTT server!");
	check(pMQTT->loop_start(), "Could not start loop!");

	// transmit version string
	std::string topic = strTopic + "/version";
	std::string msg = VERSION;
	pMQTT->publish(NULL, topic.c_str(), msg.length(), msg.c_str());

	// logging output
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

	// add timestamp
	msg += "\"timestamp\":" + (*pRes)["timestamp"] + ", ";

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

	LOG_DEBUG(msg);

	// transmit message
	//check(pMQTT->publish(NULL, topic.c_str(), msg.length(), msg.c_str()), "Could not publish", false);
	pMQTT->send(topic, msg);

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
	check(pMQTT->loop_stop(), "Could not stop loop", false);

	// disconnect from server
	check(pMQTT->disconnect(), "Could not disconnect", false);

	LOG_INFO("disconnecting");

	return true;
}


void MonitorPlugInMQTT::Show()
{
	DD("start");
}


void MonitorPlugInMQTT::check(const int returnValue, const std::string& errorMessage, const bool throwException)
{
	if (returnValue != MOSQ_ERR_SUCCESS)
	{
		LOG_ERROR(errorMessage);
		if (throwException)
		{
			MonitorException(errorMessage.c_str());
		}
	}
}



// +++++ myMQTT +++++ //

myMQTT::myMQTT() : mosqpp::mosquittopp(NULL, true)
{
}


myMQTT::~myMQTT()
{
}


void myMQTT::send(const std::string& topic, const std::string& message)
{
	publish(NULL, topic.c_str(), message.length(), message.c_str());
}


void myMQTT::on_connect(int rc)
{
	LOG_INFO("on_connect");
}


void myMQTT::on_disconnect(int rc)
{
	LOG_INFO("on_disconnect");
}


void myMQTT::on_message(const struct mosquitto_message *message)
{
	char payload[message->payloadlen];
	memcpy(payload, message->payload, message->payloadlen);

	std::string topic = message->topic;
	std::string msg = payload;

	LOG_DEBUG("Got message: " << topic << " - " << msg);
}


void myMQTT::on_subcribe(int mid, int qos_count, const int *granted_qos)
{
	DD("on_subcribe");
}

