int pir = 7;
int led = 13;
int pirdato;

void setup() {
  //start serial connection
  Serial.begin(9600);
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(pir, INPUT);
  pinMode(led, OUTPUT);
  
}

void loop() {
  delay(100);
  pirdato = digitalRead(pir);
  Serial.println(pirdato);
  if (pirdato == HIGH){
    digitalWrite(led, HIGH);
  }else{
    digitalWrite(led, LOW);
  }
}
