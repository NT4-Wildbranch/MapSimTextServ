# MapSimTextServ
MapSimTextServ is a file-driven environmental simulation server that facilitates real-time control and monitoring of simulated environments. Designed with modularity and extensibility in mind, this system is ideal for standalone use cases requiring dynamic interaction with a simulated world.

Setting up and running an experiment with GaiaOS and viewing the results:

Steps:

1. Clone the repository to your local machine.

2. Navigate to the repository.

3. Run builderman.bat [Windows] or builderman.sh [Linux/Mac]

4. Go to the "bin" folder and run "MapSimTextServ.exe"

5. From there use the "./MapSimTextServ/Scripts/update.txt", "./MapSimTextServ/control_panel.ssv", and "./MapSimTextServ/autoexec.ssv" to control the server. Update runs every tick of the engine. Control panel runs when the "./MapSimTextServ/Control_Panel_Flag.ssv" has '1' written to it. Autoexec runs on startup, this is the bootloader.

6. This is setup right now, though fully customizable and changeable, for building a copy of the parallel project "GaiaOS" and merging the directories. When using the provided scripts "./MapSimTextServ/Scripts/write_to_gaia.txt" & "./MapSimTextServ/Scripts/read_from_gaia.txt" to interface with the GaiaOS engine for testing.

If something doesn't work or there is a discrepancy please let me know at briarfisk@gmail.com so the issue can be investigated and the reason figured out. 