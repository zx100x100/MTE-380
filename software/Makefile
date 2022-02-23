LIST = command position# sensor_data
all:
	for i in $(LIST); do \
		cd proto; python3 $${NANOPB_GENERATOR_PATH:-~/programs/nanopb/generator/nanopb_generator.py} $$i.proto; \
		cd ..; \
		mv -v proto/$$i.pb.h ./mte380_main/; \
		mv -v proto/$$i.pb.c ./mte380_main/; \
	done
	protoc --python_out=./dashboard ./proto/*
