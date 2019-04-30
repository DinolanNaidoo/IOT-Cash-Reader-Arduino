const int coinpin = 2;
const int ledpin = 13;

// Variables
volatile int pulse = 0;
double credit = 0.0;

unsigned long currentT = 0; //When last action was made
unsigned long prevT = 0; //When last pulse was send
unsigned long timeout= 0; //timeout for waiting for the next pulse before sending coin to api

boolean bInserted = false;
boolean showtimer = false;

//bytes transmitting pins
const byte numPins = 5;
byte pins[] = { 12, 11, 10, 9,8};

// Setup
void setup() {

Serial.begin(9600);
//attachInterrupt(digitalPinToInterrupt(coinpin), coinInterrupt, RISING);
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

    showtimer = true;
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

if(Serial.available()){
int value = Serial.read()- 48;
if(value > 0){
DetermineCoin(value);
}
}
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

//inform user
Serial.println("R"+String(coinCredit)+" Sent to Api");
}
