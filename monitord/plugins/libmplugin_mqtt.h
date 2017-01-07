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
#include <mosquittopp.h>



class myMQTT : public mosqpp::mosquittopp
{
public:
	explicit myMQTT();
	~myMQTT();

	void send(const std::string &topic, const std::string &message);

	void on_connect(int rc);
	void on_disconnect(int rc);
	void on_message(const struct mosquitto_message *message);
	void on_subcribe(int mid, int qos_count, const int *granted_qos);
};



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


private:
	// functions
	void check(const int returnValue, const std::string &errorMessage, const bool throwException = true);

	// variables
	bool bConnected;
	myMQTT *pMQTT;
	std::string strTopic;
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
