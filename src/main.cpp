#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "苏仙女的魔法信号"
#define wifi_password "123456qq"

#define mqtt_server "192.168.31.130"
#define mqtt_port 1883
#define mqtt_user "admin"
#define mqtt_password "123456qq"

// 主题
// 开关灯主题
// const char *in_topic = "/esp01/light/in";
// const char *out_topic = "/esp01/light/out";

// 设置灯的亮度
const char *dimmable_set_topic = "/esp01/light/dimmable/set";
// 发送灯的亮度
const char *dimmable_get_topic = "/esp01/light/dimmable/get";
// Replace by 2 if you aren't enable to use Serial Monitor... Don't forget to Rewire R1 to GPIO2!
#define in_led 3

WiFiClient espClient;
PubSubClient client;

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //GPIO 3 (RX) swap the pin to a GPIO.
  // pinMode(in_led, FUNCTION_3);
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      // 连接成功之后，开启关闭重复两次
      digitalWrite(in_led, HIGH);
      delay(500);
      digitalWrite(in_led, LOW);
      delay(500);
      digitalWrite(in_led, HIGH);
      delay(500);
      digitalWrite(in_led, LOW);
      delay(500);
      digitalWrite(in_led, HIGH);

      client.publish(dimmable_get_topic, "255");
      // 订阅消息
      client.subscribe(dimmable_set_topic);
      delay(1000);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/**
 * @description: 
 * @param {char} *topic 消息到达的主题
 * @param {byte} *payload 消息负载
 * @param {int} length payload的长度
 * @return {*}
 */
void callback(char *topic, byte *payload, int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  // concat the payload into a string
  char s_payload[length];
  for (int i = 0; i < length; i++)
  {
    s_payload[i] = (char)payload[i];
  }

  // 如果主题是亮度调节
  if (String(dimmable_set_topic).equals(topic))
  {
    // 将char转成int类型
    int val = atoi(s_payload);
    Serial.println(val);
    analogWrite(in_led, val);
  }
  
  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setClient(espClient);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(in_led, OUTPUT);
  // 点亮LED灯
  analogWrite(in_led, 255);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
