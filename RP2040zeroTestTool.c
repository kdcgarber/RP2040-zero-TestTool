/* Description: 
  This program initializes the GPIO pins of a RP2040-Zero and allows the user to toggle any pin on and off. 
  The program uses a simple text-ba"sed menu for user interaction.
*/


#include <stdio.h>          // Needed for printf, fgets, etc.
#include <stdint.h>         // Needed for uint32_t and other fixed-width integer types
#include "pico/stdlib.h"    // Needed for GPIO functions, sleep_ms, etc.
#include "pico/stdio_usb.h" // Needed for USB input/output (e.g., stdio_usb_init)
#include <ctype.h>          // Needed for toupper (used in uppercase_string)
#include <string.h>         // Needed for string manipulation (e.g., strcspn, strcmp)
#include <stdlib.h>         // Needed for atoi (string to integer conversion)
#include "hardware/pio.h"   // Needed for PIO functions (e.g., pio_sm_put_blocking)
#include "ws2812.pio.h"     // Needed for WS2812 PIO program
#include "hardware/timer.h" // Needed for timer functions (e.g., add_repeating_timer_us)
#include "hardware/sync.h"  // Needed for save_and_disable_interrupts, restore_interrupts
#include "pico/multicore.h" // Needed for multicore functions (e.g., multicore_launch_core1)

// led color definitions
uint32_t red          = 0x0000FF00; // Red
uint32_t green        = 0x00FF0000; // Green
uint32_t blue         = 0xFF000000; // Blue
uint32_t off          = 0x00000000; // Off
uint32_t lightGreen   = 0x00FF8080; // Light Green
uint32_t lighterGreen = 0x001f1000; // Lighter Green
uint32_t pastelGreen  = 0x0090EE90; // Pale Green


// ANSI TEXT escape codes for colors and formatting
#define AQUA                "\033[48;5;30m"   // Backgroundn text menu color for aqu
#define RED                 "\033[31m"        // Standard red
#define GREEN               "\033[32m"         // Standard green
#define BLUE                "\033[34m"        // Standard blue
#define OFF                 "\033[0m"         // Reset formatting (no color)
#define LIGHT_GREEN         "\033[92m"        // Bright green (closest to light green)
#define LIGHTER_GREEN       "\033[38;5;120m"  // Approximated lighter green using extended 256-color mode
#define PASTEL_GREEN        "\033[38;5;151m"  // Approximated pastel green using extended 256-color mode
#define WHITE               "\033[97m"        // Standard white
#define BRIGHT_WHITE        "\033[1;37m"      // Bright white (bold white)  \033[38;5;15m
#define YELLOW              "\033[93m"        // Standard yellow
#define CYAN                "\033[96m"        // Standard cyan
#define MAGENTA             "\033[95m"        // Standard magenta
#define BLUE2               "\033[94m"        // Standard blue
#define BLACK               "\033[30m"        // Standard black
#define BOLD                "\033[1m"         // Bold text
#define UNDERLINE           "\033[4m"         // Underlined text
#define CLEAR               "\033[2J"         // Clear screen
#define HOME                "\033[H"          // Move cursor to home position (top-left corner)
#define HIDE_CURSOR         "\033[?25l"       // Hide cursor
#define SHOW_CURSOR         "\033[?25h"       // Show cursor
#define MOVE_CURSOR_UP(n)   "\033[" #n "A"    // Move cursor up n lines
#define MOVE_CURSOR_DOWN(n) "\033[" #n "B"    // Move cursor down n lines
#define MOVE_CURSOR_RIGHT(n) "\033[" #n "C"   // Move cursor right n columns
#define MOVE_CURSOR_LEFT(n) "\033[" #n "D"    // Move cursor left n columns
#define CLEAR_LINE          "\033[K"          // Clear current line
#define CLEAR_SCREEN        "\033[2J"         // Clear entire screen
#define CLEAR_SCROLLBACK    "\033[3J"         // Clear scrollback buffer

#define MOVE_CURSOR_24_0   "\033[24;0H"     // Move cursor to line 24, column 0
#define MOVE_CURSOR_24_8   "\033[24;8H"      // Move cursor to line 24, column 8
#define MOVE_CURSOR_25_0   "\033[25;0H"     // Move cursor to line 25, column 0
#define MOVE_CURSOR_25_8    "\033[25;8H"      // Move cursor to line 25, column 8
#define MOVE_CURSOR_25_1    "\033[25;1H"      // Move cursor to line 25, column 1
#define MOVE_CURSOR_25_9    "\033[25;9H"      // Move cursor to line 25, column 9  
#define MOVE_CURSOR_25_10   "\033[25;10H"     // Move cursor to line 25, column 10
#define MOVE_CURSOR_SOL     "\033[G"            // Move cursor to start of line

#define MOVE_CURSOR_8       "\033[8G"      // Move cursor 8 spaces
#define MOVE_CURSOR_9       "\033[9G"      // Move cursor 9 spaces


bool updatingMenu = false; // Flag to indicate if the menu is being updated

char *inputLocation = MOVE_CURSOR_25_8; // Set the input location to line 25, column 8

uint32_t ledColor = 0x00000000; // Variable to store the color of the LED, OFF by default

uint32_t dir_mask = ((1UL << 30) - 1); // bits for all GPIOs 0-29


#define GPIO_MASK_1 ((1UL << 30) - 1)  // Mask for GPIOs 0–29 (bitwise trickery to create a mask for 30 bits of 1s)
//#define GPIO_MASK_0 (1UL << 30)        // Mask for GPIOs 0–29 


char label00[7] = "AX0";   // Label for pin 0
char label01[7] = "AX1";   // Label for pin 1
char label02[7] = "AX2";   // Label for pin 2
char label03[7] = "AY0";   // Label for pin 3
char label04[7] = "AY1";   // Label for pin 4
char label05[7] = "AY2";   // Label for pin 5
char label06[7] = "data";   // Label for pin 6
char label07[7] = "strobe"; // Label for pin 7
char label08[7] = "EN1";    // Label for pin 8
char label09[7] = "EN2";    // Label for pin 9
char label10[7] = "Reset";  // Label for pin 10
char label11[7] = "GP11";   // Label for pin 11
char label12[7] = "TX";     // Label for pin 12
char label13[7] = "RX";     // Label for pin 13
char label14[7] = "GP14";   // Label for pin 14
char label15[7] = "GP15";   // Label for pin 15
char label16[7] = "LED";   // Label for pin 16
char label17[7] = "GP17";   // Label for pin 17
char label18[7] = "GP18";   // Label for pin 18
char label19[7] = "GP19";   // Label for pin 19
char label20[7] = "GP20";   // Label for pin 20
char label21[7] = "GP21";   // Label for pin 21
char label22[7] = "GP22";   // Label for pin 22
char label23[7] = "GP23";   // Label for pin 23
char label24[7] = "GP24";   // Label for pin 24
char label25[7] = "GP25";   // Label for pin 25
char label26[7] = "GP26";   // Label for pin 26
char label27[7] = "GP27";   // Label for pin 27
char label28[7] = "GP28";   // Label for pin 28
char label29[7] = "GP29";   // Label for pin 29


#define WS2812_PIN 16
#define NUM_LEDS 1

void set_ws2812_color(uint32_t color) {
    pio_sm_put_blocking(pio0, 0, color << 8u);
}


static inline void put_pixel(uint32_t pixel_grb) {
  pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

uint32_t  gpio_values = 0; // Variable to store GPIO values
uint32_t  gpio_values_LastCheck = 0; // Variable to store GPIO values

char bitValues[12]; // Array to store bit values
int intChoice = 0; // Variable to store integer choice from user input

// Pin states (0 = OFF, 1 = ON)
int pin_states[29] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


void set_neopixel_color(uint32_t color) {
  pio_sm_put_blocking(pio0, 0, color << 8u);  // Send color to NeoPixel
}


void toggle_pin(int pin, int pin_states[]) {
  pin_states[pin] = !pin_states[pin];
}


void setupAllPins(){
  gpio_init_mask(GPIO_MASK_1);  // Initialize all GPIOs from 0 to 29
  gpio_set_dir_masked(GPIO_MASK_1, GPIO_MASK_1);  // Set them as outputs
}


void gpio_get_dir_all(){
  for (int pin = 0; pin < 30; pin++) {  // Only checking the first 30 GPIOs
    dir_mask |= (gpio_get_dir(pin) << pin);  // Shift direction bit into mask
  } 
}

 

void displayFunctionMenu(){


  printf(HOME);  // Move cursor to top-left
  
  gpio_get_dir_all(); // Get the direction of all GPIOs
  
  gpio_values = gpio_get_all(); // Get the current state of all GPIOs
  if (ledColor == off) {
    gpio_values &= ~(1 << 16); // Clear the 16th bit to 0
  } else {
    gpio_values |= (1 << 16); // Set the 16th bit to 1
  }

  printf(HIDE_CURSOR);  // Hides the cursor
  printf(BRIGHT_WHITE); // Set text color to bright white 

  printf(AQUA"ON:All High"OFF"> <"AQUA"CL:All Low"OFF"> <"AQUA"digit GP number"OFF">      "LIGHT_GREEN"Green"OFF" = High \n");

  printf("                                                    White = Low\n");

  printf("\n        BitMask:");
  for (int i = 29; i >= 0; i--) {  printf("%d", (gpio_values >> i) & 1);}

  printf("\n");
  printf("        ┌────────────────────────────────────┐\n" );
  printf("        │              RP2040-Zero           │\n");
  printf("        │5v                              %-4s│-%-6s \n",((gpio_values >> 0) & 1) ? LIGHT_GREEN"GP00"OFF : "GP00" , label00);
  printf("        │GND           * GND             %-4s│-%-6s \n",((gpio_values >> 1) & 1)  ? LIGHT_GREEN"GP01"OFF : "GP01" , label01);
  printf("        │3V3    %6s-*%-4s             %-4s│-%-6s \n",label25, ((gpio_values >> 25) & 1) ? LIGHT_GREEN"GP25"OFF : "GP25", ((gpio_values >> 2) & 1)  ? LIGHT_GREEN"GP02"OFF : "GP02" ,label02);
  printf(" %6s-│%-4s   %6s-*%-4s             %-4s│-%-6s \n",label29, ((gpio_values >> 29) & 1)? LIGHT_GREEN"GP29"OFF : "GP29",label24, ((gpio_values >> 24) & 1) ? LIGHT_GREEN"GP24"OFF : "GP24", ((gpio_values >> 3) & 1) ? LIGHT_GREEN"GP03"OFF : "GP03", label03 );
  printf(" %6s-│%-4s   %6s-*%-4s             %-4s│-%-6s \n",label28, ((gpio_values >> 28) & 1) ? LIGHT_GREEN"GP28"OFF : "GP28",label23, ((gpio_values >> 23) & 1) ? LIGHT_GREEN"GP23"OFF : "GP23", ((gpio_values >> 4) & 1)? LIGHT_GREEN"GP04"OFF : "GP04", label04 );
  printf(" %6s-│%-4s   %6s-*%-4s             %-4s│-%-6s \n",label27, ((gpio_values >> 27) & 1) ? LIGHT_GREEN"GP27"OFF : "GP27",label22, ((gpio_values >> 22) & 1) ? LIGHT_GREEN"GP22"OFF : "GP22", ((gpio_values >> 5) & 1) ? LIGHT_GREEN"GP05"OFF : "GP05", label05 );  
  printf(" %6s-│%-4s   %6s-*%-4s             %-4s│-%-6s \n",label26, ((gpio_values >> 26) & 1) ? LIGHT_GREEN"GP26"OFF : "GP26",label21,((gpio_values >> 21) & 1) ? LIGHT_GREEN"GP21"OFF : "GP21", ((gpio_values >> 6) & 1) ? LIGHT_GREEN"GP06"OFF : "GP06", label06 );
  printf(" %6s-│%-4s   %6s-*%-4s             %-4s│-%-6s \n",label15, ((gpio_values >> 15) & 1) ? LIGHT_GREEN"GP15"OFF : "GP15",label20, ((gpio_values >> 20) & 1) ? LIGHT_GREEN"GP20"OFF : "GP20", ((gpio_values >> 7) & 1) ? LIGHT_GREEN"GP07"OFF : "GP07", label07 );
  printf(" %6s-│%-4s   %6s-*%-4s             %-4s│-%-6s \n",label14, ((gpio_values >> 14) & 1) ? LIGHT_GREEN"GP14"OFF : "GP14",label19, ((gpio_values >> 19) & 1) ? LIGHT_GREEN"GP19"OFF : "GP19", ((gpio_values >> 8) & 1) ? LIGHT_GREEN"GP08"OFF : "GP08", label08 );
  printf("        │       %6s-*%-4s                 │\n",label18, ((gpio_values >> 18) & 1) ? LIGHT_GREEN"GP18"OFF : "GP18");
  printf("        │       %6s-*%-4s                 │\n",label17, ((gpio_values >> 17) & 1) ? LIGHT_GREEN"GP17"OFF : "GP17");
  printf("        │       %6s-*%-4s                 │\n",label16, (ledColor != off) ? LIGHT_GREEN"GP16"OFF : "GP16");
  printf("        │                                    │\n");
  printf("        │  %-4s   %-4s   %-4s   %-4s   %-4s  │\n" ,  ((gpio_values >> 13) & 1) ? LIGHT_GREEN"GP13"OFF : "GP13", ((gpio_values >> 12) & 1) ? LIGHT_GREEN"GP12"OFF : "GP12", ((gpio_values >> 11) & 1) ? LIGHT_GREEN"GP11"OFF : "GP11", ((gpio_values >> 10) & 1)? LIGHT_GREEN"GP10"OFF : "GP10" , ((gpio_values >> 9) & 1) ? LIGHT_GREEN"GP9"OFF : "GP9");
 

  printf("        └───┬──────┬──────┬──────┬──────┬────┘\n" );
  printf("            │      │      │      │      │\n");
  printf("           %-6.6s %-6.6s %-6.6s %-6.6s %-6.6s\n",label13,label12,label11,label10,label09);
  printf("\n"); // Extra new lines for spacing to clear the input line



  //printf(MOVE_CURSOR_25_1); // ANSI code to move cursor down 25 lines  
  //printf(MOVE_CURSOR_SOL);  // Move cursor to Start of input line
  //printf(MOVE_CURSOR_8);    // Move cursor column 

  printf("Input: ");          // do this to clear the input line on the screen
  printf(inputLocation);

  //printf(SHOW_CURSOR);      // Shows the cursor again
  //printf(MOVE_CURSOR_SOL);  // Move cursor to Start of input line
  //printf(MOVE_CURSOR_8);    // Move cursor column 8
 
 
}


void uppercase_string(char *str) {
  while (*str) {  // Loop until null terminator is found
      *str = toupper(*str); // Convert character to uppercase
      str++;  // Move to the next character
  }
}



void getInput(char *buffer) {
  int index = 0;
  char ch;
  inputLocation = MOVE_CURSOR_25_8; // Set the input location to line 25, column 8
  while (index < 2) { // Limit input to 2 characters
      ch = getchar(); // Read single character

      if (ch == '\n') break; // Enter key pressed
      if (ch == 127 || ch == '\b') { // Handle backspace
          if (index > 0) {
              printf("\b \b"); // Erase last character
              index--;
          }
      } else if (index < 2) { // Only add character if space is available
          buffer[index++] = ch;
          printf("%c", ch); // Echo typed character
      }
  
      if (index == 0) 
      {
          inputLocation = MOVE_CURSOR_25_8; // Reset input location to line 25, column 8
      } else {
          inputLocation = MOVE_CURSOR_25_9; // Move input location to line 25, column 9
      }
  
    }

  sleep_ms(100); // Wait for a short time to ensure the cursor is moved
  printf(MOVE_CURSOR_25_0); // move cursor to statt of line 25
  printf("Input:       ");  // do this to clear the input line on the screen
  printf(MOVE_CURSOR_25_0); // move cursor to statt of line 25  
  
  buffer[index] = '\0'; // Null-terminate string
}



void readFunctionMenu(){
  char choice[3];
  uint32_t ledColorChoice = lighterGreen; // Set default color to pastel green


  getInput(choice);
  if (choice != NULL) {
    uppercase_string(choice); // Convert to uppercase

    if (strcmp(choice, "CL") == 0) {
      gpio_clr_mask(GPIO_MASK_1); // Clear all GPIOs
      set_ws2812_color(off);
      ledColor=off;
    } else if (strcmp(choice, "ON") == 0) {
      gpio_set_mask(GPIO_MASK_1); // Set all GPIOs to high
      set_ws2812_color(ledColorChoice);
      ledColor=ledColorChoice;
    } else {
      char *endptr;
      int intChoice = strtol(choice, &endptr, 10); // Convert string to integer

      // Check if the conversion was successful
      if (*endptr != '\0') {
          printf("Invalid input. Please enter a valid number.\n");
          sleep_ms(2000); // Wait for 2 second before clearing the screen
          printf(CLEAR); // Clear screen
          printf(CLEAR_SCROLLBACK); // Clear scrollback buffer
      } else {
        if (intChoice == 16) {
          if (ledColor == off) {
            set_ws2812_color(ledColorChoice); 
            ledColor=ledColorChoice;
          } else {
            set_ws2812_color(off);
            ledColor=off;
          }
        } else if (intChoice >= 0 && intChoice <= 29) {
          //printf("Toggling pin %d.\n", intChoice);  sleep_ms(1000);
          gpio_get(intChoice) ? gpio_put(intChoice, 0) : gpio_put(intChoice, 1); // Toggle pin
        } else {
          printf("Invalid choice. Please try again.\n");
          sleep_ms(1000); // Wait for 1 second before clearing the screen
          printf(CLEAR); // Clear screen
          printf(CLEAR_SCROLLBACK); // Clear scrollback buffer
        } 
      }
    }     
  }
}


void triggerMenu() {

  while (true){
      displayFunctionMenu(); // Call the function to display the menu
      sleep_ms(1000); // Wait for 1 second before checking again
  }
}


void main() {
  int64_t delay_us = 5000000; // Negative value means "start-to-start" timing, positive value means "start-to-end" timing
  struct repeating_timer timer;

  stdio_init_all();
  stdio_usb_init(); // Initialize USB standard input

  sleep_ms (5000); // Wait for 5 seconds to allow USB to initialize

  // Initialize GPIO pins  
  setupAllPins();

  PIO pio = pio0;
  int sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);
  ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);


  printf(HOME);  // Hides the cursor
  printf(CLEAR); // Clear screen
  printf(CLEAR_SCROLLBACK); // Clear scrollback buffer

  displayFunctionMenu();
  // Launch function on Core 1
  multicore_launch_core1(triggerMenu);

  while (true) {
    readFunctionMenu(); 
  }
}
