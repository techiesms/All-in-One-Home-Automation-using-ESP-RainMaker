/*

   This is the code for the project,

   All in One Home Automation project using ESP RainMaker made using ESP32

   This code was written by Arun Suthar Modified by Sachin Soni for techiesms YouTube channel

   This code is provided free for project purpose and fair use only.
   Do mail us if you want to use it commercially

   techiesms@gmail.com
   Copyrighted © by techiesms


   Watch out it's complete tutorial on our YouTube channel
   https://www.YouTube.com/techiesms
*/


//This example demonstrates the ESP RainMaker with a standard Switch device.
#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <AceButton.h>
#include <wifi_provisioning/manager.h>
#include <esp_heap_caps.h>

//for BLE
const char *service_name = "PROV_techiesms"; //name of node in BLE
const char *pop = "Techiesms123"; //password

using namespace ace_button;

#define DEBUG_SW 1 // for ON/OFF serial moniter
#define DEFAULT_RELAY_STATE false

// GPIO for push button
static uint8_t gpio_reset = 0;

// GPIO for switch
static uint8_t switch1 = 32;
static uint8_t switch2 = 35;
static uint8_t switch3 = 34;
static uint8_t switch4 = 39;

// GPIO for Relay (Appliance Control)
static uint8_t relay1 = 15;
static uint8_t relay2 = 2;
static uint8_t relay3 = 4;
static uint8_t relay4 = 22;

// GPIO for Relay (Fan Speed Control)
static uint8_t Speed1 = 21;
static uint8_t Speed2 = 19;
static uint8_t Speed4 = 18;

// GPIO for Fan Regulator Knob
static uint8_t s1 = 27;
static uint8_t s2 = 14;
static uint8_t s3 = 12;
static uint8_t s4 = 13;

bool speed1_flag = 1;
bool speed2_flag = 1;
bool speed3_flag = 1;
bool speed4_flag = 1;
bool speed0_flag = 1;

/* Variable for reading pin status*/
bool switch_state_ch1 = false;
bool switch_state_ch2 = false;
bool switch_state_ch3 = false;
bool switch_state_ch4 = false;


int Slider_Value = 0;
int curr_speed = 1;
bool fan_power = 0;



static Switch my_switch1("light1", &relay1);
static Switch my_switch2("light2", &relay2);
static Switch my_switch3("light3", &relay3);
static Switch my_switch4("light4", &relay4);
static Fan my_fan("Fan");

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);


void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);

void sysProvEvent(arduino_event_t *sys_event)
{
  switch (sys_event->event_id)
  {
    case ARDUINO_EVENT_PROV_START:
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      printQR(service_name, pop, "ble");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("\nConnected IP address : ");
      Serial.println(IPAddress(sys_event->event_info.got_ip.ip_info.ip.addr));
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("\nDisconnected. Connecting to the AP again... ");
      break;
    case ARDUINO_EVENT_PROV_CRED_RECV:
      Serial.println("\nReceived Wi-Fi credentials");
      Serial.print("\tSSID : ");
      Serial.println((const char *) sys_event->event_info.prov_cred_recv.ssid);
      Serial.print("\tPassword : ");
      Serial.println((char const *) sys_event->event_info.prov_cred_recv.password);
      break;
    case ARDUINO_EVENT_PROV_INIT:
      wifi_prov_mgr_disable_auto_stop(10000);
      break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      Serial.println("Stopping Provisioning!!!");
      wifi_prov_mgr_stop_provisioning();
      break;
  }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();

  if (strcmp(device_name, "Fan") == 0)
  {
    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received Fan power = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      fan_power = val.val.b;
      if (fan_power) {
        if (curr_speed == 0)
        {
          speed_0();
        }
        if (curr_speed == 1)
        {
          speed_1();
        }
        if (curr_speed == 2)
        {
          speed_2();
        }
        if (curr_speed == 3)
        {
          speed_3();
        }
        if (curr_speed == 4)
        {
          speed_4();
        }
      }
      else
        speed_0();
      param->updateAndReport(val);
    }


    if (strcmp(param_name, "My_Speed") == 0)
    {
      Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
      int Slider_Value = val.val.i;
      if (Slider_Value == 1)
      {
        speed_1();
      }
      if (Slider_Value == 2)
      {
        speed_2();
      }
      if (Slider_Value == 3)
      {
        speed_3();
      }
      if (Slider_Value == 4)
      {
        speed_4();
      }
      if (Slider_Value == 0)
      {
        speed_0();
      }
      param->updateAndReport(val);
    }

  }
  if (strcmp(device_name, "light1") == 0)
  {
    Serial.printf("Switch value_1 = %s\n", val.val.b ? "true" : "false");

    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch_state_ch1 = val.val.b;
      (switch_state_ch1 == false) ? digitalWrite(relay1, LOW) : digitalWrite(relay1, HIGH);
      param->updateAndReport(val);
    }

  }
  if (strcmp(device_name, "light2") == 0) {

    Serial.printf("Switch value_2 = %s\n", val.val.b ? "true" : "false");

    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch_state_ch2 = val.val.b;
      (switch_state_ch2 == false) ? digitalWrite(relay2, LOW) : digitalWrite(relay2, HIGH);
      param->updateAndReport(val);
    }
  }
  if (strcmp(device_name, "light3") == 0) {

    Serial.printf("Switch value_3 = %s\n", val.val.b ? "true" : "false");

    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch_state_ch3 = val.val.b;
      (switch_state_ch3 == false) ? digitalWrite(relay3, LOW) : digitalWrite(relay3, HIGH);
      param->updateAndReport(val);
    }
  }
  if (strcmp(device_name, "light4") == 0) {

    Serial.printf("Switch value_4 = %s\n", val.val.b ? "true" : "false");

    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch_state_ch4 = val.val.b;
      (switch_state_ch4 == false) ? digitalWrite(relay4, LOW) : digitalWrite(relay4, HIGH);
      param->updateAndReport(val);
    }
  }
}


void setup()
{

  Serial.begin(115200);

  // Configure the input GPIOs
  pinMode(gpio_reset, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(Speed1, OUTPUT);
  pinMode(Speed2, OUTPUT);
  pinMode(Speed4, OUTPUT);

  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);


  pinMode(s1, INPUT_PULLUP);
  pinMode(s2, INPUT_PULLUP);
  pinMode(s3, INPUT_PULLUP);
  pinMode(s4, INPUT_PULLUP);

  // Write to the GPIOs the default state on booting
  digitalWrite(relay1, DEFAULT_RELAY_STATE);
  digitalWrite(relay2, DEFAULT_RELAY_STATE);
  digitalWrite(relay3, DEFAULT_RELAY_STATE);
  digitalWrite(relay4, DEFAULT_RELAY_STATE);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  config3.setEventHandler(button3Handler);
  config4.setEventHandler(button4Handler);

  button1.init(switch1);
  button2.init(switch2);
  button3.init(switch3);
  button4.init(switch4);


  Node my_node;
  my_node = RMaker.initNode("All In One");

  //Standard switch device
  my_fan.addCb(write_callback);
  Param level_param("My_Speed", "custom.param.level", value(0), PROP_FLAG_READ | PROP_FLAG_WRITE);
  level_param.addBounds(value(0), value(4), value(1));
  level_param.addUIType(ESP_RMAKER_UI_SLIDER);
  my_fan.addParam(level_param);
  my_node.addDevice(my_fan);
  my_fan.updateAndReportParam("My_Speed", 0);
  my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, fan_power);
  delay(500);

  my_switch1.addCb(write_callback);
  my_node.addDevice(my_switch1);
  my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch1);
  delay(500);

  my_switch2.addCb(write_callback);
  my_node.addDevice(my_switch2);
  my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch2);
  delay(500);

  my_switch3.addCb(write_callback);
  my_node.addDevice(my_switch3);
  my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch3);
  delay(500);

  my_switch4.addCb(write_callback);
  my_node.addDevice(my_switch4);
  my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch4);
  delay(500);

  //This is optional
  RMaker.enableOTA(OTA_USING_PARAMS);
  //If you want to enable scheduling, set time zone for your region using setTimeZone().
  //The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
  // RMaker.setTimeZone("Asia/Shanghai");
  // Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
  RMaker.enableTZService();
  RMaker.enableSchedule();

  Serial.printf("\nStarting ESP-RainMaker\n");
  RMaker.start();
  delay(2000);

  WiFi.onEvent(sysProvEvent);
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);


}


void loop()
{

  button1.check();
  button2.check();
  button3.check();
  button4.check();
  fan();


  // Read GPIO0 (external button to gpio_reset device
  if (digitalRead(gpio_reset) == LOW) {
    //Push button pressed
    Serial.printf("reset Button Pressed!\n");
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_reset) == LOW) delay(50);
    int endTime = millis();

    if ((endTime - startTime) > 5000) {
      // If key pressed for more than 5secs, reset all
      Serial.printf("reset to factory.\n");
      RMakerFactoryReset(2);
    }
  }
  delay(100);
}


//functions for defineing manual switch

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  //EspalexaDevice* d1 = espalexa.getDevice(0);
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      switch_state_ch1 = true;
      my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch1);
      digitalWrite(relay1, HIGH);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      switch_state_ch1 = false;
      my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch1);
      digitalWrite(relay1, LOW);
      break;
  }
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  //EspalexaDevice* d2 = espalexa.getDevice(1);
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      switch_state_ch2 = true;
      my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch2);
      digitalWrite(relay2, HIGH);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      switch_state_ch2 = false;
      my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch2);
      digitalWrite(relay2, LOW);
      break;
  }
}
void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT3");
  //EspalexaDevice* d3 = espalexa.getDevice(2);
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      switch_state_ch3 = true;
      my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch3);
      digitalWrite(relay3, HIGH);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      switch_state_ch3 = false;
      my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch3);
      digitalWrite(relay3, LOW);
      break;
  }
}
void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT4");
  //EspalexaDevice* d4 = espalexa.getDevice(3);
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      switch_state_ch4 = true;
      my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch4);
      digitalWrite(relay4, HIGH);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      switch_state_ch4 = false;
      my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch4);
      digitalWrite(relay4, LOW);
      break;
  }
}


//functions for defineing of speeds

void speed_0()
{
  //All Relays Off - Fan at speed 0
  if (DEBUG_SW)Serial.println("SPEED 0");
  fan_power = 0;
  my_fan.updateAndReportParam("My_Speed", 0);
  my_fan.updateAndReportParam("Power", fan_power);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);

}

void speed_1()
{
  //Speed1 Relay On - Fan at speed 1
  if (DEBUG_SW)Serial.println("SPEED 1");
  curr_speed = 1;
  fan_power = 1;
  my_fan.updateAndReportParam("My_Speed", 1);
  my_fan.updateAndReportParam("Power", fan_power);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);
  delay(1000);
  digitalWrite(Speed1, HIGH);
}

void speed_2()
{
  //Speed2 Relay On - Fan at speed 2
  if (DEBUG_SW)Serial.println("SPEED 2");
  curr_speed = 2;
  fan_power = 1;
  my_fan.updateAndReportParam("My_Speed", 2);
  my_fan.updateAndReportParam("Power", fan_power);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);
  delay(1000);
  digitalWrite(Speed2, HIGH);
}

void speed_3()
{
  //Speed1 & Speed2 Relays On - Fan at speed 3
  if (DEBUG_SW)Serial.println("SPEED 3");
  curr_speed = 3;
  fan_power = 1;
  my_fan.updateAndReportParam("My_Speed", 3);
  my_fan.updateAndReportParam("Power", fan_power);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);
  delay(1000);
  digitalWrite(Speed1, HIGH);
  digitalWrite(Speed2, HIGH);

}

void speed_4()
{
  //Speed4 Relay On - Fan at speed 4
  if (DEBUG_SW)Serial.println("SPEED 4");
  curr_speed = 4;
  fan_power = 1;
  my_fan.updateAndReportParam("My_Speed", 4);
  my_fan.updateAndReportParam("Power", fan_power);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);
  delay(1000);
  digitalWrite(Speed4, HIGH);
}


// function for controling fan using regulator

void fan()
{
  if (digitalRead(s1) == LOW && speed1_flag == 1)
  {
    speed_1();
    speed1_flag = 0;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 1;
    speed0_flag = 1;


  }
  if (digitalRead(s2) == LOW && digitalRead(s3) == HIGH && speed2_flag == 1)
  {
    speed_2();
    speed1_flag = 1;
    speed2_flag = 0;
    speed3_flag = 1;
    speed4_flag = 1;
    speed0_flag = 1;

  }
  if (digitalRead(s2) == LOW && digitalRead(s3) == LOW && speed3_flag == 1)
  {
    speed_3();
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 0;
    speed4_flag = 1;
    speed0_flag = 1;
  }
  if (digitalRead(s4) == LOW  && speed4_flag == 1)
  {
    speed_4();
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 0;
    speed0_flag = 1;
  }
  if (digitalRead(s1) == HIGH && digitalRead(s2) == HIGH && digitalRead(s3) == HIGH && digitalRead(s4) == HIGH  && speed0_flag == 1)
  {
    speed_0();
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 1;
    speed0_flag = 0;
  }

}
