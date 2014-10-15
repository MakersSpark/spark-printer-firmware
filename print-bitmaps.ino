TCPClient client;

void setup()
{
  // Make sure your Serial Terminal app is closed before powering your Core
  Serial.begin(9600);
  // Now open your Serial Terminal, and hit any key to continue!
  while(!Serial.available()) SPARK_WLAN_Loop();

  if (client.connect("pastebin.com", 80))
  {
    client.println("GET /raw.php?i=KQGjb0Wv HTTP/1.0");
    client.println("Host: pastebin.com");
    client.println("Content-Length: 0");
    client.println();
  }
  else
  {
    Serial.println("connection failed");
  }
}

void loop()
{
  String fileRead = "";
  
  while(client.available())
  {
    char c = client.read();
    fileRead += c;
  }
  
  Serial.println(fileRead);

  if (!client.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;);
  }
  
  
}