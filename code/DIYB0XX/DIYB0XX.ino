#include "Nintendo.h"
/* This code uses the Nicohood Library
   Use this code at your own risk
   Code written by Simple Controllers and this code is open source.
   Meaning its free to share, improve on, or anything you like!
   Just remember to mention you used my code!
   Edited by Danny for SmashB0XX

   [Crane]
   For Smash4/U mode,
    Mod running, tilts, triggers(shields), and most firefox angles edited.
    Mods change running/walking speed. Direction, then mod1 gives perfect speed to ledgerun in smash4.
    Tilts now work with mod1.
    Triggers now give the analog value needed to shield in smash4/Ultimate. LRAStart now works too because of this
    FFox angles (b0xx method, c stick) now almost work properly. only mod1+diagonal+cright/left do not work (in smash4, not Ultimate). Can't figure out why.
    C stickless firefox angles function now.

   For Project M mode
    Tilts, firefox angles, shield drop angles (Axe method), and wavedash angles all fixed.
    Fixed bug where left and down were not working on SSS and menus.

    All three modes are in this one program, so you don't have to keep swapping.

    To launch in Melee mode, just plug in normally
    To launch in Ultimate/4 mode, hold B while plugging in.
    To launch in Project M mode, hold X while plugging in.

   Be sure to button check before starting a set, to confirm you are in the right mode.
   If mod1+up makes you jump, or if mod1+down+B makes you neutral B, you are in the wrong mode. Check both of these.

   [Jack "Hexadecimal" Stensrud]
    Added 2ip without reactivation SOCD controls for the left/right inputs. Should be more in line with what the B0XX uses.

   [Crane]
    Combined the three SOCD versions of the code. If you want to change SOCD for a specific game, you can do so on
    line 93 for Melee, line 129 for Ultimate, or line 134 for PM. The official B0XX methods are the default, and I
    advise against editing them. currentSOCD can set to "Neutral", "TwoIP", or "TwoIPNoReactivate", without the quotes.
*/
//This makes the controller bidirection data line on pin number8
CGamecubeConsole GamecubeConsole(8);      //Defines a "Gamecube Console" sending data to the console on pin 8
Gamecube_Data_t d = defaultGamecubeData;   //Structure for data to be sent to console

enum game
{
  Melee,
  PM,
  Ultimate
};

enum SOCD
{
  Neutral,
  TwoIP,
  TwoIPNoReactivate
};

//Not needed
//CGamecubeController GamecubeController1(7);

//This is the pinout of the controller.  Can be changed to your liking.  I may have mixed up some of the tilt pins but you can change that physically in your build or through the code.  Just do test runs along the way.
const int A = 46;
const int B = 44;
const int X = 49;
const int Y = 31;
const int Z = 41;
const int START = 39;

const int R = 24;
const int L = 34;
//const int RLIGHT = 36; Only if using LightShield Button
//This is the value of analog shielding 74 is lightest possible on gamecube.  It varies from gamecube to dolphin no idea why.
//const int RLIGHTv = 74;

const int LEFT = 26;
const int RIGHT = 40;
const int UP = 48;
const int DOWN = 35;

const int MOD1 = 38;
const int MOD2 = 22;

const int CLEFT = 28;
const int CRIGHT = 50;
const int CUP = 30;
const int CDOWN = 51;

const int NEUTRAL = 128;

bool isLeft = false;
bool isRight = true;
bool isUp = false;
bool isDown = true;
bool isHoldingLeft = false;
bool isHoldingRight = false;
bool isHoldingUp = false;
bool isHoldingDown = false;

game currentGame = Melee;
SOCD currentSOCD = TwoIPNoReactivate;

void setup()
{
  //This is establishing the pin assignments up there to input pins
  pinMode(A, INPUT_PULLUP);
  pinMode(B, INPUT_PULLUP);
  pinMode(X, INPUT_PULLUP);
  pinMode(Y, INPUT_PULLUP);
  pinMode(Z, INPUT_PULLUP);
  pinMode(START, INPUT_PULLUP);

  pinMode(R, INPUT_PULLUP);
  pinMode(L, INPUT_PULLUP);
  //pinMode(RLIGHT, INPUT_PULLUP);

  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);

  pinMode(MOD1, INPUT_PULLUP);
  pinMode(MOD2, INPUT_PULLUP);

  pinMode(CLEFT, INPUT_PULLUP);
  pinMode(CRIGHT, INPUT_PULLUP);
  pinMode(CUP, INPUT_PULLUP);
  pinMode(CDOWN, INPUT_PULLUP);

  //this is just to turn off built in LED at start
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  if (digitalRead(B) == LOW)
  {
    currentGame = Ultimate;
    currentSOCD = TwoIP;
  }
  if (digitalRead(X) == LOW)
  {
    currentGame = PM;
    currentSOCD = TwoIPNoReactivate;
  }



  //Not needed
  //GamecubeController1.read();
}

void loop()
{
  //This resets and establishes all the values after the controller sends them to the console and helps with initial "zeroing"
  int pinA = 0;
  int pinB = 0;
  int pinX = 0;
  int pinY = 0;
  int pinZ = 0;
  int pinSTART = 0;

  int pinR = 0;
  int pinL = 0;
  int pinRLIGHT = 0;
  int pinLLIGHT = 0;

  int pinDLEFT = 0;
  int pinDRIGHT = 0;
  int pinDUP = 0;
  int pinDDOWN = 0;

  int mod1 = 0;
  int mod2 = 0;

  int pinCLEFT = 0;
  int pinCRIGHT = 0;
  int pinCUP = 0;
  int pinCDOWN = 0;

  int pinxAxisC = NEUTRAL;
  int pinyAxisC = NEUTRAL;

  int pinxAxis = NEUTRAL;
  int xmod = 0;
  int pinyAxis = NEUTRAL;

  int rightOne = 0;
  int leftOne = 0;
  int downOne = 0;
  int upOne = 0;

  int cUp = 0;
  int cDown = 0;
  int cLeft = 0;
  int cRight = 0;

  // Common cardinal conditionals
  bool horizontal = leftOne || rightOne;
  bool vertical = upOne || downOne;
  bool diagonal = horizontal && vertical;

  if (digitalRead(MOD1) == LOW && digitalRead(MOD2) == HIGH)mod1 = 1;
  if (digitalRead(MOD2) == LOW && digitalRead(MOD1) == HIGH)mod2 = 1;

  //SOCD method for left/right is 2ip without reactivation
  if (currentSOCD == TwoIPNoReactivate)
  {
    // X Axis
    if (digitalRead(LEFT) == LOW && isRight == true)
    {
      //if left is pressed and isRight is true(if you press left while holding right):
      pinxAxis = NEUTRAL - 127;
      leftOne = 1;
      isHoldingRight = true;
    }
    else if (digitalRead(RIGHT) == LOW && isLeft == true)
    {
      //else, if right is pressed and isLeft is true (if you press right while holding left):
      pinxAxis = NEUTRAL + 127;
      rightOne = 1;
      isHoldingLeft = true;
    }

    if (digitalRead(LEFT) == HIGH && digitalRead(RIGHT) == LOW) {
      //if left is not pressed and right is pressed:
      if (isHoldingRight == false) {
        pinxAxis = NEUTRAL + 127;
        rightOne = 1;
      }
      isRight = true;
      isLeft = false;
      isHoldingLeft = false;
    }
    if (digitalRead(LEFT) == LOW && digitalRead(RIGHT) == HIGH) {
      //if left is pressed and right is not pressed:
      if (isHoldingLeft == false) {
        pinxAxis = NEUTRAL - 127;
        leftOne = 1;
      }
      isLeft = true;
      isRight = false;
      isHoldingRight = false;
    }
    if (digitalRead(LEFT) == HIGH && digitalRead(RIGHT) == HIGH) {
      //if neither button is being pressed, set all left/right booleans to false
      isHoldingLeft = false;
      isHoldingRight = false;
      isLeft = false;
      isRight = false;
    }
    /*************************************/
    // Y Axis
    if (digitalRead(DOWN) == LOW && isUp == true)
    {
      //if down is pressed and isUp is true(if you press down while holding up):
      pinyAxis = NEUTRAL - 127;
      downOne = 1;
      isHoldingUp = true;
    }
    else if (digitalRead(UP) == LOW && isDown == true)
    {
      //else, if up is pressed and isDown is true (if you press up while holding down):
      pinyAxis = NEUTRAL + 127;
      upOne = 1;
      isHoldingDown = true;
    }

    if (digitalRead(DOWN) == HIGH && digitalRead(UP) == LOW) {
      //if down is not pressed and up is pressed:
      if (isHoldingUp == false) {
        pinyAxis = NEUTRAL + 127;
        upOne = 1;
      }
      isUp = true;
      isDown = false;
      isHoldingDown = false;
    }
    if (digitalRead(DOWN) == LOW && digitalRead(UP) == HIGH) {
      //if down is pressed and up is not pressed:
      if (isHoldingDown == false) {
        pinyAxis = NEUTRAL - 127;
        downOne = 1;
      }
      isDown = true;
      isUp = false;
      isHoldingUp = false;
    }
    if (digitalRead(DOWN) == HIGH && digitalRead(UP) == HIGH) {
      //if neither button is being pressed, set all down/up booleans to false
      isHoldingDown = false;
      isHoldingUp = false;
      isDown = false;
      isUp = false;
    }
  }

  //SOCD method for left/right is 2ip
  if (currentSOCD == TwoIP)
  {
    // X Axis
    if (digitalRead(LEFT) == LOW && isRight == true)
    {
      pinxAxis = NEUTRAL - 127;
      leftOne = 1;
    }
    else if (digitalRead(RIGHT) == LOW && isLeft == true)
    {
      pinxAxis = NEUTRAL + 127;
      rightOne = 1;
    }

    if (digitalRead(LEFT) == HIGH && digitalRead(RIGHT) == LOW) {
      pinxAxis = NEUTRAL + 127;
      rightOne = 1;
      isRight = true;
      isLeft = false;
    }
    if (digitalRead(LEFT) == LOW && digitalRead(RIGHT) == HIGH) {
      pinxAxis = NEUTRAL - 127;
      leftOne = 1;
      isLeft = true;
      isRight = false;
    }

    /*************************************/
    // Y Axis
    if (digitalRead(DOWN) == LOW && isUp == true)
    {
      pinyAxis = NEUTRAL - 127;
      downOne = 1;
    }
    else if (digitalRead(UP) == LOW && isDown == true)
    {
      pinyAxis = NEUTRAL + 127;
      upOne = 1;
    }

    if (digitalRead(DOWN) == HIGH && digitalRead(UP) == LOW) {
      pinyAxis = NEUTRAL + 127;
      upOne = 1;
      isUp = true;
      isDown = false;
    }
    if (digitalRead(DOWN) == LOW && digitalRead(UP) == HIGH) {
      pinyAxis = NEUTRAL - 127;
      downOne = 1;
      isDown = true;
      isUp = false;
    }
  }

  //SOCD method for left/right is neutral
  if (currentSOCD == Neutral)
  {
    if (digitalRead(LEFT) == HIGH && digitalRead(RIGHT) == LOW) {
      pinxAxis = NEUTRAL + 127;
      rightOne = 1;
    }
    if (digitalRead(LEFT) == LOW && digitalRead(RIGHT) == HIGH) {
      pinxAxis = NEUTRAL - 127;
      leftOne = 1;
    }
    if (digitalRead(DOWN) == HIGH && digitalRead(UP) == LOW) {
      pinyAxis = NEUTRAL + 127;
      upOne = 1;
    }
    if (digitalRead(DOWN) == LOW && digitalRead(UP) == HIGH) {
      pinyAxis = NEUTRAL - 127;
      downOne = 1;
    }
  }

  //Reads CStick pins, same logic as controlstick values.
  if (digitalRead(CLEFT) == HIGH && digitalRead(CRIGHT) == LOW) {
    pinxAxisC = 255;
    cRight = 1;
  }
  if (digitalRead(CLEFT) == LOW && digitalRead(CRIGHT) == HIGH) {
    pinxAxisC = 0;
    cLeft = 1;
  }
  if (digitalRead(CDOWN) == HIGH && digitalRead(CUP) == LOW) {
    pinyAxisC = 255;
    cUp = 1;
  }
  if (digitalRead(CDOWN) == LOW && digitalRead(CUP) == HIGH) {
    pinyAxisC = 0;
    cDown = 1;
  }

  if (digitalRead(A) == LOW)pinA = 1;
  if (digitalRead(B) == LOW)pinB = 1;
  if (digitalRead(X) == LOW)pinX = 1;
  if (digitalRead(Y) == LOW)pinY = 1;
  if (digitalRead(Z) == LOW)pinZ = 1;
  if (digitalRead(START) == LOW)pinSTART = 1;



  //This is for digital shield.
  //[Crane] Also does a bit of analog shield for Smash4 and Ultimate
  if (digitalRead(R) == LOW) {
    pinR = 1;
    pinRLIGHT = 125;
  }
  if (digitalRead(L) == LOW) {
    pinL = 1;
    pinLLIGHT = 125;
  }

  if (currentGame == Melee)
  {
    if (mod1) {
      if (horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 59);
      }
      if (vertical) {
        pinyAxis = NEUTRAL + ((upOne - downOne) * 52);
      }
      if (diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 59);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 23);
      }
      //Ambiguous DI
      if (horizontal && pinA) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 47);
      }
      //FireFox Angles with cButtons
      if (cUp && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 53);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 37);
      }
      if (cDown && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 62);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 30);
      }
      if (cLeft && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 63);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 37);
      }
      if (cRight && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 51);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 42);
      }
      //Up and Down Forward Smash
      if (vertical && (cLeft || cRight)) {
        pinxAxisC = NEUTRAL + ((cRight - cLeft) * 127);
        pinyAxisC = NEUTRAL + ((upOne - downOne) * 41);
      }
    }

    if (mod2) {
      if (horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 23);
      }
      if (vertical) {
        pinyAxis = NEUTRAL + ((upOne - downOne) * 59);
      }
      //Keeps B Reversals Fair
      if (horizontal && pinB) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 59);
      }
      if (diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 23);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 59);
      }

      //Ambiguous DI
      if (horizontal && pinA) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 35);
      }
      //FireFox Angles with cButtons
      if (cUp && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 44);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 63);
      }
      if (cDown && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 31);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 64);
      }
      if (cLeft && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 37);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 63);
      }
      if (cRight && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 47);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 57);
      }
    }
  }
  else if (currentGame == Ultimate)
  {
    if (mod1) {
      if (horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 53);
      }
      if (vertical) {
        pinyAxis = NEUTRAL + ((upOne - downOne) * 51);
      }

      /******************/

      if (horizontal && pinB) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 41);
      }

      /*******************/

      if (diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 53);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 40);
      }
      //Ambiguous DI
      if (horizontal && pinA) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 47);
      }
      //FireFox Angles with cButtons
      if (cUp && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 71);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 35);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      if (cDown && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 61);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 49);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      if (cLeft && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 66);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 42);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      if (cRight && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 75);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 27);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      //Up and Down Forward Smash
      if (vertical && (cLeft || cRight)) {
        pinxAxisC = NEUTRAL + ((cRight - cLeft) * 65);
        pinyAxisC = NEUTRAL + 40;
      }
    }

    if (mod2) {
      if (horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 26);
      }
      if (vertical) {
        pinyAxis = NEUTRAL + ((upOne - downOne) * 51);
      }
      //Keeps B Reversals Fair
      if (horizontal && pinB) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 41);
      }

      if (diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 40);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 68);
      }

      //Ambiguous DI
      if (horizontal && pinA) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 35);
      }
      //FireFox Angles with cButtons
      if (cUp && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 35);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 71);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      if (cDown && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 49);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 61);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      if (cLeft && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 42);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 66);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      if (cRight && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 27);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 75);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      //Up and Down Forward Smash
      if (vertical && (cLeft || cRight)) {
        pinxAxisC = NEUTRAL + ((cRight - cLeft) * 65);
        pinyAxisC = NEUTRAL - 40;
      }

    }
  }
  else if (currentGame == PM)
  {
    if (mod1) {
      if (horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 49);
      }
      if (vertical) {
        pinyAxis = NEUTRAL + ((upOne - downOne) * 65);
      }
      if (diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 68);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 28);

        if (cUp) {
          pinxAxis = NEUTRAL + ((rightOne - leftOne) * 53);
          pinyAxis = NEUTRAL + ((upOne - downOne) * 37);
          pinxAxisC = NEUTRAL;
          pinyAxisC = NEUTRAL;
        }
        if (cDown) {
          pinxAxis = NEUTRAL + ((rightOne - leftOne) * 62);
          pinyAxis = NEUTRAL + ((upOne - downOne) * 30);
          pinxAxisC = NEUTRAL;
          pinyAxisC = NEUTRAL;
        }
        if (cLeft) {
          pinxAxis = NEUTRAL + ((rightOne - leftOne) * 63);
          pinyAxis = NEUTRAL + ((upOne - downOne) * 37);
          pinxAxisC = NEUTRAL;
          pinyAxisC = NEUTRAL;
        }
        if (cRight) {
          pinxAxis = NEUTRAL + ((rightOne - leftOne) * 51);
          pinyAxis = NEUTRAL + ((upOne - downOne) * 42);
          pinxAxisC = NEUTRAL;
          pinyAxisC = NEUTRAL;
        }
      }
      //Ambiguous DI
      if (horizontal && pinA) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 47);
      }
      //FireFox Angles with cButtons
      //Up and Down Forward Smash
      if (vertical && (cLeft || cRight)) {
        pinxAxisC = NEUTRAL + ((cRight - cLeft) * 127);
        pinyAxisC = NEUTRAL + ((upOne - downOne) * 41);
      }
    }

    if (mod2) {
      if (horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 33);
      }
      if (vertical) {
        pinyAxis = NEUTRAL + ((upOne - downOne) * 59);
      }
      //Keeps B Reversals Fair
      if (horizontal && pinB) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 59);
      }
      if (diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 28);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 68);
      }

      //Ambiguous DI
      if (horizontal && pinA) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 35);
      }
      //FireFox Angles with cButtons
      if (cUp && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 44);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 63);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      if (cDown && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 31);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 64);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      if (cLeft && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 37);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 63);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
      if (cRight && diagonal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 47);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 57);
        pinxAxisC = NEUTRAL;
        pinyAxisC = NEUTRAL;
      }
    }
  }


  if (pinL) {

    if (horizontal) {
      pinxAxis = NEUTRAL + ((rightOne - leftOne) * 127);
    }
    if (vertical) {
      pinyAxis = NEUTRAL + ((upOne - downOne) * 127);
    }

    if (upOne && horizontal) {
      pinxAxis = NEUTRAL + ((rightOne - leftOne) * 52);
      pinyAxis = NEUTRAL + 52;
    }

    //Axe Method Shield Drop
    if (currentGame == Melee)
    {
      if (downOne && horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 96);
        pinyAxis = NEUTRAL - 91;
      }
    }
    else
    {
      if (downOne && horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 96);
        pinyAxis = NEUTRAL - 99;
      }
    }

    if (mod1) {
      pinLLIGHT = 80;
      pinL = 0;
    }
    if (mod2) {
      pinLLIGHT = 80;
      pinL = 0;
    }

    //Wavedash with L and Mod1
    if ((horizontal && downOne) && mod1) {
      if (currentGame == Melee)
      {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 110);
        pinyAxis = NEUTRAL - 65;
      }
      if (currentGame == Ultimate)
      {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 110);
        pinyAxis = NEUTRAL - 65;
      }
      if (currentGame == PM)
      {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 110);
        pinyAxis = NEUTRAL - 65;
      }
    }
    //Wavedash with L and Mod2
    if ((horizontal && downOne) && mod2) {
      pinxAxis = NEUTRAL + ((rightOne - leftOne) * 65);
      pinyAxis = NEUTRAL - 110;
    }
  }

  //Manual Shield Tilt with R
  if (pinR) {

    if (currentGame == Ultimate)
    {
      if (downOne || upOne) {
        pinyAxis = NEUTRAL + ((upOne - downOne) * 51);
      }
      if (horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 51);
      }
      if (horizontal && (downOne || upOne)) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 51);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 51);
      }
    }
    else
    {
      if (downOne || upOne) {
        pinyAxis = NEUTRAL + ((upOne - downOne) * 52);
      }
      if (horizontal) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 55);
      }
      if (horizontal && (downOne || upOne)) {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 52);
        pinyAxis = NEUTRAL + ((upOne - downOne) * 52);
      }
    }

    //Wavedash with R and Mod1
    if ((horizontal && downOne) && mod1) {
      if (currentGame == Melee)
      {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 110);
        pinyAxis = NEUTRAL - 65;
      }
      if (currentGame == Ultimate)
      {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 110);
        pinyAxis = NEUTRAL - 65;
      }
      if (currentGame == PM)
      {
        pinxAxis = NEUTRAL + ((rightOne - leftOne) * 110);
        pinyAxis = NEUTRAL - 65;
      }
    }
    //Wavedash with R and Mod2
    if ((horizontal && downOne) && mod2) {
      pinxAxis = NEUTRAL + ((rightOne - leftOne) * 65);
      pinyAxis = NEUTRAL - 110;
    }
  }


  if (pinZ) {
    if (upOne && horizontal) {
      pinxAxis = NEUTRAL + ((rightOne - leftOne) * 127);
      pinyAxis = NEUTRAL + 112;
    }
  }



  //D-Pad
  if (digitalRead(MOD1) == LOW && digitalRead(MOD2) == LOW) {
    pinxAxisC = NEUTRAL;
    pinyAxisC = NEUTRAL;
    if (digitalRead(CRIGHT) == HIGH && digitalRead(CLEFT) == LOW) {
      pinDLEFT = 1;
    }
    else if (digitalRead(CUP) == HIGH && digitalRead(CDOWN) == LOW) {
      pinDDOWN = 1;
    }
    else if (digitalRead(CDOWN) == HIGH  && digitalRead(CUP) == LOW) {
      pinDUP = 1;
    }
    else if (digitalRead(CLEFT) == HIGH  && digitalRead(CRIGHT) == LOW) {
      pinDRIGHT = 1;
    }
  }



  //reports data
  d.report.a = pinA;
  d.report.b = pinB;
  d.report.x = pinX;
  d.report.y = pinY;
  d.report.z = pinZ;
  d.report.start = pinSTART;
  d.report.r = pinR;
  d.report.l = pinL;
  d.report.left = pinLLIGHT;
  d.report.right = pinRLIGHT;
  d.report.xAxis = pinxAxis;
  d.report.yAxis = pinyAxis;
  d.report.cxAxis = pinxAxisC;
  d.report.cyAxis = pinyAxisC;
  d.report.dup = pinDUP;
  d.report.dright = pinDRIGHT;
  d.report.ddown = pinDDOWN;
  d.report.dleft = pinDLEFT;
  //sends the complied data to console when console polls for the input
  GamecubeConsole.write(d);

}
