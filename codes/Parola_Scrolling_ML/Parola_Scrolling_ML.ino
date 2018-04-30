// Use the Parola library to scroll text on the display
//
// Demonstrates the use of the scrolling function to display text received
// from the serial interface on multiple lines of matrix displays
//
// User can enter text on the serial monitor and this will display as a
// scrolling message on the display.
// Message should be prefixed with the line number on which it should be displayed.
// Speed for the display is controlled by a pot on SPEED_IN analog in.
// Scrolling direction is controlled by a switch on DIRECTION_SET digital in.
// Invert ON/OFF is set by a switch on INVERT_SET digital in.
//
// UISwitch library can be found at https://github.com/MajicDesigns/MD_UISwitch
//
// NOTE: MD_MAX72xx library must be installed and configured for the LED
// matrix type being used. Refer documentation included in the MD_MAX72xx
// library or see this link:
// https://majicdesigns.github.io/MD_MAX72XX/page_hardware.html
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// set to 1 if we are implementing the user interface pot, switch, etc
#define USE_UI_CONTROL  1

#if USE_UI_CONTROL
#include <MD_UISwitch.h>
#endif

// Turn on debug statements to the serial output
#define  DEBUG  0

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

// Define the number of devices we have in the chain and the hardware interface
#define MAX_DEVICES 8
#define BUF_SIZE  150

struct LineDefinition
{
  MD_Parola  P;                   // object definition
  char    curMessage[BUF_SIZE];   // message for this display
  boolean newMessageAvailable;    // true if new message arrived
};

// Add new entries for more lines.
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
struct LineDefinition  Line[] =
{
  { MD_Parola(11, 13, 10, MAX_DEVICES), "abc", false },
};

#define MAX_LINES  1// (sizeof(Line)/sizeof(LineDefinition))

#define PAUSE_TIME      1000
#define SPEED_DEADBAND  5

// Scrolling parameters
#if USE_UI_CONTROL
#define SPEED_IN      A5
#define DIRECTION_SET 8 // change the effect
#define INVERT_SET    9 // change the invert

#endif // USE_UI_CONTROL

uint8_t frameDelay = 25;  // default frame delay value
textEffect_t  scrollEffect = PA_SCROLL_LEFT;

// Global message buffer shared by Serial and Scrolling functions
char newMessage[BUF_SIZE];
uint8_t putLine = 0;

#if USE_UI_CONTROL

MD_UISwitch_Digital uiDirection(DIRECTION_SET);
MD_UISwitch_Digital uiInvert(INVERT_SET);

void doUI(void)
{
  // set the speed if it has changed
  {
    int16_t speed = map(analogRead(SPEED_IN), 0, 1023, 10, 150);

    if ((speed >= ((int16_t)Line[0].P.getSpeed() + SPEED_DEADBAND)) ||
      (speed <= ((int16_t)Line[0].P.getSpeed() - SPEED_DEADBAND)))
    {
      for (uint8_t i=0; i<MAX_LINES; i++)
      {
        Line[i].P.setSpeed(speed);
        Line[i].P.setPause(speed);
        frameDelay = speed;
      }
      PRINT("\nChanged speed to ", Line[0].P.getSpeed());
    }
  }

  if (uiDirection.read() == MD_UISwitch::KEY_PRESS) // SCROLL DIRECTION
  {
    PRINTS("\nChanging scroll direction");
    scrollEffect = (scrollEffect == PA_SCROLL_LEFT ? PA_SCROLL_RIGHT : PA_SCROLL_LEFT);
    for (uint8_t i=0; i<MAX_LINES; i++)
    {
      Line[i].P.setTextEffect(scrollEffect, scrollEffect);
      Line[i].P.displayReset();
    }
  }

  if (uiInvert.read() == MD_UISwitch::KEY_PRESS)  // INVERT MODE
  {
    PRINTS("\nChanging invert mode");
    for (uint8_t i=0; i<MAX_LINES; i++)
    {
      Line[i].P.setInvert(!Line[i].P.getInvert());
    }
  }
}
#endif // USE_UI_CONTROL

void readSerial(void)
{
  static int16_t putIndex = -1;
  char c;
  
  while (Serial.available())
  {
    c = (char)Serial.read();
    Serial.println(putIndex);
    //c = c + '#';
    Serial.println(c);
    if (putIndex == -1)   // first character should be the line number
    {
      putIndex=0;
    }
    if(putIndex>=0){
    if ((c == '#') || (putIndex >= BUF_SIZE-2)) // end of message character or full buffer
    {
      Serial.print("Yes");
      // put in a message separator and end the string
      newMessage[putIndex] = '\0';
      // restart the index for next filling spree and flag we have a message waiting
      putIndex = -1;
      Line[0].newMessageAvailable = true;
    } else{
      // Just save the next char in next location
      newMessage[putIndex++] = c;
      Serial.println(newMessage);
    }
    }
  }
}

void setup()
{
  Serial.begin(57600);

#if USE_UI_CONTROL
  uiDirection.begin();
  uiInvert.begin();
  pinMode(SPEED_IN, INPUT);

  doUI();
#endif // USE_UI_CONTROL

  for (uint8_t i=0; i<1; i++)
  {
    Line[i].P.begin();
    Line[i].P.displayClear();
    Line[i].P.displaySuspend(false);
    Line[i].P.setSpeed(A1);
    Line[i].P.displayScroll(Line[i].curMessage, PA_LEFT, scrollEffect, frameDelay);

    strcpy(Line[i].curMessage, "Welcome! Enter today's forcast");
  }
  newMessage[0] = '\0';

  Serial.begin(57600);
  Serial.print("\n[Parola Scrolling Display Multi Line]\n");
  Serial.print(MAX_LINES);
  Serial.print(" lines\nType a message for the scrolling display\nStart message with display number\nEnd message line with a newline");
}

void loop()
{
#if USE_UI_CONTROL
  doUI();
#endif // USE_UI_CONTROL

  readSerial();
  for (uint8_t i=0; i<MAX_LINES; i++)
  {
    if (Line[i].P.displayAnimate())
    {
      if (Line[i].newMessageAvailable)
      {
        Serial.println("True");
        strcpy(Line[i].curMessage, newMessage);
        Line[i].newMessageAvailable = false;
        PRINT("\nLine ", i);
        PRINT(" msg: ", newMessage);
      }
      Line[i].P.displayReset();
    }
  }
}