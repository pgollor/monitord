/*
 * libmplugin_mqtt.h
 *
 *  Created on: 06.01.2017
 *      Author: kalle
 */

#ifndef MONITORD_PLUGINS_LIBMPLUGIN_MQTT_H_
#define MONITORD_PLUGINS_LIBMPLUGIN_MQTT_H_


#include "mplugin.h"
#include "../MonitorLogging.h"



class MonitorPlugInMQTT : public MonitorPlugIn
{
 public:
	/**
	 * @brief pLugin constructor
	 */
	MonitorPlugInMQTT();

	/**
	 * @brief plugin destructor
	 */
	virtual ~MonitorPlugInMQTT();

	// virtual functions
	virtual bool initProcessing(class MonitorConfiguration* configPtr,XMLNode config);
	virtual bool processResult(class ModuleResultBase *pRes);
	virtual bool quitProcessing();
	virtual void Show();


	// variables
	bool bConnected = false;
};


class MonitorPlugInMQTTFactory : public MonitorPlugInFactory
{
 public:
	MonitorPlugInMQTTFactory()
	{
	}

	~MonitorPlugInMQTTFactory()
	{
	}

	virtual MonitorPlugIn * CreatePlugIn()
	{
		return new MonitorPlugInMQTT() ;
	}
};



DLL_EXPORT void * factory0( void )
{
	return new MonitorPlugInMQTTFactory;
}


#endif /* MONITORD_PLUGINS_LIBMPLUGIN_MQTT_H_ */
