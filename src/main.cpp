#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "苏仙女的魔法信号"
#define wifi_password "123456qq"

#define mqtt_server "192.168.31.130"
#define mqtt_port 1883
#define mqtt_user "admin"
#define mqtt_password "123456qq"

#define in_topic "/esp01/light/in"
#define out_topic "/esp01/light/out"
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

      client.publish(out_topic, "1");
      // 订阅消息
      client.subscribe(in_topic);
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

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
    if (receivedChar == '0')
      digitalWrite(in_led, LOW);
    if (receivedChar == '1')
      digitalWrite(in_led, HIGH);
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
  digitalWrite(in_led, HIGH);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
