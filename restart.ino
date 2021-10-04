/*
  Restart Motherboard after Power Loss
  M.H.McKenzie (mckenzm@swellhunter.org)
  MicroCore support by MCUdude

  Requires https://github.com/MCUdude/MicroCore
  
  ------------------------------------------------------------------------------

  ATTiny13A pinout. INT0 on 6.

     _____                        +--v--+
     RESET ADC0 5/A0 PCINT5 PB5  1|o    |8  VCC                            *+5SBV*
     CLK1  ADC3 3/A3 PCINT3 PB3  2|     |7  PB2 PCINT2 2/A1 SCK  ADC1      *Sense*
  PWR*     ADC2 4/A2 PCINT4 PB4  3| T13 |6  PB1 PCINT1 1    MISO OC0B INT0
                            GND  4|     |5  PB0 PCINT0 0    MOSI OC0A       *TX*
                                  +-----+
  Where '5/A0' is the Arduino usage for comparison/conversion. A(n) follows ADC(n).
  PWM pins 5,6 Analogue write is not reliable elsewhere.

  Anticipated deluxe version:
  Wait 400ms and display 12V1, 12V2, 5V. Wait 1000ms and take PB2 low for 200ms.
  Have a SPST switch in series for VCC to disable.

  Simple version : test 5V and power on if not there.

  To do: jumpers/resistors for UART displays requiring prefixes, suffixes.
         jumper for "slow" diagnostics mode.

  RECOMMENDED FUSE SETTINGS FOR THIS SKETCH are H:FB and L:29
  ------------------------------------------------------------------------------

  Tools > Board          : ATTiny13
  Tools > BOD            : 4.3V
  Tools > Clock          : 600KHz
  Tools > Timing         : Micros disabled

  SERIAL REMINDER
  ------------------------------------------------------------------------------
  The baud rate is IGNORED on the ATtiny13 due to using a simplified serial.
  
  init:
  some pin as in for 5V
  some pin as out for pwr on
  set that one high
  wait 600ms

  loop:
  if pin in is high{
    for(;;);
  }
  else {
    set outpin low;
    delay(400);
    set outpin high;
    set outpin as input with pullup;
  for (;;);
*/

#include <EEPROM.h>

// It is my practice to store the OSSCAL value
// in the 2nd last EEPROM address.

unsigned short const eeLocation = E2END - 1;  /* 62 */
unsigned short powerSense       = 2;
unsigned short PWR              = 4;

void setup() {

  // Tune the oscillator
  uint8_t cal = EEPROM.read(E2END - 1);
  if (cal < 0x80) {
    OSCCAL = cal;
  }

  // initialize serial communication:
  Serial.begin(9600);
  delay(500);

  // input is +5V from opposite +5VSB:
  pinMode(powerSense, INPUT);

  // output will ground the PWR button if necessary:
  pinMode(PWR, OUTPUT);
  digitalWrite(PWR, HIGH);

  Serial.println(F("\r\n\r\nAuto power on..."));
  Serial.println(F("----------------"));

  // wait 1s for stability, may have been a brown-out.
  // need to test for narrow interruptions.
  // can always cycle power remotely or by watchdog.
  delay(1000);
}

void loop() {

  // read the input pin:
  int powerState = digitalRead(powerSense);

  // print out the state:
  Serial.print(F("\r\nPower state: "));
  Serial.println(powerState);

  // System is up, trap the PC:
  if (powerState) {
    Serial.println(F("Power OK, all good"));
    for (;;);
  }
  else {
    Serial.println(F("Booting now"));
    digitalWrite(PWR, LOW);
    delay(400);
    digitalWrite(PWR, HIGH);

    // Make it an input now to prevent conflict.
    pinMode(PWR, INPUT_PULLUP);
    for (;;);
  }
  // will never happen, leave for testing:
  delay(2000);
}
