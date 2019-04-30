  /*
  Coin Detect Test For IOT Project
*/

//networking
#include <SoftwareSerial.h>
#include <Wire.h>
SoftwareSerial mySerial(8, 9);            // RX, TX Pins
String apn = "apn";                       //APN
String apn_u = "ppp";                     //APN-Username
String apn_p = "ppp";                     //APN-Password
String url = "http://5cc155810e53350014908c4e.mockapi.io/api/coinvalue/coin";  //URL for HTTP-POST-REQUEST

//Constants
const int coinpin = 2;
const int ledpin = 13;

// Variables
volatile int pulse = 0;
double credit = 0.0;
 
unsigned long currentT = 0;   //When last action was made
unsigned long prevT = 0;    //When last pulse was send
unsigned long timeout= 0;   //timeout for waiting for the next pulse before sending coin to api

boolean bInserted = false;


// Setup
void setup() {
   // Open serial communications and wait for port to open:
   Serial.begin(9600);
   // set the data rate for the SoftwareSerial port
   mySerial.begin(9600);
   
   attachInterrupt(digitalPinToInterrupt(coinpin), coinInterrupt, RISING);
   prevT = millis();
   pulse = 0;
   //set led pin as output
   pinMode(ledpin, OUTPUT);
   Serial.println("Ready to Accept coins");
}

// Main loop
void loop() {
  // critical, time-sensitive code here
   if( bInserted ){ // for show LED ON when Inserted
       bInserted = false;
      digitalWrite(ledpin, HIGH);
      //delay(1000);
   }else{

      // Turn off LED
      digitalWrite(ledpin, LOW);
   }
   if(millis() - currentT == 150 && pulse > 0){
    Serial.println("sending credit To Api...");
    DetermineCoin(pulse);
    delay(1);
   }

  ////  int value = Serial.read()- 48;
   // if(value > 0){
    //   DetermineCoin(value);
   // }
   //}
}

// Interrupt
void coinInterrupt(){
  // Each time a pulse is sent from the coin acceptor,
  // interrupt main loop to add 1 and flip on the LED
   
   bInserted = true;
   prevT = currentT;
   currentT = millis();
   digitalWrite(ledpin, HIGH);
   if((currentT - prevT) < 150 || (currentT - prevT == currentT)){// if difference bettween last pulse and new pulse is greater than 130millis then its a new coin,//but user can insert faster than 150 millis
     pulse++; //increment current coin pulse number                    
   }else{
     pulse=1; //new coin with first pulse
   }
}
void DetermineCoin(const int pulseVal){
  double credit = pulseVal * 0.5; //account coins inserted faster than difference between pulses in milli seconds
                                           //coin inserted in normal speed >150 millis
                                       // if pulse => 1 then coin => R0.50
                                       // if pulse => 2 then coin => R1.00
                                       // if pulse => 4 then coin => R2.00
                                       // if pulse => 10 then coin => R5.00
                                          //coin inserted in faster speed <=150 millis 
                                       // if pulse => 1,2 =>3 then coin credit => R1.50
                                       // if pulse => 2,4 =>6 then coin credit => R2.00
                                       // if pulse => 1,10 =>11 then coin credit => R5.50
  Serial.println("R"+String(credit)+" inserted");
  //send to nxpProcessor
  sendCreditToApi(credit);
}
//send number of number of pulses to nxp processor in bytes through the 5 pins

//Method to send credit to Api
void sendCreditToApi(double coinCredit){
  //logic for sending to api
  gsm_sendhttp(coinCredit); //Start the GSM-Modul and start the transmisson
  delay(60000); //Wait one minute
  //inform user
  Serial.println("R"+String(coinCredit)+" Sent to Api");
}

//API communications
void gsm_sendhttp(const double credit) {
  mySerial.println("AT");
  runsl();//Print GSM Status an the Serial Output;
  delay(4000);
  mySerial.println("AT+SAPBR=3,1,Contype,GPRS");
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=3,1,APN," + apn);
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=3,1,USER," + apn_u); //Comment out, if you need username
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=3,1,PWD," + apn_p); //Comment out, if you need password
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR =1,1");
  runsl();
  delay(100);
  mySerial.println("AT+SAPBR=2,1");
  runsl();
  delay(2000);
  mySerial.println("AT+HTTPINIT");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPPARA=CID,1");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPPARA=URL," + url);
  runsl();
  delay(100);
  mySerial.println("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPDATA=192,10000");
  runsl();
  delay(100);
  +mySerial.println("coinValue=" + String(credit));
  runsl();
  delay(10000);
  mySerial.println("AT+HTTPACTION=1");
  runsl();
  delay(5000);
  mySerial.println("AT+HTTPREAD");
  runsl();
  delay(100);
  mySerial.println("AT+HTTPTERM");
  runsl(); 
}
//Print GSM Status
void runsl() {
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}
