/*
 * Program made by Nicholas Finch
 * 12:00 AM 8/26/2015
 * Open Source, please keep the header here
 * This a program that will read the voltages and PPM from a 
 * helicopter RC board. The PPM is very noisey, I used smoothing
 * to alleviate some of the jitter. Increase numReadings[] 
 * to further improve jitter.
 * 
 * The program consists of a lot of functions, This was to compartmentalize
 * and reduce code in the setup and loop functions
 * This is a purely preferential approach.
 * The functions consist of foward, back, left, right, rotations: left and right, 
 * and single wheeled movement
 * 
 * I suggest changing just the motor pins if you would like to alter the inputs
 * and outputs of motors. Don't touch the motor_*() functions, These should already be 
 * configured correctly.
*/

#include <PPMrcIn.h>
#include <Statistic.h>
Channel ch1;
const int LED = 13;
const int buttonPin[] = {A0,A1,A2};
const int motor_left[] = {4, 7}; //Left Pins
const int motor_rite[] = {5, 6}; //Rite Pins
float buttonState[] = {0,0,0};
boolean debug = true;

const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


void setup(){
	debug = false; //comment or uncomment to allow for debugging.
	if (debug){Serial.begin(9600);}
	for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;   
	pinSetup();
}

void loop(){
	buttonSetup();
	if (debug){delay(200);}	
	moveBot(buttonState);
}


void moveBot(float buttonState[]){
	/*NOTE: the mid point is 125
	 * left most is 120	
	 * rightmost is 200
	 * 50 ---x---<100-----120--<140>--160----180>---x---200
	 */
//================================================================
	if (buttonState[1] <= 0.20){
		Serial.println("^");
		if (buttonState[0] > 160) {
			Serial.println(">");
			motor_forright();
		}
		else if (buttonState[0] < 120 && buttonState[0] > 10) {
			Serial.println("<");
			motor_bacleft();
		}
		else {
			motor_foward();
		}
	}
//================================================================
	else if(buttonState[2] <= 0.20){
	    Serial.println("v");
	    if (buttonState[0] < 120 && buttonState[0] > 10) {
	      Serial.println("<");
	      motor_bacright();
	    }
	    else if (buttonState[0] > 160) {
	      Serial.println(">");
	      motor_forleft();
	    }
	    else {
	      motor_backward();
	    }
	}
//================================================================
	else if ((buttonState[0] < 120) && (buttonState[0] > 10)) {
		Serial.println("<");
		digitalWrite(LED, HIGH);
		motor_roleft();
	}
//================================================================	
	else if(buttonState[0] > 160){
		Serial.println(">");
		digitalWrite(LED, HIGH);
		motor_roright();
	}
//================================================================	
	else {
		digitalWrite(LED, LOW);
		motor_stop();		
	}
}

void motor_stop() {
  digitalWrite(motor_rite[0], LOW);
  digitalWrite(motor_rite[1], LOW);
  digitalWrite(motor_left[0], LOW);
  digitalWrite(motor_left[1], LOW);
}

void motor_foward() {
  digitalWrite(motor_rite[0], HIGH);
  digitalWrite(motor_rite[1], LOW);
  digitalWrite(motor_left[0], LOW);
  digitalWrite(motor_left[1], HIGH);
}

void motor_backward() {
  digitalWrite(motor_rite[0], LOW);
  digitalWrite(motor_rite[1], HIGH);
  digitalWrite(motor_left[0], HIGH);
  digitalWrite(motor_left[1], LOW);
}

void motor_roright() {
  digitalWrite(motor_rite[0], HIGH);
  digitalWrite(motor_rite[1], LOW);
  digitalWrite(motor_left[0], HIGH);
  digitalWrite(motor_left[1], LOW);
}

void motor_roleft() {
  digitalWrite(motor_rite[0], LOW);
  digitalWrite(motor_rite[1], HIGH);
  digitalWrite(motor_left[0], LOW);
  digitalWrite(motor_left[1], HIGH);
}

void motor_forright() {
  digitalWrite(motor_rite[0], HIGH);
  digitalWrite(motor_rite[1], LOW);
  digitalWrite(motor_left[0], LOW);
  digitalWrite(motor_left[1], LOW);
}

void motor_forleft() {
  digitalWrite(motor_rite[0], LOW);
  digitalWrite(motor_rite[1], LOW);
  digitalWrite(motor_left[0], HIGH);
  digitalWrite(motor_left[1], LOW);
}

void motor_bacright() {
  digitalWrite(motor_rite[0], LOW);
  digitalWrite(motor_rite[1], HIGH);
  digitalWrite(motor_left[0], LOW);
  digitalWrite(motor_left[1], LOW);
}

void motor_bacleft() {
  digitalWrite(motor_rite[0], LOW);
  digitalWrite(motor_rite[1], LOW);
  digitalWrite(motor_left[0], LOW);
  digitalWrite(motor_left[1], HIGH);
}

void pinSetup(){
	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);
	for (int i = 1; i < 3; i++){
		pinMode(buttonPin[i],INPUT); //all pins are inputs
	}
	ch1.init(1,buttonPin[0]);
	
	for (int i = 0; i < 2; i++){ //pins 4,5,6,7 are output, set them all to LOW
		pinMode(motor_left[i], OUTPUT);
		pinMode(motor_rite[i], OUTPUT);
		digitalWrite(motor_left[i],LOW);
		digitalWrite(motor_rite[i],LOW);
	}
}

void buttonSetup(){
	buttonState[0] = smoothPPM();
	buttonState[1] = analogRead(buttonPin[1]) * (5.0 / 1023.0); //make them readable voltages
	buttonState[2] = analogRead(buttonPin[2]) * (5.0 / 1023.0); //make them readable voltages
	Serial.println("OUTPUT");
	Serial.println(buttonState[0]);
	Serial.println(buttonState[1]);
	Serial.println(buttonState[2]);
}

float smoothPPM(){
	// subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  ch1.readSignal();
  readings[index] = ch1.getSignal()/10;//add input data here; 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  average = total / numReadings;         
  // send it to the computer as ASCII digits
  return average;   
  delay(1);        // delay in between reads for stability    
}
