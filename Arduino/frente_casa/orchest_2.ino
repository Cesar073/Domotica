// HACER
// LEER LÍNEA 333

// El sistema está controlado por una Rpi 4. Este código está preparado para ser colocado en un Arduino Uno, y comunicado con la Rpi por USB(puerto serie).
// Arduino sólo se dedica a enviar los datos de los sensores, Rpi toma las decisiones y le devuelve una orden, el cambio de variables. Dichas variables sólo se utilizan para configurar la frecuencia en que Arduino tiene que recopilar datos e informarlos. De esta manera, una vez instalado todo el sistema de sensores y el Arduino, sólo necesitamos cambiar el código de la Rpi en función a los objetivos (Alarma, modo día, noche, almuerzo, etc).
// Arduino sólo se encarga de enviar los datos en el momento que corresponda, y envía lo que haya que enviar.
// Ejemplo: [A-0375] -> En el ejemplo, la letra identifica el sensor que es uno de luz y su valor está en un formato de 4 dígitos.
// Las ordenes que Arduino reciba van a ser siempre iguales y son completas, es decir, la config de todos los sensores con el mismo largo de caracteres en cada orden.

// Necesitamos 3 tipos de órdenes, y están ordenadas en importancia:
// Config de respuesta de los sensores (Cambiamos las frecuencias de envío de datos)
// Config de estados de variables
// Config general (cambio en los pines)

// Cada orden está determinada por el valor del primer caracter A, B o C:
// B
// Ejemplo: 01001000
// A continuación, el mismo ejemplo pero separado para determinar sus bloques:
// 0010 - 4 digitos - Configuran max_loop
// 1000 - 4 digitos - Se puede sobrescribir el valor de counter_loop. Para evitar que arduino sobrescriba, "FFFF"
// 


#include "DHT.h"

#define DHTTYPE DHT11

// sensores de Luz  // Lista de Char que los identifica
int sen_luz [6] = {A0, A1, A2, A3, A4, A5};
// int sen_luz_1 = A0; // A
// int sen_luz_2 = A1; // B
// int sen_luz_3 = A2; // C
// int sen_luz_4 = A3; // D
// int sen_luz_5 = A4; // E
// int sen_luz_6 = A5; // F

// sensores generales
int sen_in[9] = {2,3,4,5,6,7,8,9,10};
// int sen_in_2 = 2; // G
// int sen_in_3 = 3; // H
// int sen_in_4 = 4; // I
// int sen_in_5 = 5; // J
// int sen_in_6 = 6; // K
// int sen_in_7 = 7; // L
// int sen_in_8 = 8; // M
// int sen_in_9 = 9; // N
// int sen_in_10 = 10; // O

#define DHTPIN1 11 // P
#define DHTPIN2 12 // Q
#define DHTPIN2 13 // R

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN2, DHTTYPE);

////////////////////////// VARIABLES AUXILIARES //////////////////////////

// STRING. En esta variable se genera el texto que se va a enviar a la Rpi
String message = "";

// Loops
// int delay_loop = 100; // tiempo para cada iteración del bucle *** Este valor quedó fijo en 100 ***
int counter_loop = 1; // cant de veces que se tiene que repetir la iteración para entrar a la captura de datos
int max_loop = 1;     // valor para reiniciar counter_loop cuando llega a 0
// Esta config por defecto, indica que cada 100 milisegundos se entraría a leer las funciones internas.
// Si lo quisiera demorar al ingreso cada 1 segundo, hay que darle valor de 1000 milisegundos o max_loop un valor de 10

// Sensor Luz
// Los sensores de luz sólo son necesarios luego de detectar movimiento. Cuando es de noche habrán luces que se prenderan sólas en función al horario
// manejado por la Rpi. Pero las luces que se prenderán en función a los sensores de luz, lo harán luego de detectar movimiento, así que sólo en esos momentos
// vamos a leer sus valores. Con una única variable indicamos que leemos todos los valores.
bool leer_luz = false;
int state_luz [6] = {0,0,0,0,0,0};
// int state_luz_1 = 0;
// int state_luz_2 = 0;
// int state_luz_3 = 0;
// int state_luz_4 = 0;
// int state_luz_5 = 0;
// int state_luz_6 = 0;

// Indicamos si leemos temp y humedad
bool leer_temp = false;

// OTROS SENSORES
// Avisamos si están connectados
bool connect_sen [12] = {false, false, false, false, false, false, false, false, false, false, false, false};
// bool connect_sen_2 = false;
// bool connect_sen_3 = false;
// bool connect_sen_4 = false;
// bool connect_sen_5 = false;
// bool connect_sen_6 = false;
// bool connect_sen_7 = false;
// bool connect_sen_8 = false;
// bool connect_sen_9 = false;
// bool connect_sen_10 = false;
// bool connect_sen_11 = false;
// bool connect_sen_12 = false;
// bool connect_sen_13 = false;

// Estado de otros sensores
int state_sen [9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
// int state_sen_2 = 0;
// int state_sen_3 = 0;
// int state_sen_4 = 0;
// int state_sen_5 = 0;
// int state_sen_6 = 0;
// int state_sen_7 = 0;
// int state_sen_8 = 0;
// int state_sen_9 = 0;
// int state_sen_10 = 0;

// Contador que indica la cantidad de loops que deben darse antes de realizar una lectura
int counter [12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// int counter_2 = 0;
// int counter_3 = 0;
// int counter_4 = 0;
// int counter_5 = 0;
// int counter_6 = 0;
// int counter_7 = 0;
// int counter_8 = 0;
// int counter_9 = 0;
// int counter_10 = 0;
// int counter_11 = 0;
// int counter_12 = 0;
// int counter_13 = 0;

// Cuando la lectura de un sensor se realice, reiniciamos el contador a éste valor
int max [12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// int max_2 = 10;
// int max_3 = 10;
// int max_4 = 10;
// int max_5 = 10;
// int max_6 = 10;
// int max_7 = 10;
// int max_8 = 10;
// int max_9 = 10;
// int max_10 = 10;
// int max_11 = 10;
// int max_12 = 10;
// int max_13 = 10;

// Los sensores de humedad pueden estar defasados, si tenemos un valor podemos corregirlo
float reajuste_hum_11 = 0;
float reajuste_tem_11 = 0;
float reajuste_hum_12 = 0;
float reajuste_tem_12 = 0;
float reajuste_hum_13 = 0;
float reajuste_tem_13 = 0;

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

  // DEBUG
  Serial.begin(9600);
  dht1.begin();
  dht2.begin();
  dht3.begin();
  // 20 seg de espera. Algunos sensores no marcan bien al iniciarse.
  // Como no afecta demasiado lo dejo en 1 seg.
  delay(1000);
}

void loop() {

  // Tener en cuenta que la conclusión obtenida por la Rpi, es a partir de los datos enviados en la iteración anterior
  delay(100);

  // Leemos posibles nuevas ordenes de la Rpi
  if (Serial.available() > 0){
    String orden = Serial.readStringUntil('\n');
    if (orden.length() > 0){
      read_command(orden);
    }
  }

  // Se chequea si hay que ingresar a leer sensores
  if (check_state_loop(counter_loop, max_loop) == true){

    // El string que se envía a Rpi se construye en esta var y se entrega al final del loop
    message = "";

    // Chequeamos los sensores de luz
    if (leer_luz == true){load_sen_luz();};

    // Chequeamos todos los sensores restantes menos los 3 últimos que son de temp-humed-sensacion
    load_sensor(connect_sen_2, sen_in_2, state_sen_2, counter_2, max_2, 'G');
    load_sensor(connect_sen_3, sen_in_3, state_sen_3, counter_3, max_3, 'H');
    load_sensor(connect_sen_4, sen_in_4, state_sen_4, counter_4, max_4, 'I');
    load_sensor(connect_sen_5, sen_in_5, state_sen_5, counter_5, max_5, 'J');
    load_sensor(connect_sen_6, sen_in_6, state_sen_6, counter_6, max_6, 'K');
    load_sensor(connect_sen_7, sen_in_7, state_sen_7, counter_7, max_7, 'L');
    load_sensor(connect_sen_8, sen_in_8, state_sen_8, counter_8, max_8, 'M');
    load_sensor(connect_sen_9, sen_in_9, state_sen_9, counter_9, max_9, 'N');
    load_sensor(connect_sen_10, sen_in_10, state_sen_10, counter_10, max_10, 'O');

    // Chequeamos los sensores de Temperatura-Humedad-Sensación Térmica
    if (leer_temp == true) {load_sen_temp();};
    
    // Enviamos los resultados
    Serial.println(message);
  }
}

// ******************************************* ESTRUCTURA Y CONFIGURACIÓN *******************************************

// Lee los comandos que recibimos por la Rpi
void read_command(String orden){

  // Las ordenes se organizan en función a combinación de los 3 primeros caracteres
  // CDS = Change Device State
  String command = orden.substring(0,4);
  // Variable tipo string auxiliar para usar dentro de toda la función sin tener que estar declarando muchas más variables
  String aux = "    ";
  
  // El primer chequeo corresponde a la posibilidad de necesitar de urgencia algún dato del sensor, para eso nos aseguramos y cambiamos varios valores del mismo.
  // UDS12 >>> Urgency Data Sensor - pin> 12


  // Si se connecta o desconnecta un sensor, siempre de a un sensor a la vez. Ejemplo en caso de activar el pin 5
  // CDS05T >>> Conect Device Sensor - pin> 05 - True
  if (command == "CDS"){
    aux = orden.substring(4,6);
    int device = aux.toInt();
    aux = orden.substring(6,7);
    if (aux == "T"){
      change_connect_device(device, true);
    }else{
      change_connect_device(device, false);
    }
  }
}

// Actualiza datos si se conecta o desconecta un nuevo sensor
void change_connect_device(int device, bool state){
  switch (device) {
    case 2:
      connect_sen_2 = state;
      break;
    case 3:
      connect_sen_3 = state;
      break;
    case 4:
      connect_sen_4 = state;
      break;
    case 5:
      connect_sen_5 = state;
      break;
    case 6:
      connect_sen_6 = state;
      break;
    case 7:
      connect_sen_7 = state;
      break;
    case 8:
      connect_sen_8 = state;
      break;
    case 9:
      connect_sen_9 = state;
      break;
    case 10:
      connect_sen_10 = state;
      break;
    case 11:
      connect_sen_11 = state;
      break;
    case 12:
      connect_sen_12 = state;
      break;
    case 13:
      connect_sen_13 = state;
      break;
  }
}

// ************************************** CODIGOS DE LECTURA ORDENADOS POR PIN **************************************
// Nota: Las lecturas se van agregando en la variable tipo String llamada "message" que luego al final del bucle se envía a la Rpi

// Lee los valores de todos los sensores de luz:
void load_sen_luz (){
  // Ej: [A-0002][B-0100][C-1024][D-0500][E-0010][F-1000]
  String aux;
  state_luz_1 = analogRead(sen_luz_1);
  aux += "[A-" + format_value(state_luz_1) + ']';
  state_luz_2 = analogRead(sen_luz_2);
  aux += "[B-" + format_value(state_luz_2) + ']';
  state_luz_3 = analogRead(sen_luz_3);
  aux += "[C-" + format_value(state_luz_3) + ']';
  state_luz_4 = analogRead(sen_luz_4);
  aux += "[D-" + format_value(state_luz_4) + ']';
  state_luz_5 = analogRead(sen_luz_5);
  aux += "[E-" + format_value(state_luz_5) + ']';
  state_luz_6 = analogRead(sen_luz_6);
  aux += "[F-" + format_value(state_luz_6) + ']';
  message += aux;
}

// Recibe los atributos de un elemento (del pin 2 hasta el 10), resta al contador, chequea si hay que enviar datos y reinicia su contador si es necesario
void load_sensor(bool connect, int counter, int state, int sensor, char name_element, int max){
  if (connect == true){
    counter --;
    if (counter == 0){
      state = digitalRead(sensor);
      String aux = "[" + name_element;
      aux += "-";
      aux += format_value(state) + "]";
      message += aux;
      counter = max;
    }
  }
}

// Lee los 3 sensores de humedad connectados
void load_sen_temp(){
  // Tiene en cuenta si están activos en su var conect_sen_X
  // Ej: [11-H-T-ST:45-21-25][12-ERROR]
  // Datos: 11 = Pin / H-T-ST = Humedad, Temperatura, Sensación térmica (separan 2 puntos sus valores) / 45 = %de humedad / 21 y 25 = grados Celcius /
  // El pin 12 tendría un error y el 13 no estaría instalado, por ende, su var conect_sen_13 sería false.
  float h, t, hic;
  if(connect_sen_11 == true){
    h = dht1.readHumidity();
    h += reajuste_hum_11;
    t = dht1.readTemperature();
    t += reajuste_tem_11;
    if (isnan(h) || isnan(t)){
      Serial.println("[11-ERROR]");
    }else{
      hic = dht1.computeHeatIndex(t, h, false);
      message += "[11-H-T-ST:";
      message += h + '-' + t + '-' + hic + ']';
    }
  }

  if(connect_sen_12 == true){
    h = dht2.readHumidity();
    h += reajuste_hum_12;
    t = dht2.readTemperature();
    t += reajuste_tem_12;
    if (isnan(h) || isnan(t)){
      Serial.println("[12-ERROR]");
    }else{
      hic = dht2.computeHeatIndex(t, h, false);
      message += "[12-H-T-ST:";
      message += h + '-' + t + '-' + hic + ']';
    }
  }

  if(connect_sen_13 == true){
    h = dht3.readHumidity();
    h += reajuste_hum_13;
    t = dht3.readTemperature();
    t += reajuste_tem_13;
    if (isnan(h) || isnan(t)){
      Serial.println("[13-ERROR]");
    }else{
      hic = dht3.computeHeatIndex(t, h, false);
      message += "[13-H-T-ST:";
      message += h + '-' + t + '-' + hic + ']';
    }
  }
}

// ********************************************** FUNCIONES AUXILIARES **********************************************

// Recibe un elemento de bucle, le resta 1, y devuelve T/F en caso de que se haya llegado a 0. Si llegó a 0 entonces actualizamos su valor a max
bool check_state_loop(int counter, int max){
  counter --;
  if (counter == 0){
    counter = max;
    return true;
  }else{
    return false;
  };
}

// Convierte un valor int a un string del tipo numérico de 4 dígitos: 14 = 0014
String format_value(int state){
  String texto = "";
  if (state < 10){
    texto += "000";
  }else{
    if (state < 100){
      texto += "00";
      }else{
        if (state < 1000){
          texto = "0";
        }
      }
  }
  texto += state;
  return texto;
}









