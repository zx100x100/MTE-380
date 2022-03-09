LIST = nav_data guidance_data tof_data imu_data hms_and_cmd_data
NANOPB_GENERATOR_PATH = ~/programs/nanopb/generator/nanopb_generator.py
all:
	@if [ -f ./nanopb_generator_path ]; then \
		NANOPB_GENERATOR_PATH = $$(<./nanopb_generator_path) ; \
	fi; \
	if [ ! -f $(NANOPB_GENERATOR_PATH) ]; then \
		echo ; \
		echo Error! no file found at $(NANOPB_GENERATOR_PATH) ; \
		echo ; \
		echo FIX WITH THE FOLLOWING STEPS!! ; \
		echo ---------------------------------------------------------------; \
		echo "git clone https://github.com/nanopb/nanopb ~/nanopb" ; \
		echo "echo ~/nanopb/generator/nanopb_generator.py; > ./nanopb_generator_path" ; \
		echo ; \
		echo "(optional): replace ~/nanopb in both commands with a different install location" ; \
		echo ---------------------------------------------------------------; \
		exit ; \
	fi ; \
	for i in $(LIST); do \
		cd proto; python3 $(NANOPB_GENERATOR_PATH) -I . $$i.proto ; \
		cd .. ; \
		mv -v proto/$$i.pb.h ./mte380_main/ ; \
		mv -v proto/$$i.pb.c ./mte380_main/ ; \
	done ; \
	protoc --python_out=./dashboard ./proto/*_data.proto ;
