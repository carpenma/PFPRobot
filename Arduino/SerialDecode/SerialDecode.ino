// Matthew Carpenter - carpent6@purdue.edu
// This program is designed to recieve sets of commands over serial, parse them then change the state of various outputs accordingly

/* Mapping of first argument to board output:
  0 - HALT ALL PWM
  1 - Left Side PWM (0-255)
  2 - Right Side PWM (0-255)
  */
//Output signals by type:
// PWM
  int L_PWM_Pin = 10;
  int R_PWM_Pin = 11;
  char data_in[3];
  int command = 0;
  int val = 128;
  int num_read;
  
// Allows for custom values to be used to identify which "packet" goes with what side
  const int LEFT_COMMAND = '1';
  const int RIGHT_COMMAND = '2';
  const int HALT_COMMAND = '0';

void setup () {
  Serial.begin(9600);
  pinMode(L_PWM_Pin,OUTPUT);
  pinMode(R_PWM_Pin,OUTPUT);
}

void loop () {
  if (Serial.available() == 3) {
    num_read = Serial.readBytes(data_in,3);
    // For debugging input data
    /*Serial.print("Read ");
    Serial.print(num_read);
    Serial.print(" bytes and they are: ");
    Serial.print(data_in);
    Serial.print(" and the value I am passing is: ");
    Serial.println(data_in[2]);*/
  
    command = data_in[0];
    val = data_in[2];
    
    // More debugging 
    /*Serial.print("Data[0]: ");
    Serial.print(data_in[0]);
    Serial.print(" Data[1]: ");
    Serial.print(data_in[1]);
    Serial.print(" Data[2]: ");
    Serial.println(data_in[2]);*/
    
    switch (command) {
      case (int)HALT_COMMAND:
        analogWrite(L_PWM_Pin,128);
        analogWrite(R_PWM_Pin,128);
        Serial.print("HH");
        break;
      case (int)LEFT_COMMAND:
        analogWrite(L_PWM_Pin,data_in[2]);
        Serial.print('L');
        Serial.print((char)val);
        break;
      case (int)RIGHT_COMMAND:
        analogWrite(R_PWM_Pin,val);
        Serial.print('R');
        Serial.print((char)val);
        break;
      default:
        Serial.print("??");
        //Halt?
        break;
    }
  }
  //delay(1);
}
