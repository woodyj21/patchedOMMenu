

#include <Button.h>
#include <midi_Settings.h>
#include <midi_Message.h>
#include <MIDI.h>
#include <midi_Namespace.h>
#include <midi_Defs.h>

/**  Example OMMenuMgr Sketch
This sketch is my base working model.  Code compiles.  I've added an
addition menu item of "Set Volume", the value of pedal one is read,
mapped 0 to 127, constrained then printed to LCD.

I have commented out the example menu items, leaving on the items
I have created.  Will delete them in next update if all works

created byte midiCCtoSend, in the menu, user will select which CC#
to send to, then in "Set Volume" the peadal value will be sent
to both the LCD screen and out on the midiCCtoSend

I've got midiCCtoSend worked out.  The pedal value is sent to the
CC# in a MIDI out CC message.  I belive I can start on working out
the sub-menus now.

 
*/

#include <OMEEPROM.h>
#include <EEPROM.h>
#include "OMMenuMgr.h"
#include <LiquidCrystal.h>

 // ini pedal analog pin
#define pedalOnePin = A2  //ini it in setup w/ pinMode()
 
 // ui setup
 
    // lcd pins
const byte LCD_RS  = 8;
const byte LCD_EN  = 9;
const byte LCD_D4  = 4;
const byte LCD_D5  = 5;
const byte LCD_D6  = 6;
const byte LCD_D7  = 7;


const byte LCD_ROWS = 2;
const byte LCD_COLS = 16;

 // button values
 
 // which input is our button
const byte BUT_PIN = A0;

  // analog button read values
const int BUTSEL_VAL  = 655;      
const int BUTFWD_VAL  = 0;
const int BUTREV_VAL  = 427;
const int BUTDEC_VAL  = 271;
const int BUTINC_VAL  = 108;

const byte BUT_THRESH  = 60;

  // mapping of analog button values for menu
const int BUT_MAP[5][2] = {
                         {BUTFWD_VAL, BUTTON_FORWARD}, 
                         {BUTINC_VAL, BUTTON_INCREASE}, 
                         {BUTDEC_VAL, BUTTON_DECREASE}, 
                         {BUTREV_VAL, BUTTON_BACK}, 
                         {BUTSEL_VAL, BUTTON_SELECT} 
                    };
                            


// ====== Test Menu =========== 

byte foo = 0;
byte sel = 0;
unsigned int bar = 1;
long baz  = 0;
float bak = 0.0;
byte midiCCtoSend = 0;    //initiailize variable for MIDI.sendControlChange(midiCCtoSend, pedalValue, 1)

  // Create a list of states and values for a select input
MENU_SELECT_ITEM  sel_ign = { 2, {"Ignore"} };
MENU_SELECT_ITEM  sel_on  = { 1, {"On"} };
MENU_SELECT_ITEM  sel_off = { 0, {"Off"} };

MENU_SELECT_LIST  const state_list[] = { &sel_ign, &sel_on, &sel_off };
                                  
  // the special target for our state input
  
                                             // TARGET VAR   LENGTH                          TARGET SELECT LIST
MENU_SELECT state_select = { &sel,           MENU_SELECT_SIZE(state_list),   MENU_TARGET(&state_list) };

  // values to use 

                    //    TYPE            MAX    MIN    TARGET 
/*Comment out, but don't delete yet.
MENU_VALUE foo_value = { TYPE_BYTE,       100,   0,     MENU_TARGET(&foo) };
MENU_VALUE bar_value = { TYPE_UINT,       10000, 100,   MENU_TARGET(&bar) };
MENU_VALUE baz_value = { TYPE_LONG,       10000, 1,     MENU_TARGET(&baz) };
MENU_VALUE bak_value = { TYPE_FLOAT_1000, 0,     0,     MENU_TARGET(&bak) };
MENU_VALUE sel_value = { TYPE_SELECT,     0,     0,     MENU_TARGET(&state_select) };
*/
MENU_VALUE midiCCtoSend_value = { TYPE_BYTE, 127, 0, MENU_TARGET(&midiCCtoSend) };

                    //        LABEL           TYPE        LENGTH    TARGET
/*Comment out, but don't delete yet.
MENU_ITEM item_checkme  = { {"Byte Edit"},    ITEM_VALUE,  0,        MENU_TARGET(&foo_value) };
MENU_ITEM item_barme    = { {"UInt Edit"},     ITEM_VALUE,  0,        MENU_TARGET(&bar_value) };
MENU_ITEM item_bazme    = { {"Long Edit"},    ITEM_VALUE,  0,        MENU_TARGET(&baz_value) };
MENU_ITEM item_bakme    = { {"Float Edit"},   ITEM_VALUE,  0,        MENU_TARGET(&bak_value) };
MENU_ITEM item_state    = { {"Select Input"}, ITEM_VALUE,  0,        MENU_TARGET(&sel_value) };
*/
MENU_ITEM item_testme   = { {"Test Action"},  ITEM_ACTION, 0,        MENU_TARGET(uiQwkScreen) };
MENU_ITEM item_voltest  =  {  {"Set Volume"}, ITEM_ACTION, 0,       MENU_TARGET(uiSetVolume) };
MENU_ITEM item_midiCCtoSend = {  {"Choose CC#"}, ITEM_VALUE, 0,  MENU_TARGET(&midiCCtoSend_value) };

                   //        List of items in menu level
/*Comment out, but don't delete yet.
MENU_LIST const root_list[]   = { &item_checkme, &item_barme, &item_bazme, &item_bakme, &item_state, &item_testme, &item_voltest, &item_midiCCtoSend };
*/
MENU_LIST const root_list[]   = { &item_testme, &item_voltest, &item_midiCCtoSend };


                  // Root item is always created last, so we can add all other items to it
MENU_ITEM menu_root     = { {"Root"},        ITEM_MENU,   MENU_SIZE(root_list),    MENU_TARGET(&root_list) };


 // initialize LCD object
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

OMMenuMgr Menu(&menu_root);

//create an instance of MIDI - also need to call it in "SETUP"
MIDI_CREATE_DEFAULT_INSTANCE();



void setup() {
  
  MIDI.begin();
  
  //ini pinMode for pedalOnePin
  pinMode(A2, INPUT);

  lcd.begin(LCD_COLS, LCD_ROWS);
  
  uiClear();
  
  Menu.setDrawHandler(uiDraw);
  Menu.setExitHandler(uiClear);
  Menu.setAnalogButtonPin(BUT_PIN, BUT_MAP, BUT_THRESH);
  Menu.enable(true); 
  
  
}

void loop() {
 Menu.checkInput();
 
}

void testAction() {

 digitalWrite(5, HIGH);  
}


void uiDraw(char* p_text, int p_row, int p_col, int len) {
  lcd.setCursor(p_col, p_row);
  
  for( int i = 0; i < len; i++ ) {
    if( p_text[i] < '!' || p_text[i] > '~' )
      lcd.write(' ');
    else  
      lcd.write(p_text[i]);
  }
}


void uiClear() {
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter for Menu");
}


void uiQwkScreen() {
  lcd.clear();
  Menu.enable(false);
  
  lcd.print("Action!");
  lcd.setCursor(0, 1);
  lcd.print("Enter 2 return");
  
  while( Menu.checkInput() != BUTTON_SELECT ) {
    ; // wait!
  }
  
  Menu.enable(true);
  lcd.clear();
}  

void uiSetVolume(){
  int myVar;		//using this as pedal value
  lcd.clear();
  Menu.enable(false);
  lcd.print("Volume is:");
  lcd.setCursor(0,1);  //do i need this?

  
    while( Menu.checkInput() != BUTTON_BACK ) {
      myVar = constrain(map(analogRead(A2), 0, 48, 0, 127), 0, 100);  //MIDI does 0-127
      lcd.clear();
      lcd.setCursor(0,2);
      lcd.print("Value:");
      lcd.setCursor(6,2);
      lcd.print(myVar);
      lcd.setCursor(9,2);
      lcd.print("CC#:");
      lcd.print(midiCCtoSend);
      delay(20);
      MIDI.sendControlChange(midiCCtoSend, myVar, 1);    //trying to figure out midiCCtoSend
      delay(20);
    ; // wait!
  }
  Menu.enable(true);
}

