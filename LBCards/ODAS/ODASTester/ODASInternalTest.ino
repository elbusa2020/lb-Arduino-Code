//////////////////////////////////////////////////////////////////////////////////////
// ODAS card internal loopback test code
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// uint8_t internalLoopBackTestCard(void) - 
//  Set all pins to inputs
//  Write to OLATAB registers
//  Read-back from OLATAB registers
//  Bounce a 1 across all the output lines
//////////////////////////////////////////////////////////////////////////////////////

uint8_t internalLoopBackTestCard(void)
{
  myI2CMux.setI2CChannel(UUT_CARD_MUX_CH);
  switch (boardType)
  {
    // Cards with single MCP23008 parts
    case OPTOIN8I2C_CARD:
    case OPTOOUT8I2C_CARD:
    case I2CIO8_CARD:
    case I2CIO8X_CARD:
      return (internalLoopbackTestSingleMCP23008());
      break;
    // Cards with single MCP23017 parts
    case DIGIO16I2C_CARD:
    case PROTO16I2C_CARD:
    case ODASRELAY16_CARD:
      return (intLBTstSingleMCP23017());
      break;
    // Cards with two MCP23017 parts
    case DIGIO32I2C_CARD:
      return (internaltestDigio32Card());
      break;
    // Cards with 8 MCP23017 parts
    case DIGIO128_CARD:
      return (internalLoopBackTestDIGIO128_CARD());
      break;
    case ODASPSOC5_CARD:
      Serial.println(F("Not supported at present"));
      break;
    case NEW_CARD:
      Serial.println(F("Not supported at present"));
      break;
    default:
      Serial.println(F("Undefined board type"));
      return 1;
      break;
  }
  myI2CMux.setI2CChannel(UUT_CARD_MUX_CH);
  return 1; // fail
}

//////////////////////////////////////////////////////////////////////////////////////
// uint8_t internalLoopbackTestSingleMCP23008(void) - Test the OptoIn8-I2C card
// Puts out 8 bit test vector on DIGIO32 pins
// Looks at the returned values on the OptoIn8-I2C
//////////////////////////////////////////////////////////////////////////////////////

uint8_t internalLoopbackTestSingleMCP23008(void)
{
  unsigned char readVal;
  int testPass = 1;
  //  Serial.println(F("Internal tests OptoIn8-I2C card"));
  for (int loopVal = 0; loopVal < 8; loopVal++)
    singleMCP23008.pinMode(loopVal,INPUT);
  singleMCP23008.writeOLAT(0x5a);
  if (singleMCP23008.readOLAT() != 0x5a)
    testPass = 0;
  if (testPass)
    return 0;
  else
    return 1;
}

//////////////////////////////////////////////////////////////////////////////////////
// uint8_t intLBTstSingleMCP23017(void) - Test the PROTO16-I2C card
//////////////////////////////////////////////////////////////////////////////////////

uint8_t intLBTstSingleMCP23017(void)
{
  uint8_t failed = 0;
  uint8_t loopCnt;
  uint16_t readBackVal;
  //  Serial.println(F("Testing single MCP23017 card"));
  for (loopCnt = 0; loopCnt < 16; loopCnt++)
    singleMCP23017.pinMode(loopCnt, OUTPUT);
  singleMCP23017.writeGPIOAB(0x55aa);
  readBackVal = singleMCP23017.readGPIOAB();
  if (readBackVal != 0x55aa)
  {
    Serial.print(F("Readback="));
    Serial.println(readBackVal);
    failed = 1;
  }
  delay(10);
  singleMCP23017.writeGPIOAB(0xaa55);
  readBackVal = singleMCP23017.readGPIOAB();
  if (readBackVal != 0xaa55)
  {
    Serial.print(F("Readback="));
    Serial.println(readBackVal);
    failed = 1;
  }
  for (loopCnt = 0; loopCnt < 16; loopCnt++)
    singleMCP23017.pinMode(loopCnt, INPUT);
  delay(10);
  return (failed);
}

//////////////////////////////////////////////////////////////////////////////////////
// uint8_t internaltestDigio32Card()
//////////////////////////////////////////////////////////////////////////////////////

uint8_t internaltestDigio32Card(void)
{
  uint16_t wrBit;
  uint16_t rback16;
  uint8_t pass0fail1 = 0;
  for (wrBit = 0; wrBit < 32; wrBit++)
    Dio32.pinMode(wrBit, INPUT);
  for (wrBit = 1; wrBit != 0; wrBit <<= 1)
  {
    Dio32.writeOLATAB(0, wrBit);
    rback16 = Dio32.readOLATAB(0);
    if (rback16 != wrBit)
    {
      pass0fail1 = 1;
      Serial.print(F("internaltestDigio32Card(): Chip 0 Wrote bit: 0x"));
      Serial.println(wrBit, HEX);
      Serial.print(F("internaltestDigio32Card(): Chip 0 Got back: 0x"));
      Serial.println(rback16, HEX);
    }
  }
  for (wrBit = 0xfffe; wrBit != 0; wrBit <<= 1)
  {
    Dio32.writeOLATAB(0, wrBit);
    rback16 = Dio32.readOLATAB(0);
    if (rback16 != wrBit)
    {
      pass0fail1 = 1;
      Serial.print(F("internaltestDigio32Card(): Chip 0 (High) Wrote bit: 0x"));
      Serial.println(wrBit, HEX);
      Serial.print(F("internaltestDigio32Card(): Chip 0 (High) Got back: 0x"));
      Serial.println(rback16, HEX);
    }
  }
  for (wrBit = 1; wrBit != 0; wrBit <<= 1)
  {
    Dio32.writeOLATAB(1, wrBit);
    rback16 = Dio32.readOLATAB(1);
    if (rback16 != wrBit)
    {
      pass0fail1 = 1;
      Serial.print(F("internaltestDigio32Card(): Chip 1 Wrote bit: 0x"));
      Serial.println(wrBit, HEX);
      Serial.print(F("internaltestDigio32Card(): Chip 1 Got back: 0x"));
      Serial.println(rback16, HEX);
    }
  }
  for (wrBit = 0xfffe; wrBit != 0; wrBit <<= 1)
  {
    Dio32.writeOLATAB(1, wrBit);
    rback16 = Dio32.readOLATAB(1);
    if (rback16 != wrBit)
    {
      pass0fail1 = 1;
      Serial.print(F("internaltestDigio32Card(): Chip 1 Wrote bit: 0x"));
      Serial.println(wrBit, HEX);
      Serial.print(F("internaltestDigio32Card(): Chip 1 Got back: 0x"));
      Serial.println(rback16, HEX);
    }
  }
  return pass0fail1;
}

//////////////////////////////////////////////////////////////////////////////////////
// uint8_t internalLoopBackTestDIGIO128_CARD(void) -
//////////////////////////////////////////////////////////////////////////////////////

uint8_t internalLoopBackTestDIGIO128_CARD(void)
{
  uint8_t testPass = 1;
  for (uint8_t port = 0; port < 128; port++)
    Dio128.pinMode(port, INPUT_PULLUP);
  for (uint8_t chip = 0; chip < 8; chip += 2)
  {
    for (uint8_t port = 0; port < 16; port++)
    {

      Dio128.pinMode((chip * 16) + port, OUTPUT);
      Dio128.pinMode(((chip + 1) * 16) + 15 - port, INPUT);

      delay(2);
      Dio128.digitalWrite((chip * 16) + port, HIGH);
      delay(2);
      if (Dio128.digitalRead(((chip + 1) * 16) + 15 - port) != HIGH)
      {
        Serial.print(F("Error on chip "));
        Serial.print(chip);
        Serial.print(F(" and port "));
        Serial.print(port);
        Serial.println(F(" Expected High"));
        testPass = 0;
      }
      Dio128.digitalWrite((chip * 16) + port, LOW);
      delay(2);
      if (Dio128.digitalRead(((chip + 1) * 16) + 15 - port) != LOW)
      {
        Serial.print(F("Error on chip "));
        Serial.print(chip);
        Serial.print(F(" and port "));
        Serial.print(port);
        Serial.println(F(" Expected LOW"));
        testPass = 0;
      }
      Dio128.pinMode((chip * 16) + port, INPUT);
      delay(2);
    }
  }
  if (testPass)
    return 0;
  else
    return 1;
}

