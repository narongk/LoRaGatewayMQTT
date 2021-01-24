
const char*   WIFI_SSID       = "home2G_8225";
const char*   WIFI_PASSWORD   = "tao@45231538";

// MQTT topic gets defined by "<MQTT_BASE_TOPIC>/<MAC_ADDRESS>/<property>"
// where MAC_ADDRESS is one of the values from FLORA_DEVICES array
// property is either temperature, moisture, conductivity, light or battery

const char*   MQTT_HOST       = "192.168.1.50";
const int     MQTT_PORT       = 1883;
const char*   MQTT_CLIENTID   = "LoRaGateWay";
const char*   MQTT_USERNAME   = "narongk";
const char*   MQTT_PASSWORD   = "tao@45231538";
const char*  GW_STATE_TOPIC = "tele/LoRaGateWay/STATE";
const String  C_CMD_TOPIC = "cmnd/";
const String  C_STAT_TOPIC = "stat/";
const String  C_BASE_TOPIC = "tele/";
const int     MQTT_RETRY_WAIT = 15*1000;
