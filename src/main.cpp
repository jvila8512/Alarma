#include <Keypad.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <string.h>

// #include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
//  #include <ir_Lego_PF_BitStreamEncoder.h>

#define VELOCIDAD 100 // Velocidad a la que se mueve el texto
// Ping ping = Ping(31);




unsigned long startTime;

int contador = 1;
int categoria = 1;
int menu = 1;       // variable para el Menu // cuenta el nivel del menu en el que se esta
int menu_alarma = 1; // variable para el Menu de la Alarma // cuenta el nivel del menu en el que se esta
int menu_reloj = 1;
int menu_config = 1;
int setAlarma = 0; // activar la alarma
int leer_setAlarma = 0; // leer de la EEPROM para la alarma
int ActivarAlarma = 0; // variable para el retardo de 3 segundos
int cursorr = 6;    //para posicionar el cursor cdo se active la Alarma


volatile int AlarmaZona1 = 0;  // Si se activa la Alarma en la zona 1
volatile int AlarmaZona2 = 0;  // Si se activa la Alarma en la zona 1
volatile int AlarmaZona3 = 0;  // Si se activa la Alarma en la zona 1

int Alarma_automatica = 0;    // Variable para activar segun una hora la Alarma automaticamente la alarma por defecto esta desactivada
int hora_Alarma = 0;         // Hora para activar la Alarma automaticamenta
int hora_Alarma_AM_PM = 2;  // AM=1 PM=2
int unavez = 1;
int tiempoEsperaAlarma = 15; // Tiempo para activar la Alarma se puede cambiar en configuraciones
int activarTeclado = 0;   // para medir el tiempo de desactivar Display

//****** Para hacer un Reloj ******//
int minutos = 0;
int segundos = 0;
int horas = 6;
int dia = 1;
int mes = 1;
int anio = 2020;

int cont = 2;

// Variables para el PIR

int pinPIR = 41;
int estadoPIR = 0;
int ledPIR = 43;
int tiempoPIR = 0;
int contarPIR = 0;
int activarPIR = 1;//Variable para activra o desatctivar el PIR 

// variables Escalera
int pinPRE = 45;
int estadoPRE = 0;
int contarEsc = 0;

////


// Variables para buscar el dia de la semana
int n_anno;
int d_anno;
int n_dia;
int r_dia;
int n_mes;
int t_mes;
int t_siglo = 0;
//******* Fin de Reloj *****//
// variables para saber cdo esta apagado o no el local 0 es apagado
int cuarto1 = 0, cuarto2 = 0, cuarto3 = 0, bano = 0;
int sala = 0, comedor = 0, cocina1 = 0, cocina2 = 0, pasillo = 0,  patio = 0;
char customKey = 'q';
IRrecv irrecv(52);
decode_results results;
const byte ROWS = 4;  //Cuatro Filas
const byte COLS = 3;  //Cuatro Columnas
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'0', '.', 'V'}
};
byte rowPins[ROWS] = {7, 6, 4, 5}; //Puntos de conexion del teclado al arduino FILAS
byte colPins[COLS] = {8, 9, 10 }; //Puntos de conexion del teclado al arduino COLUMNAS

Keypad teclado = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); //Configura el teclado

LiquidCrystal lcd(12, 11, 14, 15, 16, 17); //(RS, E, D4,D5, D6, D7)
int value, PW, pass = 1234;
char Key;
//******   Variables para controlar tiempos   ********//
int tiempo1 = 1;
int tiempo2 = 1;
int sal = 0;


////***** Caracterres especiales

byte smile[] = {
  B00000,
  B11011,
  B11011,
  B00000,
  B00000,
  B10001,
  B10001,
  B01110
};
byte nada[]  = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte robot[] = {
  B11111,
  B10101,
  B11111,
  B00100,
  B11111,
  B11111,
  B11111,
  B11111
};
byte Sound[] = {
  B00001,
  B00011,
  B00101,
  B01001,
  B01001,
  B01011,
  B11011,
  B00000,
};
byte Heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};
byte Lock[]  =
{
  B01110,
  B10001,
  B10001,
  B11111,
  B11011,
  B11011,
  B11111,
  B00000
};
byte Lock1[] = {
  B11110,
  B00001,
  B00001,
  B11111,
  B11011,
  B11011,
  B11111,
  B00000
};
byte alarma[] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  B00100,
  B00000
};
//******* SETUP ********//
void setup() {

  Serial.begin(9600);
  lcd.begin(16, 2);   // Inicia o LCD 16x02 (colunas,linhas)
  teclado.setHoldTime(500); //Tempo de espera para o teclado

  EEPROM.get( 0, PW );
  if (PW != pass && PW > 0) {
    pass = PW;
  }

  EEPROM.get( 10, leer_setAlarma );
  if (leer_setAlarma != setAlarma && leer_setAlarma >= 0) {
    setAlarma = leer_setAlarma;
  }

  //EEPROM.put( 0, 1234 );
  // Ponemos los pines de las Luces en modo OUTPUT (salida)
  for (int i = 22; i <= 35; i++)
  {
    pinMode(i, OUTPUT);
  }

  //for (int i = 22; i <= 32; i++)
  //  digitalWrite(i, HIGH);

  // pin empieza en alto para que este activo el display
  digitalWrite(33, HIGH);

  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(20, OUTPUT);
  // ping de lectura del mando
  pinMode(52, INPUT);
  //PIN PIR
  pinMode(pinPIR, INPUT);
  pinMode(45, INPUT);

  //Ping para los sonidos de buzzer
  //  pinMode(31, OUTPUT);

  //ZONAS DE LA CASA
  //
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  //pinMode(19, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);

  //pinMode(45, INPUT_PULLUP);

  Timer1.initialize(1000000);
  Timer1.attachInterrupt(contadorreloj);


  attachInterrupt(digitalPinToInterrupt(2), zona1, HIGH);
  attachInterrupt(digitalPinToInterrupt(3), zona2, HIGH);
  // attachInterrupt(digitalPinToInterrupt(19),zona3, HIGH);

  //para activar el teclado
  attachInterrupt(digitalPinToInterrupt(21), activarTeclado1, LOW);



  irrecv.enableIRIn();

  // startTime = millis();

  // desplazarTextoAbajo("Soluciones JV-Domotica");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soluc. Domótica");
  //delay(2000);
  // desplazarTextoAbajo("Ing. Javier Vila Labrada 52046805");
  lcd.setCursor(1, 1);
  lcd.print("Javi 52046805");
  delay(4000);
  lcd.clear();

  lcd.createChar(0, smile);
  lcd.createChar(1, robot);
  lcd.createChar(4, Heart);
  lcd.createChar(5, nada);
  lcd.createChar(3, Lock);
  lcd.createChar(6, Sound);
  lcd.createChar(7, Lock1);
  lcd.createChar(8, alarma);





  Menu();
}

void activarTeclado1 () {

  activarTeclado = 0;
  // Inicia o LCD 16x02 (colunas,linhas)
  digitalWrite(33, HIGH);
  ActivarAlarma = 0;

  lcd.begin(16, 2);
  //lcd.clear();
  Menu();

}

int password(void) {
  int pass = 0, i = 0;
  String KeyWord;

  //Pregunta por los 4 digitos del pass
  lcd.setCursor(6, 1);
  while (i < 4) {
    Key = teclado.getKey();//teclado.waitForKey(); //Espera hasta presionar un boton
    if (Key >= '0' && Key <= '9') {

      lcd.print('*');
      KeyWord += Key;   //Guarda los caracteres
      i++;
      cursorr++;
    }
    if (Key == 'V') {
      i = 4;
      KeyWord = "";
      lcd.clear();
      lcd.print("Cancelado");
      delay(100);

    }
  }
  if (KeyWord.length() > 0) {
    pass = KeyWord.toInt(); //Convierte para Intero
  }
  else {
    pass = 0;
  }

  return (pass);
}
void loop() {



  // Verificar siempre la Alarma

  sal = verificarAlarma();
  if (sal == 1)
  {
    Menu();
    sal = 0;
  }
  // Espero por la tecla para el menu
  char customKey = teclado.getKey();
  /*
    if (customKey){
    //Serial.println(customKey);
     lcd.clear();
      lcd.setCursor(0, 1);
      lcd.println("> ");
     lcd.print(customKey);

    }*/
  if ( customKey == '10' ) {
    activarTeclado = 0;
    //lcd.begin(16, 2);   // Inicia o LCD 16x02 (colunas,linhas)
    digitalWrite(33, HIGH);

    EEPROM.get( 10, leer_setAlarma );
    if (leer_setAlarma != setAlarma && leer_setAlarma >= 0) {
      setAlarma = leer_setAlarma;
    }
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("Alarma->");
    lcd.print(setAlarma);

    lcd.setCursor(0, 1);


    lcd.print("PIR->");

    lcd.print(contarPIR);

    lcd.setCursor(9, 1);

    lcd.print("Esc->");
    lcd.print(contarEsc);


    delay(2000);
    Menu();
  }

  if ( customKey == '.' ) {
    menu++;
    activarTeclado = 0;
    //lcd.begin(16, 2);   // Inicia o LCD 16x02 (colunas,linhas)
    digitalWrite(33, HIGH);
    Menu();
  }
  //Pregunto si se apreto el (V) para entrar en un Menu
  if ( customKey == 'V') {
    activarTeclado = 0;
    digitalWrite(33, HIGH);

    ejecutar(menu);

  }
  //Pregunto si se apreto el Mando para las Luces
  if (irrecv.decode(&results)) {
    lcd.clear();
    lcd.print("Local->");
    lcd.setCursor(0, 1);
    //lcd.println(">");
    //delay(1000);

    long a = results.value;

    luces(a);
    Menu();
  }

  //**************  Comunicacion por puerto serie Con JAVA ************************//

  /*
     if(Serial.available()>0){

     char entrada_serial;
     entrada_serial=Serial.read();




      if(entrada_serial=='1'){
        digitalWrite(20, HIGH);
         lcd.setCursor(1, 1);
         lcd.print("Encendido.....");
          lcd.clear();

         lcd.print("Com. Java1");

        Serial.println("Encendido.....");
         delay(500);

          }

        if(entrada_serial=='0')
      {
        digitalWrite(20, LOW);
        lcd.setCursor(1, 1);
         lcd.print("APagado.....");
          lcd.clear();

         lcd.print("Com. Java2");

        Serial.println("APagado.....");
         delay(500);

        }
       //delay(1000);
       lcd.clear();
      lcd.home();

     lcd.print("Esperando.....");
      }
  */
  //************** FIN  Cominicacion por puerto serie Con JAVA ************************//
}

//*** Fin del LOOPP ***//
int verificarAlarma() {
  //delay(3000)
  //int ActivarAlarma=0;
  if (setAlarma == 1 && ( AlarmaZona1 == 1 || AlarmaZona2 == 1 || AlarmaZona3 == 1) && ActivarAlarma == 1 ) {

    lcd.clear();
    if (AlarmaZona1 == 1 ) {
      lcd.print("Alarma Zona1");
      lcd.setCursor(13, 0);
      lcd.write(byte(7));

    } else if (AlarmaZona2 == 1 ) {
      lcd.print("Alarma Zona2");
      lcd.setCursor(13, 0);
      lcd.write(byte(7));
    } else if (AlarmaZona3 == 1 ) {
      lcd.print("Alarma Zona3");
      lcd.setCursor(13, 0);
      lcd.write(byte(7));
    }

    // lcd.setCursor(0, 1);
    // lcd.println("Clave>");
    int pw = password();


    EEPROM.get( 0, PW );

    if (PW != pass && PW > 0) {
      pass = PW;
    }
    if (pw == pass) {
      //   digitalWrite(20, LOW);
      AlarmaZona1 = 0;
      AlarmaZona2 = 0;
      AlarmaZona3 = 0;
      setAlarma = 0;
      EEPROM.put(10, 0);
      lcd.clear();
      lcd.print("Alarma Desact.");
      lcd.setCursor(8, 1);
      lcd.write(byte(7));
      digitalWrite(32, LOW);


      ActivarAlarma = 0;

      /*
        // cuarto 1
          digitalWrite(22, HIGH);
          cuarto1 = 1;
          //comedor
          digitalWrite(28, HIGH);

          comedor = 1;
          //sala
          digitalWrite(26, HIGH);

          sala = 1;

          comedor = 0;
          sala = 0;
          cocina1 = 0;
          cocina2 = 0;

          cuarto2 = 0;
          cuarto3 = 0;
          bano = 0;
          pasillo = 0;

      */

      delay(1000);
      cursorr = 6;
      lcd.begin(16, 2);   // Inicia o LCD 16x02 (colunas,linhas)

      Menu();
      return 1;
      //Menu();
      // delay(1000);
    } else
    {
      lcd.clear();
      lcd.print("Error de Clave");
      cursorr = 6;
      delay(2000);
      return 0;



    }
  }


  return 0;
}
//******** Función para cdo se active la alarma encontrar las zonas abiertas *****//
int comprobarzonas() {

  int zona1 = digitalRead(2);
  int zona2 = digitalRead(3);
  int zona3 = 0; //digitalRead(19);

    lcd.begin(16, 2);
    lcd.clear();
    
 lcd.setCursor(0, 0);
 lcd.print("Comprobando....");

 
  lcd.setCursor(8, 1);
  lcd.write(byte(3));
  digitalWrite(32, HIGH);
  //digitalWrite(32, HIGH);
  delay(400);
  digitalWrite(32, LOW);
  // delay(1500);
  delay(10 * 1000);

  if (zona1 == LOW && zona2 == LOW && zona3 == LOW) {

    lcd.clear();
    lcd.print("Alarma Activada");
    for (int i = 1; i <= 3; i++) {
      lcd.setCursor(8, 1);
      lcd.write(byte(3));
      digitalWrite(32, HIGH);
      //digitalWrite(32, HIGH);
      delay(400);
      digitalWrite(32, LOW);
      delay(1100);
    }

    AlarmaZona1 = 0;
    AlarmaZona2 = 0;
    AlarmaZona3 = 0;
    EEPROM.put(10, 1);
    setAlarma = 1;
    return 1;



  } else
  {
    lcd.clear();
    lcd.print(" Zona Abierta ");
    lcd.setCursor(14, 0);
    lcd.write(byte(7));
    if ( zona1 == 1  && zona2 == 0 && zona3 == 0  ) {
      lcd.setCursor(0, 1);
      lcd.print("Zona1");

    } else if (zona1 == 1 && zona2 == 1 && zona3 == 0) {

      lcd.setCursor(0, 1);
      lcd.print("Zona1, ");
      lcd.print("Zona2");



    } else if (zona1 == 1 && zona2 == 0 && zona3 == 1) {
      lcd.setCursor(0, 1);
      lcd.print(" Zona1, ");
      lcd.print("Zona3");


    } else if (zona1 == 0 && zona2 == 1 && zona3 == 1) {
      lcd.setCursor(0, 1);
      lcd.print(" Zona2, ");
      lcd.print("Zona3");


    } else if (zona1 == 1 && zona2 == 1 && zona3 == 1) {
      lcd.setCursor(0, 1);
      lcd.print("Zona1,");
      lcd.print("Zona2,");
      lcd.print("Z.3");

    } else if (zona1 == 0 && zona2 == 1 && zona3 == 0) {
      lcd.setCursor(0, 1);
      lcd.print(" Zona2");



    } else if (zona1 == 0 && zona2 == 0 && zona3 == 1) {
      lcd.setCursor(0, 1);
      lcd.print(" Zona3");

    }




    return 0;
  }



}
//*******  Función de Interupción 1 *****

void encenderTodasLuces(void) {
  for (int i = 30; i >= 22; i--)
  {
    //delay(3000);
    digitalWrite(i, HIGH);


  }
  comedor = 1;
  sala = 1;
  cocina1 = 1;
  cocina2 = 1;
  cuarto1 = 1;
  cuarto2 = 1;
  cuarto3 = 1;
  bano = 1;
  pasillo = 1;
}
void apagarTodasLuces() {

  for (int i = 22; i <= 30; i++)
  {
    delay(4000);
    digitalWrite(i, LOW);
  }
  comedor = 0;
  sala = 0;
  cocina1 = 0;
  cocina2 = 0;
  cuarto1 = 0;
  cuarto2 = 0;
  cuarto3 = 0;
  bano = 0;
  pasillo = 0;

}

void zona1() {

  int zona11 = digitalRead(2);

  if (setAlarma == 1) {

    if (zona11 == HIGH) {

      AlarmaZona1 = 1;
    }

  }

}
//*******  Función de Interupción 2 *****
void zona2() {

  int zona22 = digitalRead(3);

  if (setAlarma == 1) {
    if (zona22 == HIGH) {
      AlarmaZona2 = 1;
    }
  }
}
//*******  Función de Interupción 3 *****
void zona3() {

  if (setAlarma == 1) {
    // encenderTodasLuces();
    AlarmaZona3 = 0;
  }
}
//*******  Fin de Función de Interupción 1 *******//
//******** Funciones para desplazar el texto ******//
void desplazarTexto(String texto) {

  // Obtenemos el tamaño del texto
  int tam_texto = texto.length();

  // Mostramos entrada texto por la izquierda
  for (int i = tam_texto; i > 0 ; i--)
  {
    String texto1 = texto.substring(i - 1);

    // Limpiamos pantalla
    lcd.clear();

    //Situamos el cursor
    lcd.setCursor(0, 0);

    // Escribimos el texto
    lcd.print(texto1);

    // Esperamos
    delay(VELOCIDAD);
  }



}

void desplazarTextoAbajo(String texto) {

  // Obtenemos el tamaño del texto
  int tam_texto = texto.length();

  for (int i = 1; i <= tam_texto ; i++)
  {
    String texto1 = texto.substring(i - 1);

    // Limpiamos pantalla
    lcd.clear();

    //Situamos el cursor
    lcd.setCursor(0, 0);

    // Escribimos el texto
    lcd.print(texto1);

    // Esperamos
    delay(VELOCIDAD);
  }
}
//***** Menu para la Alarma ****///
void Menu() {

  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">Config");
      lcd.print("  Alarma");
      lcd.setCursor(1, 1);
      lcd.print("Reloj");
      lcd.print("   Luces");
      break;
    case 2:
      lcd.clear();
      lcd.print(" Config");
      lcd.print(" >Alarma");
      lcd.setCursor(1, 1);
      lcd.print("Reloj");
      lcd.print("   Luces");
      break;
    case 3:
      lcd.clear();
      lcd.print(" Config");
      lcd.print("  Alarma");
      lcd.setCursor(0, 1);
      lcd.print(">Reloj");
      lcd.print("   Luces");
      break;

    case 4:
      lcd.clear();
      lcd.print(" Config");
      lcd.print("  Alarma");
      lcd.setCursor(1, 1);
      lcd.print("Reloj");
      lcd.print("  >Luces");
      break;

    case 5:
      lcd.clear();
      lcd.print(">Config");
      lcd.print("  Alarma");
      lcd.setCursor(1, 1);
      lcd.print("Reloj");
      lcd.print("   Luces");
      menu = 1;
      break;
  }
}

void MenuReloj() {
  switch (menu_reloj) {
    case 0:
      menu_reloj = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">Menu");
      lcd.print("    Mostrar");
      lcd.setCursor(0, 1);
      lcd.print(" Hora");
      lcd.print("    Fecha");
      break;
    case 2:
      lcd.clear();
      lcd.print(" Menu");
      lcd.print("    Mostrar");
      lcd.setCursor(0, 1);
      lcd.print(">Hora");
      lcd.print("    Fecha");
      break;
    case 3:
      lcd.clear();
      lcd.print(" Menu");
      lcd.print("    Mostrar");
      lcd.setCursor(0, 1);
      lcd.print(" Hora");
      lcd.print("   >Fecha");
      break;
    case 4:
      lcd.clear();
      lcd.print(" Menu");
      lcd.print("   >Mostrar");
      lcd.setCursor(0, 1);
      lcd.print(" Hora");
      lcd.print("    Fecha");
      break;
    case 5:
      lcd.clear();
      lcd.print(">Menu");
      lcd.print("    Mostrar");
      lcd.setCursor(0, 1);
      lcd.print(" Hora");
      lcd.print("    Fecha");
      menu_reloj = 1;
      break;
  }//Fin del switch
}
//***** FIn Menu para la Alarma ****//
void MenuAlarma() {
  switch (menu_alarma) {
    case 0:
      menu_alarma = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">Menu");
      lcd.print("    C.clave");
      lcd.setCursor(0, 1);
      lcd.print(" Activar");
      lcd.print(" Desact");
      break;
    case 2:
      lcd.clear();
      lcd.print(" Menu");
      lcd.print("    C.clave");
      lcd.setCursor(0, 1);
      lcd.print(">Activar");
      lcd.print(" Desact");
      break;
    case 3:
      lcd.clear();
      lcd.print(" Menu");
      lcd.print("    C.clave");
      lcd.setCursor(0, 1);
      lcd.print(" Activar");
      lcd.print(" >Desact");
      break;
    case 4:
      lcd.clear();
      lcd.print(" Menu");
      lcd.print("   >C.clave");
      lcd.setCursor(0, 1);
      lcd.print(" Activar");
      lcd.print(" Desact");
      break;
    case 5:
      lcd.clear();
      lcd.print(">Menu");
      lcd.print("    C.clave");
      lcd.setCursor(0, 1);
      lcd.print(" Activar");
      lcd.print(" Desact");
      menu_alarma = 1;
      break;
  }
}
//***** FIn Menu para la Alarma ****///

/// Menu de Configuracion *****////
void MenuConfig() {

  switch (menu_config) {
    case 0:
      menu_config = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">Menu");
      lcd.print("    H/Alarma");
      lcd.setCursor(0, 1);
      lcd.print(" T/Esp");
      lcd.print("   A/Auto");
      break;
    case 2:
      lcd.clear();
      lcd.print(" Menu");
      lcd.print("   >H/Alarma");
      lcd.setCursor(0, 1);
      lcd.print(" T/Esp");
      lcd.print("   A/Auto");
      break;
    case 3:
      lcd.clear();
      lcd.print(" Menu");
      lcd.print("    H/Alarma");
      lcd.setCursor(0, 1);
      lcd.print(">T/Esp");
      lcd.print("   A/Auto");
      break;
    case 4:
      lcd.clear();
      lcd.print(" Menu");
      lcd.print("    H/Alarma");
      lcd.setCursor(0, 1);
      lcd.print(" T/Esp");
      lcd.print("  >A/Auto");
      break;
    case 5:
      lcd.clear();
      lcd.print(">Menu");
      lcd.print("    H/Alarma");
      lcd.setCursor(0, 1);
      lcd.print(" T/Esp");
      lcd.print("   A/Auto");
      menu_config = 1;
      break;

  }


}

//******* Funcion para ejecutar las acciones del Menu***//
//***************************************************************
//**********************************************************
//******************************************************
void ejecutar(int m) {


  // Menu de Encender Luces
  if (m == 4) {
    int salir = 0, salio = 0;
    lcd.clear();
    lcd.print("Entre el Local");
    // lcd.setCursor(0, 1);
    lcd.println(">");
    while (salir == 0) {
      customKey = teclado.getKey(); //teclado.waitForKey();
      //verificarAlarma();

      salio = verificarAlarma();
      if (irrecv.decode(&results)) {
        lcd.clear();
        //lcd.print("Local encendido");


        delay(1000);

        long a = results.value;

        luces(a);
        irrecv.resume();
        Menu();

      }
      if (salio == 1) {
        lcd.clear();
        Menu();
        salio = 0;
      }

      if (customKey) {
        salir = luces1(customKey);
        //Hacemos cero la variable para el tiempo de desactivaer el display
        activarTeclado = 0;
      }

    }


    Menu();
  }
  // Menu Configuración
  if (m == 1) {

    int salir = 0, salio = 0;

    lcd.clear();
    MenuConfig();
    while (salir == 0) {
      customKey = teclado.getKey(); //teclado.waitForKey();
      //verificarAlarma();

      salio = verificarAlarma();
      if (irrecv.decode(&results)) {
        lcd.clear();
        // lcd.print("Local encendido");


        // delay(1000);

        long a = results.value;

        luces(a);
        irrecv.resume();
        MenuConfig();

      }
      if (salio == 1) {
        lcd.clear();
        MenuConfig();
        salio = 0;
      }


      if ( customKey == '.' ) {
        menu_config++;
        lcd.clear();
        //Hacemos cero la variable para el tiempo de desactivaer el display
        activarTeclado = 0;
        digitalWrite(33, HIGH);
        MenuConfig();
      }

      if ( customKey == 'V' ) {

        //Hacemos cero la variable para el tiempo de desactivaer el display
        activarTeclado = 0;
        digitalWrite(33, HIGH);

        if (menu_config == 1)
        { salir = 1;
          menu_config = 1;
        }
        if (menu_config == 2)
        {
          int salir = 0, salio = 0;
          unavez = 1;
          lcd.clear();
          lcd.print("Camb Hora Alarma");
          lcd.setCursor(15, 1);
          lcd.write(byte(8));
          while (salir == 0) {
            salio = verificarAlarma();
            if (salio == 1) {
              lcd.print("Camb Hora Alarma");
              lcd.setCursor(15, 1);
              lcd.write(byte(8));
              salio = 0;
            }

            lcd.setCursor(0, 1);
            salir = set_hora_alarma();
            lcd.print("Hora:");

            lcd.setCursor(5, 1);
            if (hora_Alarma < 10)lcd.print("0");
            lcd.print(hora_Alarma);
            lcd.setCursor(7, 1);
            if (hora_Alarma_AM_PM == 1)
              lcd.print("  AM");
            else
              lcd.print("  PM");

          }
          lcd.clear();
          MenuConfig();

        }
        if (menu_config == 3)
        {
          int salir = 0, salio = 0;
          unavez = 1;
          lcd.clear();
          lcd.print("Camb T/Esp Alarma");
          lcd.setCursor(15, 1);
          lcd.write(byte(8));
          while (salir == 0) {
            salio = verificarAlarma();
            if (salio == 1) {
              lcd.print("Camb T/Esp Alarma");
              lcd.setCursor(15, 1);
              lcd.write(byte(8));
              salio = 0;
            }
            lcd.setCursor(0, 1);
            salir = set_tiempoEsperaAlarma();
            lcd.print("T/Esp:");
            if (tiempoEsperaAlarma < 10)lcd.print("0");
            lcd.print(tiempoEsperaAlarma);
            lcd.setCursor(9, 1);
            lcd.print("seg");
          }
          lcd.clear();
          MenuConfig();

        }

      }


    }
    // Fin del While
    Menu();
  }
  // Menu Reloj
  if (m == 3) {
    int salir = 0, salio = 0;
    // Entramos en el submenu reloj y fecha y no salimos hasta que se de menu
    lcd.clear();
    MenuReloj();
    while (salir == 0) {
      customKey = teclado.getKey(); //teclado.waitForKey();
      salio = verificarAlarma();
      if (irrecv.decode(&results)) {
        lcd.clear();
        //lcd.print("Local encendido");


        // delay(1000);

        long a = results.value;

        luces(a);
        irrecv.resume();
        MenuReloj();

      }

      if (salio == 1) {
        lcd.clear();
        MenuReloj();
        salio = 0;
      }



      if ( customKey == '.' ) {
        menu_reloj++;
        lcd.clear();
        //Hacemos cero la variable para el tiempo de desactivaer el display
        activarTeclado = 0;
        digitalWrite(33, HIGH);
        MenuReloj();
      }
      if ( customKey == 'V' ) {
        //Hacemos cero la variable para el tiempo de desactivaer el display
        activarTeclado = 0;
        digitalWrite(33, HIGH);

        if (menu_reloj == 1)
        { salir = 1;
          menu_reloj = 1;
        }

        if ( menu_reloj == 2) {
          int salirhora = 0, salio = 0;
          lcd.clear();
          lcd.print("Cambiar Hora");
          lcd.setCursor(14, 0);
          lcd.write(byte(0));
          while (salirhora == 0) {
            salio = verificarAlarma();
            if (salio == 1) {
              lcd.clear();
              lcd.print("Cambiar Hora");
              lcd.setCursor(14, 0);
              lcd.write(byte(0));
              salio = 0;
            }
            escribirhora();
            salirhora = set_hora();
          }
          lcd.clear();
          MenuReloj();
        }
        if ( menu_reloj == 3) {
          int salirfecha = 0, salio = 0;
          lcd.clear();
          lcd.print("Cambiar Fecha");
          lcd.setCursor(14, 0);
          lcd.write(byte(0));
          //delay(2000);

          while (salirfecha == 0) {

            salio = verificarAlarma();
            if (salio == 1) {
              lcd.clear();
              lcd.print("Cambiar Fecha");
              lcd.setCursor(14, 0);
              lcd.write(byte(0));
              salio = 0;
            }

            lcd.setCursor(0, 1);
            lcd.print(buscarDia());
            if (dia < 10)lcd.print("0");
            lcd.print(dia);
            lcd.print("/");
            if (mes < 10)
              lcd.print("0");
            lcd.print(mes);
            lcd.print("/");
            lcd.print(anio);
            salirfecha = set_fecha();
          }
          lcd.clear();
          MenuReloj();
        }

        if ( menu_reloj == 4) {
          mostrarFechaHora();
          MenuReloj();
        }

      }
    }
    Menu();
  }
  // Menu Alarma
  if (m == 2) {


    int salir = 0,
        salio = 0;


    // Entramos en el submenu y no salimos hasta que se de menu
    lcd.clear();
    MenuAlarma();


    while (salir == 0) {
      customKey = teclado.getKey(); //teclado.waitForKey();
      salio = verificarAlarma();
      if (irrecv.decode(&results)) {
        lcd.clear();
        //lcd.print("Local encendido");


        // delay(1000);

        long a = results.value;

        luces(a);
        irrecv.resume();
        MenuAlarma();

      }
      if (salio == 1) {
        MenuAlarma();
        salio = 0;
      }
      if ( customKey == '.' ) {
        menu_alarma++;
        lcd.clear();

        //Hacemos cero la variable para el tiempo de desactivaer el display
        activarTeclado = 0;
        digitalWrite(33, HIGH);
        MenuAlarma();
      }
      if ( customKey == 'V' ) {

        //Hacemos cero la variable para el tiempo de desactivar el display
        activarTeclado = 0;
        digitalWrite(33, HIGH);

        if (menu_alarma == 1)
        { salir = 1;
          menu_alarma = 1;
        }

        if ( menu_alarma == 2) {
          // comprobarzonas();

          salir = comprobarzonas();

          menu_alarma = 1;
          //lcd.clear();
          //MenuAlarma();
        }
        if ( menu_alarma == 3) {

          EEPROM.put(10, 0);
          setAlarma = 0;
          lcd.clear();
          lcd.print("Alarma Desact.");
          lcd.setCursor(8, 1);
          lcd.write(byte(7));
          /*lcd.setCursor(8,1);
            lcd.write(byte(0));
            lcd.setCursor(2,1);
            lcd.write(byte(1));

            lcd.setCursor(14,1);
            lcd.write(byte(2));

            lcd.setCursor(12,0);
            lcd.write(byte(3));
          */
          //lcd.setCursor(12,0);
          //lcd.write(byte(3));

          //  digitalWrite(31, HIGH);

          salir = 1;
          menu_alarma = 1;
          delay(2000);
          //  digitalWrite(31, LOW);

          //lcd.clear();
          // MenuAlarma();
        }
        if ( menu_alarma == 4) {
          cambiarPass();
          lcd.clear();
          MenuAlarma();
        }
      }
    }
    Menu();
  }



}


//******* Fin  Funcion para ejecutar las acciones del Menu***//



///***** *****////
void mostrarFechaHora() {
  lcd.clear();
  int salir1 = 0;
  // Entramos en el submenu y no salimos hasta que se de menu

  lcd.print("Fecha:");
  if (dia < 10)lcd.print("0");
  lcd.print(dia);
  lcd.print("/");
  if (mes < 10)lcd.print("0");
  lcd.print(mes);
  lcd.print("/");
  lcd.print(anio);




  escribirhora();
  startTime = millis();
  while (salir1 == 0) {



    //
    // lcd.clear();

    escribirhora();

    set_hora();
    delay(200);

    if ( (millis() - startTime) > 5000 ) {

      salir1 = 1;
      startTime = millis();
      //delay(500);



    }
  }






}

//***** *******/

//**** Funcion para la Alarma *///////

void alarmaok() {



}
//**** Fin Funcion para la Alarma *///////

//**** Funcion para las Luces *///////

int luces(long customKey) {

  if (customKey == 0x20DF22DD) {
    lcd.begin(16, 2);
    lcd.clear();

    comprobarzonas();


  }

  if (customKey == 0x20DF40BF) {
    lcd.begin(16, 2);
    lcd.clear();

    lcd.setCursor(8, 1);
    lcd.write(byte(3));
    digitalWrite(32, HIGH);
    //digitalWrite(32, HIGH);
    delay(400);
    digitalWrite(32, LOW);
    delay(1500);
    setAlarma = 0;
    EEPROM.put(10, 0);
    lcd.clear();
    lcd.print("Alarma Desact.");
    lcd.setCursor(8, 1);
    lcd.write(byte(7));



    ActivarAlarma = 0;


  }



  // Si es 1 o la tecla 1 del mando enciendo el Cuarto 1
  if ( (customKey == 0x2FD807F) || (customKey == 0x20DF8877 ) || (customKey == 0x6EAC926D))
  {
    if (cuarto1 == 0)
    { digitalWrite(22, HIGH);
      cuarto1 = 1;
      //Serial.println("Cuarto1 Encendido.....");
    }
    else
    { digitalWrite(22, LOW);
      cuarto1 = 0;
      //
      Serial.println("Cuarto2 Apagado.....");
    } irrecv.resume();
    return 1;
  }
  // Si es 2o la tecla 2 del mando enciendo el Cuarto 1
  if ( customKey == 0x2FD40Bf || customKey == 0x20DF48B7  || customKey == 0x6EAC8A75)
  {
    if (cuarto2 == 0)
    { digitalWrite(23, HIGH);
      cuarto2 = 1;
    }
    else
    { digitalWrite(23, LOW);
      cuarto2 = 0;
    } irrecv.resume();
    return 1;
  }

  // Si es 3 o la tecla 3 del mando enciendo el Baño
  if ( customKey == 0x2FDC03f || customKey == 0x20DFC837  || customKey == 0x6EAC9A65)
  {
    if (bano == 0)
    { digitalWrite(24, HIGH);
      bano = 1;
    }
    else
    { digitalWrite(24, LOW);
      bano = 0;
    } irrecv.resume();
    return 1;
  }


  // Si es 4 o la tecla 4 del mando enciendo el Cuarto 3
  if ( customKey == 0x2FD20Df || customKey == 0x20DF28D7  || customKey == 0x6EACB04F)
  {
    if (cuarto3 == 0)
    { digitalWrite(25, HIGH);
      cuarto3 = 1;
    }
    else
    { digitalWrite(25, LOW);
      cuarto3 = 0;
    } irrecv.resume();
    return 1;
  }

  // Si es 5 o la tecla 5 del mando enciendo el Sala
  if ( customKey == 0x2FDA05f || customKey == 0x20DFA857  || customKey == 0x6EACA857)
  {
    if (sala == 0)
    { digitalWrite(26, HIGH);
      sala = 1;
    }
    else
    { digitalWrite(26, LOW);
      sala = 0;
    } irrecv.resume();
    return 1;
  }
  // Si es 6 o la tecla 6 del mando enciendo el comedor
  if ( customKey == 0x2FD609f || customKey == 0x20DF6897  || customKey == 0x6EACB847)
  {
    if (comedor == 0)
    { digitalWrite(27, HIGH);
      comedor = 1;
    }
    else
    { digitalWrite(27, LOW);
      comedor = 0;
    } irrecv.resume();
    return 1;
  }
  // Si es 7 o la tecla 7 del mando enciendo el cocina1
  if ( customKey == 0x2FDE01F || customKey == 0x20DFE817  || customKey == 0x6EAC906F)
  {
    if (cocina1 == 0)
    { digitalWrite(28, HIGH);
      cocina1 = 1;
    }
    else
    { digitalWrite(28, LOW);
      cocina1 = 0;
    } irrecv.resume();
    return 1;
  }

  // Si es 8 o la tecla 8 del mando enciendo el PAsillo
  if (customKey == 0x2FD10Ef || customKey == 0x20DF18E7  || customKey == 0x6EAC8877)
  {
    if (pasillo == 0)
    { digitalWrite(29, HIGH);
      pasillo = 1;
    }
    else
    { digitalWrite(29, LOW);
      pasillo = 0;
    } irrecv.resume();
    return 1;
  }
  // Si es 9 o la tecla 9 del mando enciendo el pasillo
  if ( customKey == 0x2FD906f || customKey == 0x20DF9867  || customKey == 0x6EAC9867)
  {
    if (patio == 0)
    { digitalWrite(35, HIGH);
      patio = 1;
    }
    else
    { digitalWrite(35, LOW);
      patio = 0;
    } irrecv.resume();
    return 1;

  }

  // Si es 0 o la tecla 0 del mando APAGAR TODOOOO
  if ( customKey == 0x2FD00ff || customKey == 0x20DF08F7  || customKey == 0x6EAC28D7)
  {

    for (int i = 22; i <= 30; i++)
    {
      digitalWrite(i, LOW);
    }
    digitalWrite(35, LOW);
    comedor = 0;
    sala = 0;
    cocina1 = 0;
    cocina2 = 0;
    cuarto1 = 0;
    cuarto2 = 0;
    cuarto3 = 0;
    bano = 0;
    pasillo = 0;
    patio=0;
    irrecv.resume();
    return 1;

  }




  irrecv.resume();

  return 1;



}
//**** Fin Funcion para las Luces *****//

int luces1(char customKey) {

  // Si es 1 o la tecla 1 del mando enciendo el Cuarto 1
  if (customKey == '1')
  {
    if (cuarto1 == 0)
    { digitalWrite(22, HIGH);
      cuarto1 = 1;
      //Serial.println("Cuarto1 Encendido.....");
    }
    else
    { digitalWrite(22, LOW);
      cuarto1 = 0;
      //
      Serial.println("Cuarto2 Apagado.....");
    } irrecv.resume();
    return 1;
  }
  // Si es 2o la tecla 2 del mando enciendo el Cuarto 1
  if (customKey == '2' )
  {
    if (cuarto2 == 0)
    { digitalWrite(23, HIGH);
      cuarto2 = 1;
    }
    else
    { digitalWrite(23, LOW);
      cuarto2 = 0;
    } irrecv.resume();
    return 1;
  }

  // Si es 3 o la tecla 3 del mando enciendo el Baño
  if (customKey == '3' )
  {
    if (bano == 0)
    { digitalWrite(24, HIGH);
      bano = 1;
    }
    else
    { digitalWrite(24, LOW);
      bano = 0;
    } irrecv.resume();
    return 1;
  }


  // Si es 4 o la tecla 4 del mando enciendo el Cuarto 3
  if (customKey == '4' )
  {
    if (cuarto3 == 0)
    { digitalWrite(25, HIGH);
      cuarto3 = 1;
    }
    else
    { digitalWrite(25, LOW);
      cuarto3 = 0;
    } irrecv.resume();
    return 1;
  }

  // Si es 5 o la tecla 5 del mando enciendo el Sala
  if (customKey == '5' )
  {
    if (sala == 0)
    { digitalWrite(26, HIGH);
      sala = 1;
    }
    else
    { digitalWrite(26, LOW);

      sala = 0;
    } irrecv.resume();
    return 1;
  }
  // Si es 6 o la tecla 6 del mando enciendo el comedor
  if (customKey == '6' )
  {
    if (comedor == 0)
    { digitalWrite(27, HIGH);
      comedor = 1;
    }
    else
    { digitalWrite(27, LOW);
      comedor = 0;
    } irrecv.resume();
    return 1;
  }
  // Si es 7 o la tecla 7 del mando enciendo el cocina1
  if (customKey == '7' )
  {
    if (cocina1 == 0)
    { digitalWrite(28, HIGH);
      cocina1 = 1;
    }
    else
    { digitalWrite(28, LOW);
      cocina1 = 0;
    } irrecv.resume();
    return 1;
  }

  // Si es 8 o la tecla 8 del mando enciendo el pasillo
  if (customKey == '8' )
  {
    if (pasillo == 0)
    { digitalWrite(29, HIGH);
      pasillo = 1;
    }
    else
    { digitalWrite(29, LOW);
      pasillo = 0;
    } irrecv.resume();
    return 1;
  }
  // Si es 9 o la tecla 9 del mando enciendo el PATIO
  if (customKey == '9' )
  {
    if (patio == 0)
    { digitalWrite(34, HIGH);
      patio = 1;
    }
    else
    { digitalWrite(34, LOW);
      patio = 0;
    } 
    
    irrecv.resume();
    return 1;

  }

  // Si es 0 o la tecla 0 del mando APAGAR TODOOOO
  if (customKey == '0' )
  {

    for (int i = 22; i <= 30; i++)
    {
      digitalWrite(i, LOW);
    }
    digitalWrite(34, LOW);
    comedor = 0;
    sala = 0;
    cocina1 = 0;
    cocina2 = 0;
    cuarto1 = 0;
    cuarto2 = 0;
    cuarto3 = 0;
    bano = 0;
    pasillo = 0;
    patio=0;
    irrecv.resume();
    return 1;

  }




  irrecv.resume();

  return 1;



}



//*** Funcion para pedir contrasena******//
void cambiarPass() {
  lcd.clear();
  lcd.home();
  lcd.print("Entre clave");
  lcd.setCursor(12, 0);
  lcd.write(byte(4));

  int pas = password();

  if (pas != 0) {

    EEPROM.get( 0, PW);
    if (PW != pass && PW > 0) {
      pass = PW;
    }

    if (pas == pass) {

      lcd.clear();
      lcd.home();
      lcd.print("Nueva Clave:");
      PW = password();
      if (PW != 0) {
        EEPROM.put( 0, PW );
        lcd.clear();
        lcd.print("Salvando...");
        delay(100);
        lcd.clear();
        lcd.home();
        lcd.print("Nueva Clave");
        lcd.setCursor(1, 1);
        lcd.println("Guardada");
        delay(150);
      }
    } else
    {
      lcd.clear();
      lcd.home();
      lcd.print("Error de clave!!");
      delay(200);
      customKey = 'q';
    }
    //customKey='q';
  }


}

void contadorreloj() {
  segundos++;


  Serial.print("d");
  Serial.print(cuarto1);
  Serial.print(cuarto2);
  Serial.print(bano);
  Serial.print(cuarto3);
  Serial.print(sala);
  Serial.print(comedor);
  Serial.print(cocina1);
  Serial.print(pasillo);
  Serial.print(patio);
  
  // Serial.print("h");

  if (segundos < 10) {
    Serial.print("0");
    Serial.print(segundos);
  }
  else {
    Serial.print(segundos);
  }

  if (minutos < 10) {
    Serial.print("0");

    Serial.print(minutos);
  }
  else
  {
    Serial.print(minutos);


  }


  if (horas < 10) {
    Serial.print("0");

    Serial.print(horas);


  }
  else {
    Serial.print(horas);
  }



 if(dia<10){
  Serial.print("0");
   Serial.print(dia);
  }else{ Serial.print(dia);}
 if(mes<10){
  Serial.print("0");
  Serial.print(mes);}else{ Serial.print(mes);}
 
  Serial.print(anio);




 //PAra saber si AM o PM
 
 Serial.print(hora_Alarma_AM_PM);// AM=1 PM=2

//madamos la informacion de la Alarma 


 Serial.print(tiempoEsperaAlarma);
 Serial.print(setAlarma);
  


//FIn de Informacion de la Alarmaa

  Serial.print("#");


  
  // Para calibrar Sensor PIR
  if (segundos > 50)
    tiempoPIR = 1;

  //tiempo para desactivar display pq no esta haciendo nada
  activarTeclado++;
  if (activarTeclado == 30) {
    digitalWrite(33, LOW);
  }


  //Leemos el estado del sensor PIR cada 1 segundo
  estadoPIR = digitalRead(pinPIR);


  //if(setAlarma==1  && estadoPIR==1 && tiempoPIR==1){

  if (estadoPIR == 1 && tiempoPIR == 1 && activarPIR==1) {
    //int pinPIR=41;

    //int ledPIR=43;
    //  digitalWrite(ledPIR, HIGH);

    
   // Enciendo el PASILLO
    digitalWrite(29, HIGH);
    contarPIR++;


  }
  // setAlarma==1  &&
  if (activarPIR==1  && estadoPIR == 0 && pasillo != 1)
  {
    // digitalWrite(ledPIR, LOW);
    digitalWrite(29, LOW);
  }

  //Leemos el estado del sensor Presencia escalera cada 1 segundo
  /* estadoPRE= digitalRead(pinPRE);

    if( estadoPRE==1 && tiempoPIR==1){

    digitalWrite(28, HIGH);
    contarEsc++;

     }
     //setAlarma==1  &&
     if( estadoPRE==0 && cocina1!=1){
     digitalWrite(28, LOW);

       }
  */

  //  int pinPRE=45;
  // int estadoPRE;


  // Establecer un tiempo para encender la Alarma
  if ( setAlarma == 1  && (AlarmaZona1 == 1 || AlarmaZona2 == 1 || AlarmaZona3 == 1) ) {
    tiempo1++;

    if (segundos % 2 == 0) {
      lcd.setCursor(15, 0);
      lcd.write(byte(8));
      lcd.setCursor(cursorr, 1);

    } else
    {
      lcd.setCursor(15, 0);
      lcd.write(byte(5));
      lcd.setCursor(cursorr, 1);

    }

    int zona1delante = digitalRead(2);
    int zona2detras = digitalRead(3);
    //  int zona3= 0;//digitalRead(19);

    //Retardo de 3 segundos para que se active las luces
    if (tiempo1 == 3) {

      if ( zona1delante == HIGH || zona2detras == HIGH ) {
        // cuarto 1
        digitalWrite(22, HIGH);
        cuarto1 = 1;
        //comedor
        digitalWrite(28, HIGH);

        comedor = 1;
        //sala
        digitalWrite(26, HIGH);

        sala = 1;
        ActivarAlarma = 1;

        // cocina1 = 1;
        // cocina2 = 1;

        // cuarto2 = 1;
        // cuarto3 = 1;
        // bano = 1;
        // pasillo = 1;

      } else {

        AlarmaZona1 = 0;
        AlarmaZona2 = 0;
        AlarmaZona3 = 0;
        tiempo1 = 1;
        // Menu();



      }
    }



    if (tiempo1 == tiempoEsperaAlarma)
    { activarTeclado = 0;
      digitalWrite(33, HIGH);
      digitalWrite(32, HIGH);
      tiempo1 = 1;

      // activar el teclado cdo se active la alarma


    }

  }



  if (segundos > 59) {
    minutos++;
    segundos = 0;
  }
  if (minutos > 59) {
    horas++;
    minutos = 0;
    segundos = 0;
  }
  if (horas >= 12)
  {
    cont++;
    horas = 0;
    minutos = 0;
    segundos = 0;
    if (cont == 3)
    {
      cont = 1;
      dia++;

      if (((mes == 1) || (mes == 3) || (mes == 5) || (mes == 7) || (mes == 8) || (mes == 10) || (mes == 12)) && (dia > 31))
      {
        mes++;
        dia = 1;

      }
      if (mes == 13)
      {
        mes = 1;
        anio++;
        dia = 1;
      }


      if ( dia > 30 && (mes == 4 || mes == 6 || mes == 9 || mes == 11))
      {
        mes++;
        dia = 1;
      }

      if (((mes == 2) && (dia > 28)))
      {
        mes++;
        dia = 1;
      }

    }
  }




}

void escribirhora() {

  //lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Hora:");

  if (horas == 0)
  {
    lcd.print("12");
    lcd.print(":");
    if (minutos < 10)lcd.print("0");
    lcd.print(minutos);
    lcd.print(":");
    if (segundos < 10)lcd.print("0");
    lcd.print(segundos);
    lcd.print(" ");
    if (cont == 1)lcd.print("AM");
    if (cont == 2)lcd.print("PM");
    lcd.setCursor(0, 1);

    //delay(950);
  }

  if (horas != 0)
  {
    if (horas < 10)lcd.print("0");
    lcd.print(horas);
    lcd.print(":");
    if (minutos < 10)lcd.print("0");
    lcd.print(minutos);
    lcd.print(":");
    if (segundos < 10)lcd.print("0");
    lcd.print(segundos);
    lcd.print(" ");
    if (cont == 1)lcd.print("AM");
    if (cont == 2)lcd.print("PM");
    lcd.setCursor(0, 1);

    // delay(950);
  }


}

int set_tiempoEsperaAlarma() {
  char customKey = teclado.getKey();


  if ( customKey == '0' ) {

    tiempoEsperaAlarma++;
    return 0;

  }
  if ( customKey == '.' ) {

    tiempoEsperaAlarma--;
    return 0;

  }
  if ( customKey == 'V') {


    return 1;

  }

  return 0;

}
int set_hora() {

  char customKey = teclado.getKey();
  if ( customKey == '.' ) {

    minutos++;
    if ( minutos == 60) {
      minutos = 1;
    }
    return 0;

  }
  if ( customKey == '8' ) {

    minutos--;
    if ( minutos < 0  ) {
      minutos = 59;
    }
    return 0;

  }


  if ( customKey == '0') {

    horas++;
    if (horas == 13)
      horas = 1;

    return 0;

  }
  if ( customKey == '7') {

    horas--;
    if (horas == 0)
      horas = 1;
    return 0;

  }
  if ( customKey == '9') {

    // AM=1 PM=2
    if (cont == 2) {
      // AM=1
      cont = 1;
    }
    else {
      // PM=2
      cont = 2;
    }
    return 0;

  }
  if ( customKey == 'V') {


    return 1;

  }

  return 0;
}
int set_hora_alarma() {

  char customKey = teclado.getKey();





  if ( customKey == '0' ) {


    hora_Alarma++;
    // Hora para activar la Alarma automaticamente
    if (hora_Alarma == 13) {
      hora_Alarma = 1;
      unavez = 1;




    }
    return 0;

  }

  if ( customKey == '.') {

    // AM=1 PM=2
    if (hora_Alarma_AM_PM == 2) {
      // AM=1
      hora_Alarma_AM_PM = 1;
    }
    else {
      // PM=2
      hora_Alarma_AM_PM = 2;
    }
    return 0;

  }
  if ( customKey == 'V') {


    return 1;

  }

  return 0;
}
int set_fecha() {

  char customKey = teclado.getKey();



  if ( customKey == '4' ) {
    dia--;
    if (dia == 0)
      dia = 31;



    return 0;

  }

  if ( customKey == '7' ) {
    dia++;
    if (dia == 32)
      dia = 1;



    return 0;

  }

  if ( customKey == '0') {

    mes++;
    if (mes == 13)
      mes = 1;
    return 0;

  }

  if ( customKey == '.') {

    anio++;
    return 0;

  }
  if ( customKey == 'V') {

    return 1;

  }
  return 0;

}

String buscarDia() {

  String diasemana = "";
  n_anno = (anio - 2000);
  d_anno = n_anno / 4;
  n_dia = dia;
  n_mes = mes;

  switch (n_mes) {
    case 1:
      t_mes = 0;
      break;
    case 2:
      t_mes = 3;
      break;
    case 3:
      t_mes = 3;
      break;
    case 4:
      t_mes = 6;
      break;
    case 5:
      t_mes = 1;
      break;
    case 6:
      t_mes = 4;
      break;
    case 7:
      t_mes = 6;
      break;
    case 8:
      t_mes = 2;
      break;
    case 9:
      t_mes = 5;
      break;
    case 10:
      t_mes = 0;
      break;
    case 11:
      t_mes = 3;
      break;
    case 12:
      t_mes = 5;
      break;
    default:
      t_mes = t_mes;
      break;
  }

  r_dia = n_dia + t_mes + n_anno + d_anno + t_siglo;
  r_dia = r_dia % 7;

  switch (r_dia) {
    case 1:
      diasemana = "Lun:";

      break;
    case 2:
      diasemana = "Mar:";

      break;
    case 3:
      diasemana = "Mie:";

      break;
    case 4:
      diasemana = "Jue:";

      break;
    case 5:
      diasemana = "Vie:";

      break;
    case 6:
      diasemana = "Sab:";

      break;
    case 0:
      diasemana = "Dom:";

      break;
    default:
      diasemana = "---";

      break;

      return diasemana;





  }     return diasemana;
}


void serialEvent() {


  if (Serial.available() > 0) {

    String datos_entrada = Serial.readStringUntil("#");

    int total = datos_entrada.length();

    char datos_serie[total];


    datos_entrada.toCharArray(datos_serie, total);

    if (datos_serie[0] == 'l') {


      //Analizamos si es el primer cuarto el que mandamos a encender
      if (datos_serie[1] == '1') {

        if (cuarto1 == 0)
        { digitalWrite(22, HIGH);
          cuarto1 = 1;

        }
        else
        { digitalWrite(22, LOW);
          cuarto1 = 0;
          //

        }

      }

      //Analizamos si es el Cuarto 2 el que mandamos a encender
      if (datos_serie[1] == '2') {

        if (cuarto2 == 0)
        { digitalWrite(23, HIGH);
          cuarto2 = 1;
        }
        else
        { digitalWrite(23, LOW);
          cuarto2 = 0;
        }
      }




      //Analizamos si es el Baño el que mandamos a encender
      if (datos_serie[1] == '3') {

        if (bano == 0)
        { digitalWrite(24, HIGH);
          bano = 1;
        }
        else
        { digitalWrite(24, LOW);
          bano = 0;
        }

      }

      //Analizamos si es el Cuarto 3 el que mandamos a encender
      if (datos_serie[1] == '4') {

        if (cuarto3 == 0)
        { digitalWrite(25, HIGH);
          cuarto3 = 1;
        }
        else
        { digitalWrite(25, LOW);
          cuarto3 = 0;
        }

      }

      //Analizamos si es la Sala  el que mandamos a encender
      if (datos_serie[1] == '5') {

        if (sala == 0)
        { digitalWrite(26, HIGH);
          sala = 1;
        }
        else
        { digitalWrite(26, LOW);
          sala = 0;
        }

      }

      //Analizamos si es la Comedor  el que mandamos a encender
      if (datos_serie[1] == '6') {

        if (comedor == 0)
        { digitalWrite(27, HIGH);
          comedor = 1;
        }
        else
        { digitalWrite(27, LOW);
          comedor = 0;
        }

      }

      //Analizamos si es la Cocina  el que mandamos a encender
      if (datos_serie[1] == '7') {

        if (cocina1 == 0)
        { digitalWrite(28, HIGH);
          cocina1 = 1;
        }
        else
        { digitalWrite(28, LOW);
          cocina1 = 0;
        }

      }

      //Analizamos si el pasillo  el que mandamos a encender
      if (datos_serie[1] == '8') {

        if (pasillo == 0)
        { digitalWrite(29, HIGH);
          pasillo = 1;
        }
        else
        { digitalWrite(29, LOW);
          pasillo = 0;
        }

      }


 //Analizamos si el Patio  el que mandamos a encender
      if (datos_serie[1] == '9') {
        if (patio == 0)
        { digitalWrite(35, HIGH);
          patio = 1;
        }
        else
        { digitalWrite(35, LOW);
        patio= 0;
        
        }

      }
      \
      


      //Analizamos si es Todo apagado  y apagamos
      if (datos_serie[1] == '0') {

        for (int i = 22; i <= 30; i++)
        {
          digitalWrite(i, LOW);
        }
         digitalWrite(35, LOW);
        comedor = 0;
        sala = 0;
        cocina1 = 0;
        cocina2 = 0;
        cuarto1 = 0;
        cuarto2 = 0;
        cuarto3 = 0;
        bano = 0;
        pasillo = 0;

      }




    }//Fin de las Luces





   if(datos_serie[0] == 'a'){

    //Analizamos si cambiar el tiempo de activacion de la ALarma 

    if(datos_serie[1] == '1'){

        String t=String(datos_serie[2]);

        t+=String(datos_serie[3]);
      

       tiempoEsperaAlarma =t.toInt();
      
    }
      
  

//Analizamos si es desactivar o activar la Alarma
    if(datos_serie[1] == '2'){

  if(setAlarma==0){
     lcd.begin(16, 2);
    lcd.clear();

    comprobarzonas();
    
    
    }else{
      EEPROM.put(10, 0);
      setAlarma = 0;
      
      }
      
  
   
    }
      }


if(datos_serie[0] == 'f'){
/*
int minutos = 0;
int segundos = 0;
int horas = 6;
int dia = 1;
int mes = 1;
int anio = 2020;
int hora_Alarma_AM_PM = 2;  // AM=1 PM=2
*/
  
      String d=String(datos_serie[1]);
        d+=String(datos_serie[2]);
        dia =d.toInt();
  
   String m=String(datos_serie[3]);
        m+=String(datos_serie[4]);
        mes =m.toInt();
  
   String a=String(datos_serie[5]);
        a+=String(datos_serie[6]);
        a+=String(datos_serie[7]);
        a+=String(datos_serie[8]);
        anio =a.toInt();

 String h=String(datos_serie[9]);
        h+=String(datos_serie[10]);
        horas =h.toInt();

        

        String mi=String(datos_serie[11]);
       mi+=String(datos_serie[12]);
       minutos =mi.toInt();
        

  String am_pm=String(datos_serie[13]);
       
        int actu =am_pm.toInt();
        if(actu==0)

        hora_Alarma_AM_PM=1;
        else
        hora_Alarma_AM_PM=2;

        
  
  
  }

}



  }
