lib = display

mqtt: mqtt.c
	gcc mqtt.c -o mqtt.run -lpaho-mqtt3c -lwiringPi -pthread -lm -lrt -lcrypt $(lib).o

$(lib).o: $(lib).s
	as -g -o $(lib).o $(lib).s
