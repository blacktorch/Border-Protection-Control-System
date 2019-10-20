BORDER PROTECTION CONTROL SYSTEM (BPCS)

All the scripts needed to run and test the simulation are all present herein.
The scripts come in extension .sh for UNIX systems

#*******Compilation*********#
To compile the entire model simple run the "compile.sh" script

#*******Running and testing the Atomic models*******************#

Atomic Model ##################### Script to Run
Hovering                ###        run_hovering_test.sh
SearchAndLocateThreat   ###        run_search_and_locate.sh
CaptureImageAndLocation ###        run_capture_image.sh

#*******Running and testing the Coupled models*******************#

Coupled Model ############################# Script to Run
UAV                              ###        run_uav.sh
BorderProtectionControlSystem    ###        run_bpcs.sh

#**********************INPUT FILES*******************#
All input files are located in the input_data directory

Hovering -> hovering_input_test_in_id.txt //for threatId input
Hovering -> hovering_input_test_threat_details_sent.txt // for threat_sent acknowledgement
SearchAndLocateThreat  -> search_and_locate_input_test.txt
CaptureImageAndLocation -> capture_image_location_input_test.txt
UAV -> input_uav.txt
BorderProtectionControlSystem -> input_bpcs.txt

#**********************SIMULATION RESULTS*******************#
All simulation result files are located in the simulation_results directory

**All result files are prefixed with the name of their model
