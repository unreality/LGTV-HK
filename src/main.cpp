#include <Arduino.h>
#include "HomeSpan.h"
#include "LGRS232.h"
#include "NeoBlinker.h"

#define REFRESH_INTERVAL 1000
#define NUM_LEDS 1
#define LED_PIN 27
#define LED_BRIGHTNESS 1
#define BTN_PIN 39


#define TV_ID  0x01
#define RX_PIN 22
#define TX_PIN 19


#define COLOR_RED       255,0,0
#define COLOR_GREEN     0,255,0
#define COLOR_GREEN_DIM 0,1,0
#define COLOR_BLUE      0,0,255
#define COLOR_MAGENTA   162,0,162
#define COLOR_YELLOW    162,162,0
#define COLOR_CYAN      0,168,168
#define COLOR_ORANGE    255,115,0

const int OTHER = 0;
const int HOME_SCREEN = 1;
const int TUNER = 2;
const int HDMI = 3;
const int COMPOSITE_VIDEO = 4;
const int S_VIDEO = 5;
const int COMPONENT_VIDEO = 6;
const int DVI = 7;
const int AIRPLAY = 8;
const int USB = 9;
const int APPLICATION = 10;

LGRS232 tv;
NeoBlinker *rgbLED;

bool       wifiConnected = false;
bool       wifiConnecting = false;
bool       needsPairing = false;

struct HKTV : Service::Television { 

  SpanCharacteristic *active;
  SpanCharacteristic *activeIdentifier;
  SpanCharacteristic *configuredName;
  SpanCharacteristic *remoteKey;
  SpanCharacteristic *sleepDiscoveryMode;
  
  SpanService *speaker;
  SpanCharacteristic *mute;
  SpanCharacteristic *speakerActive;
  SpanCharacteristic *volume;
  
  SpanService         *hdmi1;
  SpanCharacteristic  *hdmi1Identifier;
  SpanCharacteristic  *hdmi1TargetVisibility;
  SpanCharacteristic  *hdmi1CurrentVisibility;

  SpanService         *hdmi2;
  SpanCharacteristic  *hdmi2Identifier;
  SpanCharacteristic  *hdmi2TargetVisibility;
  SpanCharacteristic  *hdmi2CurrentVisibility;

  SpanService         *hdmi3;
  SpanCharacteristic  *hdmi3Identifier;
  SpanCharacteristic  *hdmi3TargetVisibility;
  SpanCharacteristic  *hdmi3CurrentVisibility;

  SpanService         *hdmi4;
  SpanCharacteristic  *hdmi4Identifier;
  SpanCharacteristic  *hdmi4TargetVisibility;
  SpanCharacteristic  *hdmi4CurrentVisibility;
  
  LGRS232             *tvControl;
  unsigned long       lastUpdate;
  

  HKTV(LGRS232 *lgtv) : Service::Television(){       // constructor() method
    Serial.print("Init HKTV\n");           // initialization message

    this->tvControl = lgtv;
    
    active             = new Characteristic::Active();
    activeIdentifier   = new Characteristic::ActiveIdentifier(1);
    configuredName     = new Characteristic::ConfiguredName("Television");
    remoteKey          = new Characteristic::RemoteKey();
    sleepDiscoveryMode = new Characteristic::SleepDiscoveryMode(1);
    
    speaker = new Service::TelevisionSpeaker();
      mute = new Characteristic::Mute();
      speakerActive = new Characteristic::Active();
      volume = new Characteristic::Volume();
      volume->setRange(0, 100, 1);
      new Characteristic::VolumeControlType(3);
      new Characteristic::VolumeSelector();

    hdmi1 = new Service::InputSource();
      new Characteristic::ConfiguredName("HDMI 1");
      new Characteristic::IsConfigured(1); // configured = 1, not configured = 0
      new Characteristic::InputSourceType(HDMI);
      new Characteristic::Name("HDMI 1");
      hdmi1Identifier = new Characteristic::Identifier(144);
      hdmi1CurrentVisibility = new Characteristic::CurrentVisibilityState(0);  // 0 shown, 1 hidden
      hdmi1TargetVisibility = new Characteristic::TargetVisibilityState(0);  // 0 shown, 1 hidden

    hdmi2 = new Service::InputSource();
      new Characteristic::ConfiguredName("HDMI 2");
      new Characteristic::IsConfigured(1); // configured = 1, not configured = 0
      new Characteristic::InputSourceType(HDMI);
      new Characteristic::Name("HDMI 2");
      hdmi2Identifier = new Characteristic::Identifier(145);
      hdmi2CurrentVisibility = new Characteristic::CurrentVisibilityState(0);  // 0 shown, 1 hidden
      hdmi2TargetVisibility = new Characteristic::TargetVisibilityState(0);  // 0 shown, 1 hidden
      
    hdmi3 = new Service::InputSource();
      new Characteristic::ConfiguredName("HDMI 3");
      new Characteristic::IsConfigured(1); // configured = 1, not configured = 0
      new Characteristic::InputSourceType(HDMI);
      new Characteristic::Name("HDMI 3");
      hdmi3Identifier = new Characteristic::Identifier(146);
      hdmi3CurrentVisibility = new Characteristic::CurrentVisibilityState(0);  // 0 shown, 1 hidden
      hdmi3TargetVisibility = new Characteristic::TargetVisibilityState(0);  // 0 shown, 1 hidden

    hdmi4 = new Service::InputSource();
      new Characteristic::ConfiguredName("HDMI 4");
      new Characteristic::IsConfigured(1); // configured = 1, not configured = 0
      new Characteristic::InputSourceType(HDMI);
      new Characteristic::Name("HDMI 4");
      hdmi4Identifier = new Characteristic::Identifier(147);
      hdmi4CurrentVisibility = new Characteristic::CurrentVisibilityState(0);  // 0 shown, 1 hidden
      hdmi4TargetVisibility = new Characteristic::TargetVisibilityState(0);  // 0 shown, 1 hidden
    

    addLink(speaker);
    addLink(hdmi1);
    addLink(hdmi2);
    addLink(hdmi3);
    addLink(hdmi4);
  }

  boolean update(){
    
    if(active->updated() && active->getVal() != active->getNewVal()) {
      Serial.printf("update(): active %d -> %d\n", active->getVal(), active->getNewVal());
      tvControl->setPower(active->getNewVal());
      lastUpdate = millis() + 4500; // turning on the tv takes time, so wait 5 seconds before updating in loop()
    }
    
    if(volume->updated() && volume->getVal() != volume->getNewVal()) {
      Serial.printf("update(): volume %d -> %d\n", volume->getVal(), volume->getNewVal());
      tvControl->setVolume(active->getNewVal());
      lastUpdate = 0; // force an update in loop()
    }

    if(mute->updated() && mute->getVal() != mute->getNewVal()) {
      Serial.printf("update(): mute %d -> %d\n", mute->getVal(), mute->getNewVal());
      tvControl->setMute(mute->getNewVal());
      lastUpdate = 0; // force an update in loop()
    }

    if(speakerActive->updated()) {
      Serial.printf("update(): speakerActive %d -> %d\n", speakerActive->getVal(), speakerActive->getNewVal());
      lastUpdate = 0; // force an update in loop()
    }

    if(activeIdentifier->updated() && activeIdentifier->getVal() != activeIdentifier->getNewVal()) {
      Serial.printf("update(): activeIdentifier %d -> %d\n", activeIdentifier->getVal(), activeIdentifier->getNewVal());
      tvControl->setInput(activeIdentifier->getNewVal());
      lastUpdate = 0; // force an update in loop()
    }

    if(configuredName->updated()) {
      Serial.printf("update(): configuredName %s->%s\n", configuredName->getString(), configuredName->getNewString());
      lastUpdate = 0; // force an update in loop()
    }

    return true;
  }

  void loop() {
    bool b;
    int i;
    if(millis() - lastUpdate > REFRESH_INTERVAL) {
      
      // update power
      b = tvControl->getPower();
      if(b != 0xFF && (b == 0x01) != active->getVal()) {
        active->setVal(b == 0x01);
      }

      // update mute
      b = tvControl->getMute();
      if(b != 0xFF && (b == 0x00) != mute->getVal()) {
        mute->setVal(b == 0x00);
      }

      // update volume
      i = tvControl->getVolume();
      if(i != 0xFF && i != volume->getVal()) {
        volume->setVal(i);
      }

      // update input
      i = tvControl->getInput();
      if(i != 0xFF && i != activeIdentifier->getVal()) {
        activeIdentifier->setVal(i);
      }

      lastUpdate = millis();
      
    }
  }
};

static void homeSpanEventHandler(int32_t event_id) {

  // Serial.printf("GOT EVENT %d\n", event_id);

  switch(event_id) {
    case HOMESPAN_WIFI_CONNECTING:
      rgbLED->setColor(COLOR_ORANGE);
      if(!wifiConnecting) {
        rgbLED->start(250);
        wifiConnecting = true;
      }
    break;

    case HOMESPAN_WIFI_CONNECTED:
      wifiConnected = true;
      wifiConnecting = false;
      if(needsPairing) {
        rgbLED->setColor(COLOR_MAGENTA);
        rgbLED->startPulse(2);
      } else {
        rgbLED->setColor(COLOR_GREEN);
        rgbLED->setBrightness(10);
        rgbLED->on();
      }
    break;

    case HOMESPAN_WIFI_DISCONNECTED:
      wifiConnected = false;
      wifiConnecting = false;
      rgbLED->setColor(COLOR_RED);
      rgbLED->on();
    break;

    case HOMESPAN_PAIRING_NEEDED:
      needsPairing = true;
      rgbLED->setColor(COLOR_MAGENTA);
      rgbLED->startPulse(2);
    break;

    case HOMESPAN_PAIRED:
      needsPairing = false;
    break;

    case HOMESPAN_WIFI_NEEDED:
      wifiConnected = false;
      rgbLED->setColor(COLOR_ORANGE);
      rgbLED->startPulse(2);
    break;

    case HOMESPAN_OTA_STARTED:
    break;

    case HOMESPAN_AP_STARTED:
    break;

    case HOMESPAN_AP_CONNECTED:
    break;
  }
}


char sn[32];
char tvName[64];

void setup() {
  memset(sn, 0, 32);
  memset(tvName, 0, 64);
  
  Serial.begin(115200); 

  rgbLED = new NeoBlinker(LED_PIN);
  rgbLED->off();
  rgbLED->stop();

  sprintf(sn, "%08X", (uint32_t)ESP.getEfuseMac());
  sprintf(tvName, "LGTV %s", sn);

  tv.connect(&Serial2, TV_ID, RX_PIN, TX_PIN);
  
  homeSpan.setControlPin(BTN_PIN);
  homeSpan.addEventCallback(homeSpanEventHandler);
  homeSpan.begin(Category::Television,"LGTV", "LGTV-HK", "LGTV-HK");

  
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Name(tvName);
      new Characteristic::Manufacturer("LG");
      new Characteristic::SerialNumber(sn);
      new Characteristic::Model("LGTV-HK");
      new Characteristic::FirmwareRevision("0.1");
      new Characteristic::Identify();
      
    new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      
  new HKTV(&tv);
}

void loop() {
  // put your main code here, to run repeatedly:
  homeSpan.poll();
}
