# monitord

[![Linux build status](https://travis-ci.org/pgollor/monitord.svg)](https://travis-ci.org/pgollor/monitord)

monitord ist ein Funkauswerter fuer die Protokolle ZVEI, Pocsag und FMS.
Das eigentliche monitord-Projekt liegt auf www.monitord.de.
Da die Entwicklung allerdings nicht mehr sonderlich aktiv ist, habe ich hier bei GitHub das Repository geklont.

## Infos
Dies Repo ist nur ein Klon vom SVN-Repo. Sollten dort Änderungen auftauchen, werde ich sie hier zurückspielen.
Ich bin weder Leiter des Projekts noch Ansprechpartner für irgendwelche Protokollfragen :-)

## Kompilierung
### Cent OS 6.2

	yum install autoconf automake libtool pkg-config alsa-lib-devel lua-devel
	aclocal -Im4
	./autogen.sh
	./configure && make && make install

## Features gegenüber der SVN-Basis
### Integration log4cxx
Die monitord-Version aus dem Subversion-Repository beinhaltet einen einfachen Logging-Mechanismus, der i.a.R. auch ausreicht.
Will man allerdings File-Rolling haben, braucht man einen anderen Logging-Provider.
Aus diesem Grund habe ich log4cxx integriert.

#### Kompilierung
log4cxx muss selbst gebaut werden. Die aktuelle Version 0.10.0 enthält zwei Fehler, die mit einem Patch gefixt werden können.

	# nötige Libs
	yum install apr apr-util apr-devel apr-util-devel
	# unter Ubuntu:
	# apt-get install libaprutil1-dev
	# log4cxx von einer passenden Location herunterladen
	wget $apache-log4cxx-0.10.0.tar.gz
	tar -xvf apache-log4cxx-0.10.0.tar.gz
	# Patch von Markus Mazurczak einspielen
	# siehe http://markus-mazurczak.de/?p=76
	./configure && make && make install

Da log4cxx standardmäßig keine pkg-config-Datei erzeugt, muss der Pfad zu den Include-Dateien bei der Kompilierung von monitord manuell mit angegeben werden:

	./configure --with-log4cxx --with-log4cxx-includes=/pfad-zu-log4cxx-includes

#### Konfiguration
In der monitord.xml gibt es nur zwei Sachen zu konfigurieren:

	<monitordconfig>
		<logfile>log4cxx</logfile>
		<log4cxxConfig>/pfad/zu/log4cxx.properties</log4cxxConfig>
		...
	</monitordconfig>

Sobald logfile auf 'log4cxx' gestellt ist, wird log4cxx auch benutzt. log4cxxConfig kann zu einer log4cxx.properties oder log4cxx.xml verweisen. Wird keine definiert, werden die Default-log4cxx-Einstellungen benutzt.

### Integration ActiveMQ
ActiveMQ ist eine Messaging Queue, die nach dem Pub/Sub-Verfahren arbeitet.
Das Plugin libmplugin_activemq pusht automatisch alle eingehenden Nachrichten (ZVEI, Pocsag, FMS) in die hinterlegte Queue.
Andere Clients können darauf zugreifen und die Nachrichten dementsprechend verarbeiten.
Entweder muss das Paket activemq-cpp selbst kompiliert werden oder aber man zieht es sich aus einem RPM-/Deb-Repository.

#### Kompilierung mit activemq-cpp über Paketmanager

	yum install activemq-cpp

Danach muss monitord mit den Parametern

	./configure --enable-plugins --with-activemq

kompiliert werden.


#### Kompilierung mit activemq-cpp als Quellcodedistribution unter CentOS 6.7

	yum install gcc-c++ openssl-devel apr-devel

	wget ...activemq-cpp-library-3.8.4.zip
	unzip activemq-cpp-library-3.8.4.zip
	cd activemq-cpp-library-3.8.4
	./configure --disable-ssl
	make && make install

	cd monitord

Danach muss monitord mit den Parametern

	CPPFLAGS=`activemqcpp-config --includes` ./configure --enable-plugins --with-activemq

monitord kompiliert werden.
	
#### Konfiguration
Das Plugin wird in der monitord.xml folgendermaßen konfiguriert:

	<dataplugins>
	<plugin name="activemq">
		<file>plugins/libmplugin_activemq.so</file>
		<parameters>
			<logfile>activemq.log</logfile>
			<loglevel>DEBUG</loglevel>
			<brokerUri>tcp://127.0.0.1:61616</brokerUri>
			<username>your_username_or_empty</username>
			<password>your_password_or_empty</password>
			<clientId>your_clientid_or_empty</clientId>
			<sendTimeout>5</sendTimeout>
			<closeTimeout>5</closeTimeout>
			<producerWindowSize>5</producerWindowSize>
			<useCompression>1</useCompression>
			<clientAck>0</clientAck>

			<!-- generic configuration -->
			<useTopic>1</useTopic>
			<deliveryModePersistent>0</deliveryModePersistent>
			<destUri>zabos</destUri>

			<!-- overwrite generic configuration for FMS -->
			<topic type="fms">
				<destUri>zabos.fms</destUri>
			</topic>

			<!-- overwrite gneric configuration for POCSAG -->
			<topic type="pocsag">
				<useTopic>0</useTopic>
				<!-- destUri is still zabos, imported by generic configuration -->
			</topic>
			<!-- ZVEI is not defined, so generic configuration is used -->
		</parameters>
	</plugin>
	</dataplugins> 

Bitte beachten: Der XML-Parser von monitord unterstützt *keine* leeren Tags (also <tag/> oder <tag></tag>). Sollte die Konfiguration dennoch einen solchen enthalten, gibt es einen Segmentation Fault.

##### IPv6
activemq-cpp unterstützt momentan keine IPv6-Endpunkte. Der ActiveMQ-Broker muss also auf einem IPv4-Port lauschen. Damit ActiveMQ standardmäßig IPv4 anstelle von IPv6 nutzt, kann folgende Einstellung in der /etc/default/activemq gesetzt werden

	ACTIVEMQ_DEBUG_OPTS="-Djava.net.preferIPv4Stack=true"

##### Wiederherstellung der ActiveMQ-Verbindung
Je nach Einsatz kann es sein, dass die Verbindung zwischen monitord und dem ActiveMQ-Broker abbricht. Dies kann zum Beispiel auftreten, wenn es Probleme mit der TCP-Verbindung gibt oder aber der Broker zwischenzeitlich neugestartet worden ist.
Um darauf zu reagieren, kann der ActiveMQ-Client ein Failover nutzen:

	<monitordconfig version="1.0">
		<!-- ... -->
		<!-- startupMaxReconnectAttempts=1 sorgt dafür, dass beim *ersten* Start mindestens ein Server kontaktierbar ist. Dies stellt sicher, dass sich keine Konfigurationsfehler einschleichen -->
		<brokerUri>failover://(tcp://127.0.0.1:61616)?startupMaxReconnectAttempts=1</brokerUri>
		<!-- ... -->
	</monitordconfig>

bzw. wenn mehrere Broker genutzt werden sollen:

	<monitordconfig version="1.0">
		<!-- ... -->
		<!-- startupMaxReconnectAttempts=1 sorgt dafür, dass beim *ersten* Start mindestens ein Server kontaktierbar ist. Dies stellt sicher, dass sich keine Konfigurationsfehler einschleichen -->
		<brokerUri>failover://(tcp://192.168.0.1:61616,192.168.0.2:61616)?startupMaxReconnectAttempts=1</brokerUri>
		<!-- ... -->
	</monitordconfig>

## Start
Das Script *monitord-start-stop* muss nach /etc/init.d/monitord kopiert werden. Standardmäßig läuft monitord unter der Benutzer *monitord*. Dieser muss vorher erstellt worden sein.
Die Datei /etc/init.d/monitord muss angepasst werden, so dass die korrekten Pfade zur ausführbaren Datei von monitord und zur monitord.xml eingetragen worden sind.


Über
	
	/etc/init.d/monitord start

lässt sich monitord starten.
