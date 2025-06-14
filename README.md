![RP2040-zero-TestTool](/images/trs-80MotherboardKeyBoard3.jpg?rawtrue "Header")

# RP2040-zero-TestTool
This is used as a test harness for a RP2040-zero and connecting it to other I/O.
I needed a small tool to enable me to turn on or read any GPIO port.
I saw a cool version with a web interface that does more for a Raspberry Pi Pico W or any with wifi.  (https://github.com/sysprogs/PicoHTTPServer/)
This is an small attempt to give me a lot of those features with terminal output instead of a web interface,
So, it lacks the polish but is for me helpful testing small projects.

The C code is provided (RP2040zeroTestTool.c) that can be compiled or edited as needed.  I've also put a compiled version, the RP2040zerTestTool.uf2.
That file can be placed on the RP by pressing the boot and reset button to open a windows file manager window and then dragging the ‘.uf2’ file to that mounted drive.
When it’s placed on the RP, it will boot to the program, and you can view it via a terminal program like putty connecting it to its com port.

The terminl window should display a representation of the RP2040-Zero and the GPIO ports.  There is a menu at the top 
<ON:All High> 		- ON sets all the pins to 1
<CL:All Low>  		- CL sets all the pins to 0
<ED:edit labels>	- ED brings up the edit label screen which allows label configuration for all pins labels 
<digit GP number>	- Enter a 2-digit GPIO port number and that pin will toggle between On and off (0/1)
  Green = High		 - The name of the GPIO will turn green when it has been turned to a 1 or if the GPIO is held HIGH by a power source
  White = Low		- The name of the GPIO will turn white when the port is LOW, a 0

The LED on the board is GP16 and can be toggled on or off by entering 16 at Input:
Just type a GPIO number or one of the menu entries and it will auto enter when the second character is entered.

There is a bitmask string also displayed for the mask values if you are in need of it in code to set the series of bits in a single command.


<img src="https://github.com/kdcgarber/RP2040-zero-TestTool/blob/main/images/RP2040-zero.gif" width=300 height=200> 

<p>&nbsp</p>

<img src="https://github.com/kdcgarber/RP2040-zero-TestTool/blob/main/images/RP2040zeroTestTool.gif" width=7200 height=600>

Label edit screen

<img src="https://github.com/kdcgarber/RP2040-zero-TestTool/blob/main/images/RP2040zeroLabels.gif" width=7200 height=400>


This is just a test breadboard I used to validate the software to ensure I/O works in both directions and that the tool labels are configurable  correctly.

<img src="https://github.com/kdcgarber/RP2040-zero-TestTool/blob/main/images/breadboardtesting.jpg" width=350 height=400>



Pinout for the RP2040-zero

<img src="https://github.com/kdcgarber/RP2040-zero-TestTool/blob/main/images/RP2040zero-pinout.gif" width=750 height=700>

<pre>
</pre>


## License

This project is licensed under the [MIT License](LICENSE.md) - Copyright 2025 Todd Garber<br><br>
