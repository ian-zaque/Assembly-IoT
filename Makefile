lib = display

mqtt: mqtt.c
	gcc mqtt.c -o mqtt.run -lpaho-mqtt3c $(lib).o

$(lib).o: $(lib).s
	as -g -o $(lib).o $(lib).s
