temperature_sensor.o: temperature_sensor.cc
	avr-g++ temperature_sensor.cc \
		-I /home/saisuman/arduino/hardware/arduino/cores/arduino \
		-I /home/saisuman/arduino/hardware/arduino/variants/standard \
		-c -o temperature_sensor.o

arduino_support: /home/saisuman/arduino/hardware/arduino/cores/arduino/*.c*
	avr-g++ /home/saisuman/arduino/hardware/arduino/cores/arduino/*.c* \
		-I /home/saisuman/arduino/hardware/arduino/cores/arduino \
		-I /home/saisuman/arduino/hardware/arduino/variants/standard \
		temperature_sensor.o -o arduino_support \
		-D__AVR_ATmega328P__ \
		-D__OPTIMIZE__ \
		-DF_CPU=16000000UL