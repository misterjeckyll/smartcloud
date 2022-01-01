#ifndef SMARTBALL_h
#define SMARTBALL_h

//---------------------------------------------------------------
// COLOR CONSTANT
//---------------------------------------------------------------

#define RED    0xFF0000
#define BLUE   0x0000FF
#define GREEN  0x00FF00
#define BLACK  0x000000

#define WAIT   0X8F4E00
#define ALERT  0X7F0000

// EEPROM ADDRESS
#define WS_ADDR  0x10

#define SSID_LEN 64
#define PWD_LEN  64
#define IP_LEN 16

struct _factorySettings {
  uint16_t serialNumber;
  uint16_t deviceFlag;
  float adcCalibration;
};

struct _imuSettings {
  byte streamFlag;
  byte accRange;
  byte gyrRange;
};

struct _generalSettings {
  uint16_t idNumber;
};

#endif