// HACER
// * Separar en funciones los checks que nos devuelven datos a la Rpi, ya que también los quiero llamar desde dentro de cada orden una vez que se haya ejecutado alguna
// NOTA: los ajustes por que algún sensor marca distinto (como la temp por ej), se hacen desde la Rpi.



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

// PINES
// Si bien algunos no se van a utilizar, para mayor facilidad coloco todos los pines que trae el Arduino. De esta manera si quiero hacer referencia al pin 5, puedo llegar a él de 2 formas, o ingresar el valor 5 directamente, o buscar la posición 5 desde este array.
// En algunos casos una opción es más práctica que la otra por ende, evito posibles errores si a un elemento lo llamo de distintas maneras pero con un mismo valor.
byte pines [20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
// Los sensores de luz no leen constantemente su actividad, son necesario bajo ciertas condiciones, por ende, como es tan poco frecuente, indicamos al arduino en qué momento leer sus valores, y que tome a los 6 sensores por igual mediante la variable leer_luz
bool leer_luz = false;

// SENSORES DE TEMPERATURA-HUMEDAD-SENSACIÓN TÉRMICA
DHT dht1(11, DHTTYPE);
DHT dht2(12, DHTTYPE);
DHT dht3(13, DHTTYPE);
// Indicamos si leemos temp y humedad
bool leer_temp = true;

////////////////////////// VARIABLES AUXILIARES //////////////////////////

// STRING. En esta variable se genera el texto que se va a enviar a la Rpi
String message = "";

// Loops
// int delay_loop = 100; // tiempo para cada iteración del bucle *** Este valor quedó fijo en 100 milisegundos ***
int counter_loop = 1; // cant de veces que se tiene que repetir la iteración para entrar a la captura de datos
int max_loop = 50;     // valor para reiniciar counter_loop cuando llega a 0
// Esta config por defecto, indica que cada 100 milisegundos se entraría a leer las funciones internas.
// Si lo quisiera demorar al ingreso cada 1 segundo, hay que darle valor de 1000 milisegundos o max_loop un valor de 10

// GENERALES
// Avisamos si están connectados
// Pines 0 y 1: Son de transmisión
// Pines 2 a 10: Sensores comúnes (En este caso de movimiento)
// Pines 11 a 13: Sensores de Temperatura/Humedad/Sensación térmica
// Pines 14 a 19: Sensores de Luz
// Antes eran booleanos, pero como ocupan igual espacio que un byte y estoy teniendo errores de impresión, paso todo a byte
byte connect_sen [20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
// Contador que indica la cantidad de loops que deben darse antes de realizar una lectura
int counter [20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Cuando la lectura de un sensor se realice, reiniciamos el contador a éste valor
int max [20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


void setup() {
  // Configuramos los sensores menos los de Temperatura que se configuran a parte (11, 12 y 13)
  for (int i = 0; i < 20; i++){
    if (i < 11 || i > 13){
      pinMode(pines[i], INPUT);
    };
  };

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
  bool recibe_orden = false;

  // Leemos posibles nuevas ordenes de la Rpi
  if (Serial.available() > 0){
    String orden = Serial.readStringUntil('\n');
    Serial.println("Su orden: " + orden);
    if (orden.length() > 0){
      read_command(orden);
      Serial.println("msg: " + message);
      recibe_orden = true;
    }
  }

  // Se chequea si hay que ingresar a leer sensores
  counter_loop --;
  if (counter_loop <= 0){
    counter_loop = max_loop;
    // El string que se envía a Rpi se construye en esta var y se entrega al final del loop
    message = "";
    // Chequeamos los sensores de luz
    if (leer_luz == true){load_pines();};

    // Chequeamos todos los sensores restantes menos los últimos que son de temp-humed-sensacion y los de luz
    for (int i = 2; i < 11; i++){
      if (connect_sen[i] > 0){
        if (check_state_loop(i) == true){
          load_sensor(i);
        };
      };
    };

    // Chequeamos los sensores de Temperatura-Humedad-Sensación Térmica
    if (leer_temp == true) {load_sen_temp();};
    
    // Enviamos los resultados
    Serial.println("msg: " + message);
  };
}

// ******************************************* ESTRUCTURA Y CONFIGURACIÓN *******************************************

// Lee los comandos que recibimos por la Rpi
void read_command(String orden){

  // Las ordenes se organizan en función a combinación de los 3 primeros caracteres
  // CDS = Change Device State
  String command = orden.substring(0,3);
  // Variable tipo string auxiliar para usar dentro de toda la función sin tener que estar declarando muchas más variables
  String auxS = "    ";
  
  // El primer chequeo corresponde a la posibilidad de cambiar los valores GENERALES
  // Ej real: CGVF000F000
  // Ej real separado por claves: CGV F000 F000
  // CGV = Change General Values
  // F000 = True o False para indicar si los siguientes caracteres serán el int para sobrescribir el valor de counter_loop
  // F000 = True o False para indicar si los siguientes caracteres serán el int para configurar el valor de max_loop
  if (command == "CGV"){
    if(orden.substring(3,4) == "T"){
      int value = orden.substring(4,7).toInt();
      counter_loop = value;
    };
    if(orden.substring(7,8) == "T"){
      int value = orden.substring(8,11).toInt();
      max_loop = value;
    };
    check("BG");
  };

  // Cambiamos los valores de cada sensor
  // Ej real: CSVTF[03010010[04020010[11010030
  // Ej real separado por claves: CSV T F [03010010 [04020010 [11010030
  // T = True o False para la lectura de sensores de luz
  // F = True o False para la lectura de sensores de temperatura
  // [03010010 = (03) 2 dígitos para la posición en array del sensor - (010) 3 dig para el valor del contador del sensor - (010) 3 dig para el max
  if (command == "CSV"){
    // Lectura Sensores de Luz
    auxS = orden.substring(3,4);
    if (auxS == "T"){
      leer_luz = true;
    }else{
      leer_luz = false;
    };
    // Lectura Sensores de Temperatura
    auxS = orden.substring(4,5);
    if (auxS == "T"){
      leer_temp = true;
    }else{
      leer_temp = false;
    };
    // Demás sensores
    int largo = orden.length();
    if (largo > 5){
      auxS = orden.substring(5,largo);
      int tope = (largo - 5) / 9;
      int auxI, auxIpin, auxIcon, auxImax;
      for (int i = 0; i < tope; i++){
        // Cada iteracción debería contener esto: [03010010, donde auxI es la pos1, donde arranca el id del pin
        auxI = 1 + (9 * i);
        auxIpin = auxS.substring(auxI,auxI+2).toInt();
        auxIcon = auxS.substring(auxI+2,auxI+5).toInt();
        auxImax = auxS.substring(auxI+5,auxI+8).toInt();
        counter[auxIpin] = auxIcon;
        max[auxIpin] = auxImax;
      };
    };
    check("LT");
    Serial.println("msg: " + message);
    message = "";
    check("CS");
    Serial.println("msg: " + message);
    message = "";
    check("MS");
  };

  // Si se connecta o desconnecta un sensor, siempre de a un sensor a la vez. Ejemplo en caso de activar el pin 5 (está en la posición 3 de los arrays)
  // CDS03T >>> Conect Device Sensor - posición 3 - True
  // Los pines van desde 2 a 13 inclusive. Los 6 pines de luz se ignoran para esta actividad
  if (command == "CDS"){
    auxS = orden.substring(3,5);
    int device = auxS.toInt();
    auxS = orden.substring(5,6);
    if (auxS == "T"){
      connect_sen[device] = 1;
    }else{
      connect_sen[device] = 0;
    };
    // BORRAR BUCLE PARA PRINT
    message += "STATES: [";
    for(int j = 0; j < 14; j ++){
      if(connect_sen[j] > 0){
        message += "-T";
      }else{
        message += "-F";
      };
    };
    message += "] /// ";
    check("SS");
  };

  // Cuando la Rpi necesita algún conjunto de datos se lo enviamos
  // Ej: CHKBG > CHK = Check / BG = Bucle General
  // Un sólo comando por cada iteracción, 3 digitos para dar la orden y 2 
  if (command == "CHK"){
    auxS = orden.substring(3,5);
    check(auxS);
    };
};

// ************************************** CODIGOS DE LECTURA ORDENADOS POR PIN **************************************
// Nota: Las lecturas se van agregando en la variable tipo String llamada "message" que luego al final del bucle se envía a la Rpi

// Lee los valores de todos los sensores de luz:
void load_pines (){
  // Ej: [A-0002][B-0100][C-1024][D-0500][E-0010][F-1000]
  String aux;
  int state;
  for(int i = 14; i < 20; i++){
    state = analogRead(pines[i]);
    aux += "[" + String(pines[i]) + "-" + format_value(state) + "]";
  };
  message += aux;
}

// Recibe los atributos de un elemento (del pin 2 hasta el 10), lee los datos y los envía
void load_sensor(int pos){
  // Ej: [5-0255] > El sensor de movimiento del pin 5, devolvió 255 de valor de lectura
  int state = digitalRead(pines[pos]);
  String aux = "[" + String(pines[pos]) + "-" + format_value(state) + "]";
  message += aux;
}

// Lee los 3 sensores de humedad connectados
void load_sen_temp(){
  // Tiene en cuenta si están activos en su var conect_sen_X
  // Ej: [11-H-T-ST:45-21-25][12-ERROR]
  // Datos: 11 = Pin / H-T-ST = Humedad, Temperatura, Sensación térmica (separan 2 puntos sus valores) / 45 = %de humedad / 21 y 25 = grados Celcius /
  // El pin 12 tendría un error y el 13 no estaría instalado, por ende, su var conect_sen_13 sería false.
  float h, t, hic;
  if(connect_sen[11] > 0){
    h = dht1.readHumidity();
    t = dht1.readTemperature();
    if (isnan(h) || isnan(t)){
      message += "[11-ERROR]";
    }else{
      hic = dht1.computeHeatIndex(t, h, false);
      message += "[11-H-T-ST:";
      message += String(h) + '-' + String(t) + '-' + String(hic) + ']';
    }
  }

  if(connect_sen[12] > 0){
    h = dht2.readHumidity();
    t = dht2.readTemperature();
    if (isnan(h) || isnan(t)){
      message += "[12-ERROR]";
    }else{
      hic = dht2.computeHeatIndex(t, h, false);
      message += "[12-H-T-ST:";
      message += h + '-' + t + '-' + hic + ']';
    }
  }

  if(connect_sen[13] > 0){
    h = dht3.readHumidity();
    t = dht3.readTemperature();
    if (isnan(h) || isnan(t)){
      message += "[1-ERROR]";
    }else{
      hic = dht3.computeHeatIndex(t, h, false);
      message += "[13-H-T-ST:";
      message += h + '-' + t + '-' + hic + ']';
    }
  }
}

// ********************************************** FUNCIONES AUXILIARES **********************************************

// Recibe la posición de un elemento, le resta 1 a su contador, y devuelve T/F en caso de que se haya llegado a 0.
// Si llegó a 0 entonces actualizamos su valor a max.
bool check_state_loop(int pos){
  counter[pos]--;
  if (counter[pos] <= 0){
    counter[pos] = max[pos];
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

// ************************************* DEVOLVEMOS LOS VALORES QUE RPI NECESITE ************************************

void check(String orden){
  // Bucle General
  if (orden == "BG"){
    message = "[counter:" + String(counter_loop) + " / max:" + String(max_loop) + "]";
    return;
  };
  
  // Indica True o False por los sensores de Luz y Temperatura, las vars que indican que se lean o no sus datos
  if (orden == "LT"){
    message = "[leer_luz:" + String(leer_luz) + " / leer_temp:" + String(leer_temp) + "]";
    return;
  };

  // Lista de estados que indica qué sensores están conectados
  if (orden == "SS"){
    message = "[";
    for(int i = 0; i < 20; i++){
      message += String(connect_sen[i]);
      if(i<19){message += "-";}else{message += "]";};
    };
    return;
  };
  
  // Lista del valor de contadores que tienen los sensores
  if (orden == "CS"){
    message = "[";
    for(int i = 0; i < 20; i++){
      message += String(counter[i]);
      if(i<19){message += "-";}else{message += "]";};
    };
    return;
  };

  // Lista del valor de max que tienen los sensores
  if (orden == "MS"){
    message = "[";
    for(int i = 0; i < 20; i++){
      message += String(max[i]);
      if(i<19){message += "-";}else{message += "]";};
    };
    return;
  };
  message = "[CHK-ERROR]";
}

// CDS00T
// CSVFF[00005005
