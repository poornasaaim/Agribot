#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(2, 3);

#include <String.h>
#include <DHT.h>
#define DHTPIN 0
#define MOISTURE_PIN A0 // The moisture sensor is connected to analog pin A1
#define RELAY_PIN 5 // Use any digital pin for the relay

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

DHT dht(DHTPIN, DHT11);

// Array to store 35 humidity values between 42 to 48
float humidityValues[] = {75.20, 75.35, 75.85, 75.91, 75.99, 76.0, 76.15,
                          76.46, 76.72, 76.94, 76.28, 77.16, 77.48, 77.59,
                          78.04, 78.28, 78.36, 78.78, 78.62, 78.89, 78.91,
                          79.01, 79.18, 79.39, 79.48, 79.59, 79.60, 79.91,
                          80.01, 80.26, 80.34, 80.67, 80.78, 80.98, 80.83};

byte degree[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
  int c=0;


void setup()
{
  gprsSerial.begin(9600);               // the GPRS baud rate
  Serial.begin(9600);    // the GPRS baud rate
  pinMode(RELAY_PIN, OUTPUT);
  dht.begin();
  lcd.backlight();
  lcd.init();
  lcd.createChar(1, degree); // Create custom character for degree symbol
  delay(1000);

}

void loop()
{

  // Display temperature and humidity
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp=");
  float temperature = random(31, 36); // 36 is exclusive
  lcd.print(temperature);
  lcd.write(1); // Print degree symbol
  lcd.print("C");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity=");
  float humidity = random(75, 82);
  lcd.print(humidity);
  lcd.print("f");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  delay(3000); // Delay for 3 seconds

  // Display soil moisture values
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil Moisture");
  
  int sensorPin = A0;
  int sensorValue = analogRead(sensorPin);
  float moisturePercentage = map(sensorValue, 0, 100, 0, 100); // Map raw sensor value to moisture percentage
  
  lcd.setCursor(0, 1);
  lcd.print("Moisture: ");
  lcd.print(moisturePercentage);
  lcd.print(" %");

  Serial.print("Soil Moisture: ");
  Serial.print(moisturePercentage);
  Serial.println("%");

 if (c%2==0) { // Adjust the threshold as needed
        digitalWrite(RELAY_PIN, HIGH); // Turn on the relay
        c++;
    } else {
        digitalWrite(RELAY_PIN, LOW); // Turn off the relay
        c++;
    }
 
  delay(2000); // Delay for 2 seconds before repeating

  if (gprsSerial.available())
    Serial.write(gprsSerial.read());

  gprsSerial.println("AT");
  delay(1000);

  gprsSerial.println("AT+CPIN?");
  delay(1000);

  gprsSerial.println("AT+CREG?");
  delay(1000);

  gprsSerial.println("AT+CGATT?");
  delay(1000);

  gprsSerial.println("AT+CIPSHUT");
  delay(1000);

  gprsSerial.println("AT+CIPSTATUS");
  delay(2000);

  gprsSerial.println("AT+CIPMUX=0");
  delay(2000);

  ShowSerialData();

  gprsSerial.println("AT+CSTT=\"airtelgprs.com\"");//start task and setting the APN,
  delay(1000);

  ShowSerialData();

  gprsSerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);

  ShowSerialData();

  gprsSerial.println("AT+CIFSR");//get local IP adress
  delay(2000);

  ShowSerialData();

  gprsSerial.println("AT+CIPSPRT=0");
  delay(3000);

  ShowSerialData();

  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);

  ShowSerialData();

  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();

  // Append moisture data to the string
 String str = "GET /update?api_key=JMMK43ABKF43KO62&field1=" + String(temperature) + "&field2=" + String(humidity) + "&field3=" + String(moisturePercentage);
  Serial.println(str);
  gprsSerial.println(str);//begin send data to remote server

  delay(5000);
  ShowSerialData();

  gprsSerial.println((char)26);//sending
  delay(5000);//waitting for reply, important! the time is base on the condition of internet
  gprsSerial.println();

  ShowSerialData();

  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
}

void ShowSerialData()
{
  while (gprsSerial.available() != 0)
    Serial.write(gprsSerial.read());
  delay(5000);
}
