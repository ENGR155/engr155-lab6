/*
File: Lab_6_JHB.c
Author: Josh Brake
Email: jbrake@hmc.edu
Date: 9/14/19
*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "..\lib\DS1722.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart =
    "<!DOCTYPE html>"
    "<html lang=\"en\">"
    "<head>"
    "<meta charset=\"utf-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<title>E155 Lab 6</title>"
    "</head>"
    "<body>"
    "<h1>E155 Web Server</h1>"
    "<p>Control the board's LED and view the room temperature.</p>";
// LED on/off and temp refresh
char* ledStr =
    "<h2>Controls</h2>"
    "<form action=\"/ledon\" method=\"get\">"
    "<input type=\"submit\" value=\"Turn the LED on\">"
    "</form>"
    "<form action=\"/ledoff\" method=\"get\">"
    "<input type=\"submit\" value=\"Turn the LED off\">"
    "</form>"
    "<form action=\"/\" method=\"get\">"
    "<input type=\"submit\" value=\"Refresh temperature\">"
    "</form>";
// Resolution buttons
char* resolutionControls =
    "<h3>Change resolution</h3>"
    "<form action=\"/res8\" method=\"get\">"
    "<input type=\"submit\" value=\"8-bit (1 degree C steps)\">"
    "</form>"
    "<form action=\"/res10\" method=\"get\">"
    "<input type=\"submit\" value=\"10-bit (0.25 degree C steps)\">"
    "</form>"
    "<form action=\"/res12\" method=\"get\">"
    "<input type=\"submit\" value=\"12-bit (0.0625 degree C steps)\">"
    "</form>"
    "<p>The reading is the latest completed conversion. After changing resolution, wait 3 seconds, then press Refresh temperature.</p>";
char* webpageEnd   = "</body></html>";

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

int updateLEDStatus(char request[])
{
	// Keep current LED state
	int led_status = digitalRead(LED_PIN);
	int i = 0;
	while (request[i] != '\0' && request[i] != '?' && request[i] != '\n') {
		i++;
	}
	request[i] = '\0';
	// The request has been received. now process to determine whether to turn the LED on or off
	if (strcmp(request, "/REQ:ledoff") == 0) {
    digitalWrite(LED_PIN, PIO_LOW);
    led_status = 0;
  }
  else if (strcmp(request, "/REQ:ledon") == 0) {
      digitalWrite(LED_PIN, PIO_HIGH);
      led_status = 1;
  }

	return led_status;
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  // Begin with LED off
  digitalWrite(LED_PIN, PIO_LOW);
  pinMode(PB3, GPIO_OUTPUT);
  
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);
  
  // 125000 baud
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

  // TODO: Add SPI initialization code
  initSPI(6, 0, 1);
  initDS1722();
  // Wait
  delay_millis(TIM15, 151);
  // Initial resolution 8 bits
  int resolution = 8;

  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */

    // Receive web request from the ESP
    char request[BUFF_LEN] = {0}; // initialize to known value
    int charIndex = 0;
    int requestTooLong = 0;
  
    // Keep going until you get end of line character
    while(1) {
      // Wait for a complete request to be transmitted before processing
      char received = readChar(USART);
      // Ignore CR so LF and CRLF are same
      if (received == '\r') { continue; }
      // Keep room for the ending zero
      if (charIndex < BUFF_LEN - 1) {
        request[charIndex++] = received;
      } else {
        requestTooLong = 1;
      }
      // Break at end of line
      if (received == '\n') { break; }
    }
    // End string at end of line
    request[charIndex] = '\0';

    // If request is long then ignore
    if (requestTooLong) { continue; }
    // Request starts with /REQ
    if (strncmp(request, "/REQ:", 5) != 0) { continue; }

    // Update string with current LED state

    int led_status = updateLEDStatus(request);

    int requestedResolution = resolution;
    if (strcmp(request, "/REQ:res8") == 0) { requestedResolution = 8; }
    else if (strcmp(request, "/REQ:res10") == 0) { requestedResolution = 10; }
    else if (strcmp(request, "/REQ:res12") == 0) { requestedResolution = 12; }

    if (requestedResolution != resolution) {
      ds1722SetResolution(requestedResolution);
      resolution = requestedResolution;
    }

    char resolutionStr[4];
    snprintf(resolutionStr, sizeof(resolutionStr), "%d", resolution);

    // TODO: Add SPI code here for reading temperature
    // Read sensor
    int16_t rawTemperature = ds1722ReadRaw();
    // Set CS high to end SPI transaction
    // Say temperature not in int
    int magnitude = rawTemperature;
    char *sign = "";
    if (magnitude < 0) {
      magnitude = -magnitude;
      sign = "-";
    }

    // Degrees and tenths
    int degrees = magnitude / 256; // 9 bit 2 complement
    int fraction = (magnitude % 256) * 10000 / 256;
    char temperatureStr[32];
    snprintf(temperatureStr, sizeof(temperatureStr),
             "%s%d.%04d", sign, degrees, fraction);
  
    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code

    // Put measurement into temperature page
    sendString(USART, "<h2>Temperature</h2><p>Temperature: ");
    sendString(USART, temperatureStr);
    sendString(USART, " &deg;C</p>");


    sendString(USART, "<h2>Resolution</h2><p>Selected resolution: ");
    sendString(USART, resolutionStr);
    sendString(USART, "-bit</p>");

    sendString(USART, "<h2>LED Status</h2>");
    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    sendString(USART, ledStr); // button for controlling LED
    // Resolution controls
    sendString(USART, resolutionControls);
    // End tag
    sendString(USART, webpageEnd);
  }
}
