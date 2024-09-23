//SLAVE-TEST R4 WIFI
// SLAVE ON COM 11
//RUN THE SLAVE FIRST - AFTER YOU START THE MASTER

//#include <SoftwareSerial.h>
#include <ArduinoBLE.h>

int C_val; //Int value for Cool White LED
int W_val; //Int value for Warm White LED 

//Initializing Pin 5 and Pin 6 for the two converter modules 5VPWM to 0V-10V
int coolconvmodpin = 5; //Pin 5 for converter module 5VPWM to 0V-10V for cool white LED
int warmconvmodpin = 6; //Pin 6 for converter module 5VPWM to 0V-10V for warm white LED

const char* peripheralName = "SLAVER4"; //publish itself as "SLAVER4" 

BLEService coldledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Create Bluetooth® Low Energy LED Service
BLEService warmledService("19B10000-E8F2-537E-4F6C-D104768A1215");
//BLEService ledService("1800"); //Αυτό δεν θα δουλέψει διότι μόνο τα Standard services have a 16-bit UUID 

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
//custom services have a 128-bit UUID
BLEByteCharacteristic coldledCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic warmledCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1215", BLERead | BLEWrite);
//BLEByteCharacteristic warmledCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);


void setup() {
Serial.begin(9600); 
delay(2000);
Serial.println("Hi from Slave"); 
//Declaring Pin 5 as output
pinMode(coolconvmodpin, OUTPUT);
pinMode(warmconvmodpin, OUTPUT);
while (!Serial);

// begin initialization
if (!BLE.begin()) {
    Serial.println("SLAVE: starting Bluetooth® Low Energy module failed!");
    while (1);
}

Serial.println("SLAVE: Bluetooth® Low Energy Central");
// set advertised local name and service UUID:
BLE.setLocalName(peripheralName); //BLE.setLocalName("SLAVER4");
BLE.setAdvertisedService(coldledService);
BLE.setAdvertisedService(warmledService);

// add the characteristic to the service
coldledService.addCharacteristic(coldledCharacteristic);
warmledService.addCharacteristic(warmledCharacteristic);
//ledService.addCharacteristic(warmledCharacteristic);

// add service
BLE.addService(coldledService);
BLE.addService(warmledService);

// set the initial value for the characteristic:
coldledCharacteristic.writeValue(0);
warmledCharacteristic.writeValue(0);

// start advertising
BLE.advertise(); 
Serial.println("Bluetooth® SLAVER4 device active, waiting for connections...");

}


void loop() {

  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

analogWrite(coolconvmodpin, 255);
analogWrite(warmconvmodpin, 255); 

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the 0-100 Cl percentage value to control the COOL White LED:
     
     if (coldledCharacteristic.written() &&  warmledCharacteristic.written()) {
        Serial.print("Cool White Led Value in percentage is: ");
        Serial.print(coldledCharacteristic.value());Serial.println("%");
        //0% being 0, 100% being 255
        C_val= (coldledCharacteristic.value()*255)/100;
        Serial.print("C_val for analogueWrite (0-255, 0-10V)is: "); Serial.println(C_val);
        //0 being 0 volts, 128 being 5V and 255 being 10V
      analogWrite(coolconvmodpin, C_val); //C_val must be between 0 and 255
      // led_testingCoolLED();
       Serial.print("Warm White Led Value in percentage is: ");
       Serial.print(warmledCharacteristic.value());Serial.println("%");
       W_val= (warmledCharacteristic.value()*255)/100;
       Serial.print("W_val for analogueWrite (0-255, 0-10V) is: "); Serial.println(W_val);
      analogWrite(warmconvmodpin, W_val); //W_val must be between 0 and 255
      // led_testingWarmLED();
     }
          
    } //END of while (central.connected()
  

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

void led_testingCoolLED()
{ 
  Serial.println("Testing Cool White LED");
  analogWrite(coolconvmodpin, 255); delay(1000);
  analogWrite(coolconvmodpin, 128); delay(1000);
  analogWrite(coolconvmodpin, 64); delay(1000);
  analogWrite(coolconvmodpin, 32); delay(1000);
  analogWrite(coolconvmodpin, 10); delay(1000);
  analogWrite(coolconvmodpin, 0); delay(1000);
 }

void led_testingWarmLED()
{   
  Serial.println("Testing Warm White LED");
  analogWrite(warmconvmodpin, 255); delay(1000);
  analogWrite(warmconvmodpin, 128); delay(1000);
  analogWrite(warmconvmodpin, 64); delay(1000);
  analogWrite(warmconvmodpin, 32); delay(1000);
  analogWrite(warmconvmodpin, 10); delay(1000);
  analogWrite(warmconvmodpin, 0); delay(1000);
 }

void led_testing2()
{
//pwm from pin 5 slowly increases, then decreases repeatedly
//For testing only: Fading the LED
for(int i=0; i<255; i++){
analogWrite(coolconvmodpin, i);
delay(15);
}
for(int i=255; i>0; i--){
analogWrite(coolconvmodpin, i);
delay(15);
}
}



