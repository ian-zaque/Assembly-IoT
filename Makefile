mqtt: mqtt.c
	gcc mqtt.c -o mqtt.run -lpaho-mqtt3c -lwiringPi -lwiringPiDev -lm -lrt -lcrypt
