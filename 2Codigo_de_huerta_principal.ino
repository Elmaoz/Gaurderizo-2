#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


// Dirección I2C de la LCD
#define LCD_ADDRESS 0x27

// Inicialización del objeto LCD
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

// Definicion de los pines del encoder
const int salida_A = 6;    //CLK 
const int salida_B = 5;    //DT
const int swPin  = 4;      //SW

int contador = 0; 

int A_estado_actual;
int A_ultimo_estado;

int velocidad_scroll = 300;

// Definición de pines para el sensor ultrasónico HC-SR04
const int Trigger =  3;
const int Echo = 2;


// Definición de pines para las bombas de agua
#define BOMB1_ENA 9
#define BOMB1_IN1 10
#define BOMB1_IN2 11

// Variables para calcular la distancia con el sensor ultrasónico
float t;
int d;

int humedad;

// Umbrales de humedad (ajusta según tus necesidades)
int umbralSeco = 900; // Valor aproximado cuando el suelo está seco
int umbralHumedo = 700; // Valor aproximado cuando el suelo está húmedo
int umbralEmpapado = 300; // Valor aproximado cuando el suelo está empapado

//::::::::::MENU::::::::::::::::::::::

String menu1[] = {"Humedad","Distancia"};  //Inicializamos nuestro Array con los elementos del menu
int sizemenu1 = sizeof(menu1) / sizeof(menu1[0]);                           //Obtenemos el número de elementos ocupados en la matriz. en este caso 6     


String menu2[] = {"Humedad: " + humedad,"Atras"};
int sizemenu2 = sizeof(menu2) / sizeof(menu2[0]);

String menu3[] = {"Distancia: " + d,"Atras"};
int sizemenu3 = sizeof(menu3) / sizeof(menu3[0]);


String linea1,linea2;           //Lineas del LCD
int seleccion = 0;
int pos = 0;
int level_menu = 0;             //Iniciamos la variable en el menu principal 0 --> 6
int opcion = 0;
bool btnpress = false;          //Esta variable de retorno en false aegurando que el boton del Encoder aun no se ha oprimido
byte flecha[] = {B10000,B11000,B11100,B11110,B11100,B11000,B10000,B00000};      //Creamos un array de 8 posiciones para la flecha del menu

void setup() {
 
  Serial.begin(9600); // Inicialización del puerto serial
  
  pinMode (salida_A,INPUT);
  pinMode (salida_B,INPUT);
  pinMode (swPin,INPUT_PULLUP);


  // Inicialización de la LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, flecha);              //Caracter personalizado       
  fn_creditos();
  fn_menu(contador,menu2,sizemenu2);      //Iniciamos presentando el menu principal
  
  A_ultimo_estado = digitalRead(salida_A);         //Leemos el estado de la salida del Encoder usando el pin CLK
  
  delay(2000); // Esperar 2 segundos antes de continuar
}

void loop() {
  // put your main code here, to run repeatedly:





// Sensor Ultrasonico
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);
  t = pulseIn(Echo, HIGH);
  d = t/59;
  Serial.print(" Distancia:");
  Serial.print(d);
  delay(3000);
  Serial.println();

//Sensor de humedad
  humedad = analogRead(A2);
  Serial.print("La humedad de la tierra en el primer sensor es: ");
  Serial.println(humedad);
  delay(2000);

  selectOption();                             //Funcion para detectar cuando se oprime el encoder.  btnpress == true


  
  if(level_menu == 0){                        //Esta variable corresponde al nivel principal del menu. level_menu = 0.
    //Menu principal
    if(fn_encoder(sizemenu1) ){               //Esta funcion muestra en el LCD el menu en el que estamos
      fn_menu(contador,menu1,sizemenu1);      //Esta funcion muestra la posicion dentro de ese menu segun el valor de la variable contador
    }
 
    if(btnpress){                             //Verificamos si el boton del encoder fue oprimido. btnpress == true
      //Humedad      
      if(contador == 0){
          contador = 0;                       //Seleccionamos la posicion donde quedara la flecha de seleccion. On, Off, Atras
          fn_menu(contador,menu2,sizemenu2);  //Vamos a la funcion que nos muestra las opciones para el Led 1
          level_menu = 1;                     //Esta variable corresponde al nivel secundario del menu. level_menu = 1.                  
      }

      //Distancia
      if(contador == 1){
          contador = 0;
          fn_menu(contador,menu3,sizemenu3);
          level_menu = 2;  
      }

      btnpress = false;                               //  Nos aseguramos que esta variable de retorno de la funcion selectOption() vuelva a su estado inicial
    }

  }
  

  //SubMenu Humedad

  if(level_menu == 1){                                 

    if(fn_encoder(sizemenu2)){                         //Nos desplazamos con el encoder sleccionando las diferentes opciones
      fn_menu(contador,menu2,sizemenu2);
    }
    
    if(btnpress){                                      //Verificamos si el boton del encoder fue oprimido. btnpress == true      
      if(contador == 1){
          contador = 0;
          fn_menu(contador,menu1,sizemenu1);
          level_menu = 0;   
      }
      
      btnpress = false;
    }
  }
  

  //SubMenu Distancia

  if(level_menu == 2){

    if(fn_encoder(sizemenu2)){
      fn_menu(contador,menu2,sizemenu2);
    }
    
    if(btnpress){
      if(contador == 1){
          contador = 1;
          fn_menu(contador,menu1,sizemenu1);
          level_menu = 0;   
      }
      
      btnpress = false;
    }
  }


  
}

void selectOption(){
  if(digitalRead(swPin) == LOW){
    delay(500);
    btnpress = true;
  }
}

void fn_contador_brillo(){
  
         A_estado_actual = digitalRead(salida_A); 
  
         if (A_estado_actual != A_ultimo_estado){     

          if (digitalRead(salida_B) != A_estado_actual){        //DT != CLK  ``````````    
          contador ++;
          delay(250);     
          }
          else {
          contador --;
          delay(250); 
          } 
        }
                     
        if(contador>2){
        contador = 2;
        }
        if(contador<0){
          contador = 0;
        }
        
}

void fn_creditos(){

  // Mostrar mensaje inicial
  lcd.setCursor(0, 0);
  lcd.print("Gauderizo");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  
  for (int positionCounter = 0; positionCounter < 21; positionCounter++) {  
        
  lcd.scrollDisplayLeft();             //Desplaza el texto del LCD hacia la Izquierda
  delay(velocidad_scroll);

  lcd.clear();
  lcd.setCursor(0, 0);
  
  }
    
}


void fn_menu(int pos,String menus[],byte sizemenu){
  lcd.clear();
  linea1="";
  linea2="";
   
  if((pos % 2) == 0){  

     lcd.setCursor(0, 0);
     lcd.write(byte(0));
     linea1 = menus[pos];
    
    if(pos+1 != sizemenu){
      linea2 = menus[pos+1];
    }
    
  }else{
    linea1 = menus[pos-1];
    lcd.setCursor(0, 1);
    lcd.write(byte(0));
    linea2 = menus[pos];
  }
  
     lcd.setCursor(1, 0);
     lcd.print(linea1);

     lcd.setCursor(1, 1);
     lcd.print(linea2); 
   
}


bool fn_encoder(byte sizemenu){ 
  bool retorno = false;

  A_estado_actual = digitalRead(salida_A); 
  
  if (A_estado_actual != A_ultimo_estado){     

    if (digitalRead(salida_B) != A_estado_actual){        //DT != CLK     
     contador ++;
     delay(250);     
    }
    else {
     contador --;
     delay(250); 
    }
    
    if(contador <=0){
      contador = 0;
    }
    
    if(contador >= sizemenu-1 ){
      contador = sizemenu-1;
    }
    
    retorno = true;

  } 

  return retorno; 
}


