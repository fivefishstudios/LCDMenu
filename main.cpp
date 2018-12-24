// ------------------------------------------------------------------
// Name: LCDMenu
// Description:
// Sample program for displaying a multi-layered menu system
// ------------------------------------------------------------------
#include "mbed.h"
#include "LCD_DISCO_F429ZI.h"
#include "PinDetect.h" // URL: http://mbed.org/users/AjK/libraries/PinDetect
using namespace std;

#define TITLE_BAR_HEIGHT        40
#define TITLE_BAR_COLOR         0xff0055ff
#define TITLE_BAR_TEXTCOLOR     0xff00ffff

#define STATUS_BAR_HEIGHT       30
#define STATUS_BAR_COLOR        0xff000000
#define STATUS_BAR_TEXTCOLOR    0xffaaaaaa

#define MENU_FRAME_COLOR        0xff222222
#define MENU_FRAME_TEXTCOLOR    0xffffffff
#define MENU_FRAME_LINE_HEIGHT  35
#define MENU_FRAME_PADDING      10

#define MENU_DISPLAY_COUNT       7
#define MENU_HIGHLIGHT_TEXTCOLOR 0xff000000
#define MENU_HIGHLIGHT_BACKCOLOR 0xffDDDDDD

// forward declare functions
void UpdateRotaryValue();

LCD_DISCO_F429ZI lcd;
char lcdBuffer[20]; // lcd display buffer
uint16_t LcdWidth = lcd.GetXSize();
uint16_t LcdHeight = lcd.GetYSize();

Serial pc(USBTX, USBRX); // tx, rx

DigitalOut led1(PG_14); // red
DigitalOut led2(PG_13); // green
DigitalOut greenLed(PD_12);

// Interrupt for Encoder Switch
PinDetect EncoderSwitch(PF_9);
void SwitchHandler()
{
  led2 = !led2;
  greenLed = !greenLed;
}
// Interrupt for Encoder Rotary Out A/B
PinDetect EncoderOutA(PC_12, PullUp);
PinDetect EncoderOutB(PC_11, PullUp);
int EncoderOutA_LastState = 0;
int EncoderOutB_LastState = 0;
int EncoderOutA_State;
int EncoderOutB_State;
int rotation_value = 0;

// Check Rotary Encoder status (switch + rotation)
// based on datasheet (CW Rotation)
// position | OutA | OutB
// 1 | 0 | 0
// 2 | 1 | 0
// 3 | 1 | 1 // starting
// 4 | 0 | 1

// Interrupt Handlers for A/B
void RotaryEncoderHandlerA_assert()
{
  EncoderOutA_State = 1;
  led1 = 1;
  if (EncoderOutA_State != EncoderOutA_LastState)
  {
    // check if we moved CW, increment for each detent
    if (EncoderOutA_State != EncoderOutB_State)
    {
      // CW
      rotation_value += 1;
    }
    else
    {
      rotation_value -= 1;
    }
  }
  EncoderOutA_LastState = EncoderOutA_State;
  UpdateRotaryValue();
}
void RotaryEncoderHandlerA_deasserted()
{
  EncoderOutA_State = 0;
  led1 = 0;
  if (EncoderOutA_State != EncoderOutA_LastState)
  {
    // check if we moved CW, increment for each detent
    if (EncoderOutA_State != EncoderOutB_State)
    {
      // CW
      rotation_value += 1;
    }
    else
    {
      // CCW
      rotation_value -= 1;
    }
  }
  EncoderOutA_LastState = EncoderOutA_State;
  UpdateRotaryValue();
}

void RotaryEncoderHandlerB_assert()
{
  EncoderOutB_State = 1;
  led2 = 1;
  EncoderOutB_LastState = EncoderOutB_State;
}

void RotaryEncoderHandlerB_deasserted()
{
  EncoderOutB_State = 0;
  led2 = 0;
  if (EncoderOutB_LastState)
  {
    if (!EncoderOutA_State)
    {
      rotation_value -= 1;
    }
  }
  EncoderOutB_LastState = EncoderOutB_State;
  UpdateRotaryValue();
}

void UpdateRotaryValue()
{
  // display on LCD
  lcd.SetFont(&Font24);
  lcd.SetBackColor(LCD_COLOR_BLACK);
  lcd.SetTextColor(LCD_COLOR_WHITE);
  sprintf(lcdBuffer, "%03d", rotation_value);
  lcd.DisplayStringAt(1, 100, (uint8_t *)lcdBuffer, CENTER_MODE);
}

void InitRotaryEncoder()
{
  // setup Interrupts for Encoder Switch
  EncoderSwitch.attach_asserted(&SwitchHandler);
  EncoderSwitch.setSampleFrequency(10000); // Start sampling pb input using interrupts (us)
  // setup Interrupts for Encoder Output A/B
  EncoderOutA.attach_asserted(&RotaryEncoderHandlerA_assert);
  EncoderOutA.attach_deasserted(&RotaryEncoderHandlerA_deasserted);
  EncoderOutA.setSampleFrequency(250); // in (us)
  EncoderOutB.attach_asserted(&RotaryEncoderHandlerB_assert);
  EncoderOutB.attach_deasserted(&RotaryEncoderHandlerB_deasserted);
  EncoderOutB.setSampleFrequency(250); // in (us)
}

void InitLCDScreen()
{
  lcd.Clear(LCD_COLOR_BLACK);
  // setup LCD Display
  // lcd.SetFont(&Font24);
  // lcd.SetBackColor(LCD_COLOR_BLACK); // text background color
  // lcd.SetTextColor(LCD_COLOR_WHITE); // text foreground color
  UpdateRotaryValue();
}

void DrawTitleBar(char *Title)
{
  lcd.SetTextColor(TITLE_BAR_COLOR);
  lcd.FillRect(0, 0, LcdWidth, TITLE_BAR_HEIGHT);
  lcd.SetTextColor(TITLE_BAR_TEXTCOLOR);
  lcd.SetBackColor(TITLE_BAR_COLOR);
  lcd.SetFont(&Font24);
  lcd.DisplayStringAt(0, 10, (uint8_t *)Title, CENTER_MODE);
}

void ClearStatusBar()
{
  lcd.SetTextColor(LCD_COLOR_BLACK);
  lcd.FillRect(0, LcdHeight - STATUS_BAR_HEIGHT, LcdWidth, STATUS_BAR_HEIGHT);
}

void UpdateStatusBar(char *Text)
{
  ClearStatusBar();
  lcd.SetBackColor(STATUS_BAR_COLOR);
  lcd.SetTextColor(STATUS_BAR_TEXTCOLOR);
  lcd.SetFont(&Font16);
  lcd.DisplayStringAt(5, 300, (uint8_t *)Text, LEFT_MODE);
}

void DrawMenuFrame(){
  lcd.SetTextColor(MENU_FRAME_COLOR);
  lcd.FillRect(0, TITLE_BAR_HEIGHT, LcdWidth, LcdHeight - TITLE_BAR_HEIGHT - STATUS_BAR_HEIGHT);
}

void DisplayMenuOptions(){
  lcd.SetBackColor(MENU_FRAME_COLOR);
  lcd.SetTextColor(MENU_FRAME_TEXTCOLOR);
  lcd.SetFont(&Font20);
  for (int i=0; i < MENU_DISPLAY_COUNT; i++){
    lcd.DisplayStringAt(MENU_FRAME_PADDING, (TITLE_BAR_HEIGHT + MENU_FRAME_PADDING) + (MENU_FRAME_LINE_HEIGHT * i), (uint8_t *)"AAAAAAAAAAAA", LEFT_MODE);  
  }

}

void HighlightMenuOption(uint8_t position){
  // note: text may have uneven padding, so we need to draw the highlight bkgd color manually
  lcd.SetTextColor(MENU_HIGHLIGHT_BACKCOLOR);
  lcd.FillRect(0, TITLE_BAR_HEIGHT + (MENU_FRAME_LINE_HEIGHT * (position - 1)), LcdWidth, MENU_FRAME_LINE_HEIGHT);
  // then write the text
  lcd.SetBackColor(MENU_HIGHLIGHT_BACKCOLOR);
  lcd.SetTextColor(MENU_HIGHLIGHT_TEXTCOLOR);
  lcd.DisplayStringAt(MENU_FRAME_PADDING, (TITLE_BAR_HEIGHT + MENU_FRAME_PADDING) + (MENU_FRAME_LINE_HEIGHT * (position - 1)), (uint8_t *)"SELECTED MENU", LEFT_MODE);  
}




int main()
{
  pc.baud(115200);
  InitRotaryEncoder();
  InitLCDScreen();

  // draw title bar
  DrawTitleBar("LCDMenu v0.1");

  DrawMenuFrame();
  
  DisplayMenuOptions();
  HighlightMenuOption(3);

  while (true)
  {
    led1 = !led1;
    wait(0.5);

    // update status bar with text
    UpdateStatusBar("Hello world!");
    wait(1.5);
    UpdateStatusBar("Sample text goes here... what if its long?"); // it just gets truncated
    wait(1.5);
    UpdateStatusBar("and so on...");
    wait(1.5);
  }
}