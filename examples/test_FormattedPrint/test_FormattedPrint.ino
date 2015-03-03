void printTest();
void printSequenceTeaser();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {} // wait for serial port to connect. Needed for Leonardo only
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5000);
  printDemo();
  
  //printSequenceTeaser();
  
  // Do not discuss  printSequence features on the Print enhancement review thread.
  // a dedicated thread will be started for that.
}

// MJ development comment: do not forget to move Print* files if you changed them (see ./pv script)
