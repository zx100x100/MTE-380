all:
	cd proto; python3 ~/programs/nanopb/generator/nanopb_generator.py command.proto
	cd proto; python3 ~/programs/nanopb/generator/nanopb_generator.py position.proto
	mv -v proto/command.pb* ./mte380_main/
	mv -v proto/position.pb* ./mte380_main/
	protoc --python_out=./dashboard ./proto/*
