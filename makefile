CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ./cadmium/include
INCLUDEDESTIMES=-I ./DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
threat_details.o: data_structures/threat_details.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/threat_details.cpp -o build/threat_details.o

main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/main.cpp -o build/main_top.o

main_search_and_locate_test.o: test/main_search_and_locate_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_search_and_locate_test.cpp -o build/main_search_and_locate_test.o

main_hovering_test.o: test/main_hovering_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_hovering_test.cpp -o build/main_hovering_test.o

main_capture_image_location_test.o: test/main_capture_image_location_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_capture_image_location_test.cpp -o build/main_capture_image_location_test.o

tests: main_search_and_locate_test.o main_hovering_test.o main_capture_image_location_test.o threat_details.o
		$(CC) -g -o bin/SEARCH_AND_LOCATE_TEST build/main_search_and_locate_test.o build/threat_details.o
		$(CC) -g -o bin/HOVERING_TEST build/main_hovering_test.o
		$(CC) -g -o bin/CAPTURE_IMAGE_LOCATION_TEST build/main_capture_image_location_test.o build/threat_details.o

#TARGET TO COMPILE ONLY ABP SIMULATOR
simulator: main_top.o threat_details.o
	$(CC) -g -o bin/BORDER_PROTECTION_CONTROL_SYSTEM build/main_top.o build/threat_details.o

#TARGET TO COMPILE EVERYTHING (ABP SIMULATOR + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean:
	rm -f bin/* build/*