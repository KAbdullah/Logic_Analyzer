# 2-Pin Logic Analyzer and I2C Protocol Decoder

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

