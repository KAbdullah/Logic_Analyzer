# 2-Pin Logic Analyzer and I2C Protocol Decoder

A bare-metal 2-channel logic analyzer and I2C protocol decoder built from scratch on an STM32F446RE (Cortex-M4) with a real-time Python serial visualizer.

[Logic Analyzer working](https://youtu.be/S_OY3ORduxM)

## System Architecture

![System Architecture](assets/Logic_Analyzer_architecture.jpg)

* In this project, I showcase my knowledge for seperation of concerns so that the low level is in charge of configuring drivers and providing driver level functions, and the main loop is in change of pure logic. The adapter comes between these two levels to make sure each level is concerned only with their duties and doesn't create functions that doesn't suite their objectives.
* AI was ONLY used to help with the GUI so that the project can be completed at a much faster rate due to time constraints. The seperation of concerns also helped since the GUI only had to be built on top of the usb file(s), which allowed me to inform AI exactly what data to expect and what it means.
* In the diagram, the buffer is shown to be present in the adapter file, however it's created and managed via pointers in the uart driver's file, but since the main loop adds to the buffer, I thought it most appropriate to add it to the adapter layer to showcase it's usage by both the high and low level.


## Key Technical Highlights

* **Run-Length Encoding (RLE):** [Add how RLE compresses repeated signal states/samples and why you chose it.]
* **UART Buffer & Sizing Calculation:** [Add how the UART buffer handles data spikes and how you calculated the buffer size.]
* **Non-Blocking State Transitions:** [Add how enforcing state transitions avoids blocking/waiting steps so other tasks continue progressing.]
* **Error Handling:** [Add one specific error case and how your firmware/system detects and handles it.]

## What you need to get it working

* Work in a Linux system (I did this in Ubuntu 24.04)
* An STM32F446RE board
* 3 jumper cables: two for sniffing voltages and one for grounding it to the target board

## Setup & Usage

1. Install CMake:
   ```bash
   sudo apt install cmake
   ```

2. Clone my GitHub repo locally.

3. Run in the parent directory:
   ```bash
   cmake -S . -B build
   cmake --build build --target flash
   ```

4. In another terminal, run:
   ```bash
   python3 visualizer.py
   ```
   * From the Serial port options, select `ttyACM*` (whichever one shows up for the board running the logic analyzer code).
   * Keep the default baud rate.

5. Click **Start** and see the voltages for two pins get populated on the screen in real time.

   ---

   > **Note:** If you are working in Windows and have VS Code running on a Linux virtual machine, you need to pass the USB traffic from Windows to your virtual machine.
   >
   > I was using WSL 2, so I used the below command in administrative PowerShell:
   > ```powershell
   > usbipd attach --wsl --busid <BUSID>
   > ```
   > *(You can get this number by running `usbipd list`)*

6. The picture below demonstrates how to attached the jumper cables, where PIN 0 and PIN 1 are your voltage sniffers and the GND pin is so your logic analyzer and the target board share the same commond ground.

   ![Jumper cable wiring for PC0, PC1, and GND](assets/Logic_Analyzer_connection_pins.jpg)


## Future Improvements

* [Add planned improvement 1, e.g., support additional protocols such as SPI or UART.]
* [Add planned improvement 2, e.g., add trigger conditions or increase sample rate/buffer depth.]
* [Add planned improvement 3, e.g., refine error handling or host visualization features.]