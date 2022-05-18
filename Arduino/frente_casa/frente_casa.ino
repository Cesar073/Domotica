
#include "DHT.h"

#define DHTPIN1 11
#define DHTPIN2 12

#define DHTTYPE DHT11

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

// sensores de Luz
int sen_luz_1 = A0;
int sen_luz_2 = A1;
int sen_luz_3 = A2;
int sen_luz_4 = A3;
int sen_luz_5 = A4;
int sen_luz_6 = A5;

// sensores generales
int sen_in_2 = 2;
int sen_in_3 = 3;
int sen_in_4 = 4;
int sen_in_5 = 5;
int sen_in_6 = 6;
int sen_in_7 = 7;

int sen_in_8 = 8;
int sen_in_9 = 9;
int sen_in_10 = 10;
int sen_in_11 = 11;
int sen_in_12 = 12;
int sen_in_13 = 13;

// int unit_time = 4;

// int state_luz_1 = 0;
// int state_luz_2 = 0;
// int state_luz_3 = 0;
// int state_luz_4 = 0;
// int state_luz_5 = 0;
// int state_luz_6 = 0;

// int cambio_luz_1 = 600;
// int cambio_luz_2 = 500;
// int cambio_luz_3 = 650;
// int cambio_luz_4 = 400;
// int cambio_luz_5 = 500;
// int cambio_luz_6 = 400;

// int state_mov_1 = 0;
// int state_mov_2 = 0;
// int state_mov_3 = 0;
// int state_mov_4 = 0;
// int state_mov_5 = 0;
// int state_mov_6 = 0;

// int state_rele_1 = 0;
// int state_rele_2 = 0;
// int state_rele_3 = 0;
// int state_rele_4 = 0;
// int state_rele_5 = 0;
// int state_rele_6 = 0;

// int count_rele_1 = 0;
// int count_rele_2 = 0;
// int count_rele_3 = 0;
// int count_rele_4 = 0;
// int count_rele_5 = 0;
// int count_rele_6 = 0;

void setup() {
  // config sensores de luz
  pinMode(sen_luz_1, INPUT);
  pinMode(sen_luz_2, INPUT);
  pinMode(sen_luz_3, INPUT);
  pinMode(sen_luz_4, INPUT);
  pinMode(sen_luz_5, INPUT);
  pinMode(sen_luz_6, INPUT);

  // config sensores de entrada
  pinMode(sen_in_2, INPUT);
  pinMode(sen_in_3, INPUT);
  pinMode(sen_in_4, INPUT);
  pinMode(sen_in_5, INPUT);
  pinMode(sen_in_6, INPUT);
  pinMode(sen_in_7, INPUT);

  pinMode(sen_in_8, INPUT);
  pinMode(sen_in_9, INPUT);
  pinMode(sen_in_10, INPUT);
  pinMode(sen_in_11, INPUT);
  pinMode(sen_in_12, INPUT);
  pinMode(sen_in_13, INPUT);

  // DEBUG
  Serial.begin(9600);
  dht1.begin();
  dht2.begin();
  // 20 seg de espera. Algunos sensores no marcan bien al iniciarse
  delay(2000);
}

void loop() {
  delay(5000);
  load_sen_luz(false);
  load_sen_mov(false);
  double_condition(state_luz_1, state_mov_1, rele_1, cambio_luz_1, 1);
  double_condition(state_luz_2, state_mov_2, rele_2, cambio_luz_2, 1);
  double_condition(state_luz_3, state_mov_3, rele_3, cambio_luz_3, 1);
  
  float h = dht1.readHumidity();
  float t = dht1.readTemperature();
  if (isnan(h) || isnan(t)){
    Serial.println("ERROR obteniendo hum y temp");
  }else{
    float hic = dht1.computeHeatIndex(t, h, false);

    Serial.print("H-T-ST: ");
    Serial.print(h);
    Serial.print(" - ");
    Serial.print(t);
    Serial.print(" - ");
    Serial.println(hic);
  }

  h = dht2.readHumidity();
  t = dht2.readTemperature() - 1;
  if (isnan(h) || isnan(t)){
    Serial.println("ERROR obteniendo hum y temp");
  }else{
    float hic = dht2.computeHeatIndex(t, h, false);

    Serial.print("H-T-ST: ");
    Serial.print(h);
    Serial.print(" - ");
    Serial.print(t);
    Serial.print(" - ");
    Serial.println(hic);
  }
}

void load_sen_luz (bool imprimir){
  state_luz_1 = analogRead(sen_luz_1) - 27;
  state_luz_2 = analogRead(sen_luz_2) + 28;
  state_luz_3 = analogRead(sen_luz_3) - 133;
  state_luz_4 = analogRead(sen_luz_4) + 90;
  state_luz_5 = analogRead(sen_luz_5) - 2;
  state_luz_6 = analogRead(sen_luz_6) + 43;

  if (imprimir == true){
    Serial.print("SENSORES DE LUZ: ");
    Serial.print(create_data(state_luz_1));
    Serial.print(create_data(state_luz_2));
    Serial.print(create_data(state_luz_3));
    Serial.print(create_data(state_luz_4));
    Serial.print(create_data(state_luz_5));
    Serial.println(create_data(state_luz_6));
  }
}

void load_sen_mov (bool imprimir){
  state_mov_1 = digitalRead(sen_mov_1);
  state_mov_2 = digitalRead(sen_mov_2);
  state_mov_3 = digitalRead(sen_mov_3);
  state_mov_4 = digitalRead(sen_mov_4);
  state_mov_5 = digitalRead(sen_mov_5);
  state_mov_6 = digitalRead(sen_mov_6);

  if (imprimir == true){
    Serial.print("SENSORES DE MOVIMIENTO: ");
    Serial.print(create_data(state_mov_1));
    Serial.print(create_data(state_mov_2));
    Serial.print(create_data(state_mov_3));
    Serial.print(create_data(state_mov_4));
    Serial.print(create_data(state_mov_5));
    Serial.println(create_data(state_mov_6));
  }
}

String create_data(int state){
  String texto = "[";
  if (state < 100){texto += "0";}
  if (state < 10){texto += "0";}
  texto += state;
  texto += "]";
  return texto;
}

void double_condition(int val1, int val2, int p_sensor, int cond_1, int cont_2){
  if (val1 >= cond_1 && val2 >= cont_2){
    digitalWrite(p_sensor, HIGH);
  }else{
    digitalWrite(p_sensor, LOW);
  }
}
