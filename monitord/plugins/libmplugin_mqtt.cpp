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



MonitorPlugInMQTT::MonitorPlugInMQTT()
{
	DD("start");
}


MonitorPlugInMQTT::~MonitorPlugInMQTT()
{
	DD("start");
}


bool MonitorPlugInMQTT::initProcessing(class MonitorConfiguration* configPtr,XMLNode config)
{
	DD("start");
	return true;
}


bool MonitorPlugInMQTT::processResult(class ModuleResultBase *pRes)
{
	DD("start");
	return true;
}


bool MonitorPlugInMQTT::quitProcessing()
{
	DD("start");
	return true;
}


void MonitorPlugInMQTT::Show()
{
	DD("start");
}

