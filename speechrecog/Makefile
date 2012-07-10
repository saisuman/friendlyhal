CC=g++
export LD_LIBRARY_PATH=/usr/local/lib

all: listener

listener: listener_main.cc
	${CC} -o listener listener_main.cc -DMODELDIR=\"`pkg-config --variable=modeldir pocketsphinx`\" `pkg-config --cflags --libs pocketsphinx sphinxbase`

models:
	text2wfreq < commands_reference.txt | wfreq2vocab > commands.vocab
	text2idngram -vocab commands.vocab -idngram commands.idngram < commands_reference.txt
	idngram2lm -vocab_type 1 -idngram commands.idngram -vocab commands.vocab  -arpa commands.arpa
	sphinx_lm_convert -i commands.arpa -o commands.DMP

clean:
	rm -f commands.vocab commands.idngram commands.arpa commands.DMP *.o listener
