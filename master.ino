//MASTER-TEST R4 WIFI - to slave R4 WIFI
// MASTER ON COM 9
//RUN THE SLAVE FIRST AND THEN RUN THE MASTER in order the MASTER to find the SLAVE

#include <ArduinoBLE.h>
#include <Adafruit_AS7341.h>
//#include <SoftwareSerial.h> // used in photoresistor2 sketch 

Adafruit_AS7341 as7341;
float fmEDIs[8];
float m_EDI = 0; // just a default value
float delta = 10; // just a default value e.g. 50 of δ dimming value
float C_per =0; //Percentage value for Cold LED
float W_per =0;  //Percentage value for Warm LED
int phRespin = 0; // Photoresistor at Arduino analog pin A0
float phResvalue;	// Store value from photoresistor (0-1023)

const char* peripheralName = "MASTERR4"; //publish itself as "MASTERR4" 

void setup() {

//SETUP για το AS7341 spectral colour sensor
  Serial.begin(9600);
  
  //Wait for communication with the host computer serial monitor
  while (!Serial) {                   
    delay(1);
  }
 if (!as7341.begin()){
    Serial.println("Could not find AS7341"); 
    while (1) { delay(10); }
  }
  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);


Serial.begin(9600); 
delay(2000);
Serial.println("Hi from Master"); 
while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("MASTER: starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  Serial.println("MASTER: Bluetooth® Low Energy Central");
// start scanning for peripherals

BLE.setLocalName(peripheralName);

 BLE.scan();

 pinMode(phRespin, INPUT);// Set photoresistor A0 pin as an input (optional)

}// END of setup()
// *************************************************************************


// m-EDI value calculation
// based on AS7341 readings calculates m-EDI for each spectral reading
void m_EDIcalc(float fcounts, float typediv, int j){
float firradiance, filluminance, fEML, fmEDI;
firradiance = (fcounts / typediv) * 0.01;
filluminance = firradiance *118;
fEML= filluminance * 0.60;
fmEDI= fEML * 0.9058;
fmEDIs[j]=fmEDI;
//Serial.print("fmEDI= "); Serial.println(fmEDI);
//Serial.print("fmEDIs[j]= "); Serial.println(fmEDIs[j]);
}


void readAS7341(){
// Read the AS7341 spectral colour sensor
// Read all channels at the same time and store in as7341 object
// Calculates the final overall -as an average- m-EDI
float fcounts,typediv; 
int i=0, k=0;
if (!as7341.readAllChannels()){
    Serial.println("Error reading all channels!"); //or Serial1 ?
    return; 
  }
// Print out the stored values for each channel
//Serial.print("F1 415nm : "); 
//Serial.println(as7341.getChannel(AS7341_CHANNEL_415nm_F1)); 
fcounts= as7341.getChannel(AS7341_CHANNEL_415nm_F1);
typediv=0.51;
m_EDIcalc(fcounts,typediv,i);
i++;
//Serial.print("F1 445nm : "); //or Serial1 ?
//Serial.println(as7341.getChannel(AS7341_CHANNEL_445nm_F2)); //or Serial1 ? 
fcounts= as7341.getChannel(AS7341_CHANNEL_445nm_F2);
typediv=1.02;
m_EDIcalc(fcounts,typediv,i);
i++;
fcounts= as7341.getChannel(AS7341_CHANNEL_480nm_F3);
typediv=1.95;
m_EDIcalc(fcounts,typediv,i);
i++;
fcounts= as7341.getChannel(AS7341_CHANNEL_515nm_F4);
typediv=3.62;
m_EDIcalc(fcounts,typediv,i);
i++;
fcounts= as7341.getChannel(AS7341_CHANNEL_555nm_F5);
typediv=5.48;
m_EDIcalc(fcounts,typediv,i);
i++;
fcounts= as7341.getChannel(AS7341_CHANNEL_590nm_F6);
typediv=7.80;
m_EDIcalc(fcounts,typediv,i);
i++;
fcounts= as7341.getChannel(AS7341_CHANNEL_630nm_F7);
typediv=12.54;
m_EDIcalc(fcounts,typediv,i);
i++;
fcounts= as7341.getChannel(AS7341_CHANNEL_680nm_F8);
typediv=9.94;
m_EDIcalc(fcounts,typediv,i);
Serial.println("");
for (k=0;k<=7;k++) {m_EDI =m_EDI+fmEDIs[k];} m_EDI=m_EDI/(i+1); Serial.print("m_EDI= "); Serial.println(m_EDI);
}



void CWcalc(){
// C&W percemtage values calculation
float c=0; //cool value
float w=0; //warm value
float cd=0; //cool value *delta
float wd=0; //warm value *delta
if (m_EDI<250) //NIGHT, increase warm white LED value,  w/c=delta, w=(delta*100)/(1+delta), c=100-w
 { 
  w=(delta/10*100)/(1+delta/10); //warm white value
  c=100-w; //cool white value
  wd=(w*delta)/100; //warm value *delta
  W_per=wd;   //warm percentage value, for delta=50% is 41.7, analogWrite(106.3)
  cd=(c*delta)/100; //cool value *delta
  C_per=cd;  //cool percentage value, for delta=50% is 8.3, analogWrite(21.3)
  Serial.println("For m_EDI<250 : ");
  Serial.print("W_per: "); Serial.println(W_per);
   Serial.print("C_per: "); Serial.println(C_per);
 } 
 else //DAY, increase cool white LED value, c/w=delta/10, c=(delta/10*100)/(1+delta/10), w=100-c
 { 
  c=(delta/10*100)/(1+delta/10); //cool white value
  w=100-c; //warm white value
  cd=(c*delta)/100; //cool value *delta
  C_per=cd; //cool percentage value, for delta=50% is 41.7, analogWrite(106.3)
  wd=(w*delta)/100; //warm value *delta
  W_per=wd; //warm percentage value, for delta=50% is 8.3, analogWrite(21.3)
  Serial.println("For m_EDI>=250 : ");
  Serial.print("C_per: "); Serial.print(C_per); Serial.println("%");
  Serial.print("W_per: "); Serial.print(W_per); Serial.println("%");
 }
}

// Read the photosensor and define delta dimming value
void readphRes()
{
  //photoresistor:  A low value will occur when the sensor is in darkness, while a high value will occur when it is well lit.
  phResvalue = analogRead(phRespin); //read the light level
  Serial.print("The light intensity in the outdoor space is: "); Serial.println(phResvalue); 
  phResvalue=map(phResvalue,0,1023,0,100); //adjust the value 0 to 1023  to  0 to 100
  phResvalue=constrain(phResvalue,0,100); //make sure the value is between 0 and 100
  delta = phResvalue;
   Serial.print("delta is: "); Serial.println(delta);
}
//****************************************


void loop() {

// *************************************************************************
//Establish bluetooth communication between the two R4s
// check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.println("Peripheral Found ");
    Serial.print("Peripheral's address: ");
    Serial.println(peripheral.address());
    Serial.print("Peripheral's name: ");
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.println();
    Serial.print("Peripheral's advertisedServiceUuid: ");
    Serial.println(peripheral.advertisedServiceUuid());
    //Serial.println();
  
    // check for peripheral's name
  // In order to find the SLAVER4 it must be started before MASTERR4 
  if (peripheral.localName() != "SLAVER4") 
     { return;}

  Serial.println("Found SLAVER4");
  // stop scanning
  BLE.stopScan();
  
 controlslave(peripheral);

  // peripheral disconnected, start scanning again
 //BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
 BLE.scan();
}
}
// END of loop()
// *************************************************************************


void controlslave(BLEDevice peripheral){
  // connect to the peripheral
  Serial.println("MASTER: Connecting ...");
  if (peripheral.connect()) {
    Serial.println("MASTER: Connected to slave");
   }
  else {
    Serial.println("MASTER: Failed to connect!");
    return;
  }

// discover peripheral attributes
  //Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
          //Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic CLcharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");
  BLECharacteristic WLcharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1215");

 
int cycles=1; // just for testing purposes
while (peripheral.connected()) { // Check if connection was successful
    while(cycles<=2) // for testing purposes, e.g. 2 cycles to adjust the LEDs 1 times - for n cycles the LEDs run n-1 times
    {
    // while the peripheral is connected
    readphRes(); // Read the photosensor and define delta dimming value
    readAS7341(); // Read the AS7341 spectral sensor and calculate m-EDI
    CWcalc(); // Based on an average m-EDI, calculate Cool white and Warm white LEDs percentage values
    // Send C & W perecentage values to slave R4 via bluetooth
    Serial.println("Now writing to slave's CL characteristic the cool white led CL value");
    //CLcharacteristic.writeValue((byte)0x01); // Sends COLD percemtage value C_per to the slave
    CLcharacteristic.writeValue((byte)C_per);
    //slaveCLCharacteristic.writeValue((byte)0x01);
    Serial.println("Now writing to slave's WL characteristic the warm white led WL value");
    //WLcharacteristic.writeValue((byte)0x03); // Sends WARM percemtage value W_per to the slave
    WLcharacteristic.writeValue((byte)W_per);
    //slaveCLCharacteristic.writeValue((byte)0x02);
    cycles++; // for testing purposes
    //delay(15000);
    }  // END of while(cycles<= ...) for testing purposes
    while(1); // do nothing else for now
       
  Serial.println("Peripheral -slave- disconnected");
}
}// END controlslave()



// *************************************************************************
void exploreService(BLEService service) {
  // print the UUID of the service
  Serial.print("Service ");
  Serial.println(service.uuid());
  // loop the characteristics of the service and explore each
  for (int i = 0; i < service.characteristicCount(); i++) {
    BLECharacteristic characteristic = service.characteristic(i);
    exploreCharacteristic(characteristic);
  }
}

void exploreCharacteristic(BLECharacteristic characteristic) {
  // print the UUID and properties of the characteristic
  Serial.print("\tCharacteristic ");
  Serial.print(characteristic.uuid());
  Serial.print(", properties 0x");
  Serial.print(characteristic.properties(), HEX);
  // check if the characteristic is readable
  if (characteristic.canRead()) {
    // read the characteristic value
    characteristic.read();
    if (characteristic.valueLength() > 0) {
      // print out the value of the characteristic
      Serial.print(", value 0x");
      printData(characteristic.value(), characteristic.valueLength());
    }
  }
  Serial.println();
  // loop the descriptors of the characteristic and explore each
  for (int i = 0; i < characteristic.descriptorCount(); i++) {
    BLEDescriptor descriptor = characteristic.descriptor(i);
    exploreDescriptor(descriptor);
  }
}

void exploreDescriptor(BLEDescriptor descriptor) {
  // print the UUID of the descriptor
  Serial.print("\t\tDescriptor ");
  Serial.print(descriptor.uuid());
  // read the descriptor value
  descriptor.read();
  // print out the value of the descriptor
  Serial.print(", value 0x");
  printData(descriptor.value(), descriptor.valueLength());
  Serial.println();
}

void printData(const unsigned char data[], int length) {
  for (int i = 0; i < length; i++) {
    unsigned char b = data[i];
    if (b < 16) {
      Serial.print("0");
    }
    Serial.print(b, HEX);
  }
}
// *************************************************************************