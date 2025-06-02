# Actividad 2 Grupo 12
Actividad 2 de la asignatura Equipos e Instrumentación Electrónica del Grupo 12

## 1. Introducción

### 1.1. Objetivo

El objetivo de la presente práctica ha sido diseñar e implementar un sistema integrado que permita la medición de variables climáticas (temperatura, humedad y luminosidad), el control y actuación sobre actuadores (servomotores y LEDs) en función de dichas mediciones, y la presentación de la información relevante en una interfaz LCD. 

### 1.2. Pasos de Desarrollo
 
 A lo largo del desarrollo, se han seguido los pasos recomendados para:
	
+ Extender el sistema de medición de la Actividad 1 incorporando nuevos sensores y acondicionadores
+ Definir e implementar algoritmos de control ON-OFF para regular temperatura y humedad
+ Definir e implementar histéresis en los algoritmos de control para mejorar la estabilidad en entornos reales
+ Programar la lectura de una LDR para ajustar el brillo de un LED de forma proporcional
+ Integrar todos los componentes en una sola plataforma (Arduino Uno) y presentar la información en un display LCD
+ Realizar pruebas de funcionamiento y documentar resultados
+ Evaluar ventajas y desventajas del sistema creado

## 2. Diseño del Sistema

### 2.1. Componentes Hardware

Microcontrolador:
	
+ Arduino Uno
	
Sensores:

+ DHT22 (temperatura y humedad)
+ LDR (resistencia dependiente de la luz)
	
Actuadores:

+ Dos servomotores para climatización (calefacción/ventilación)
+ Servomotor para control de humedad (simulación de turbina)
+ LED cuya luminosidad se regula según nivel de luz ambiente

Pantalla de Presentación:

+ Display LCD 16×2 con interfaz I2C
	
Alimentación:

+ Fuente de 5 V (del propio Arduino o módulo externo en caso de demanda mayor)

### 2.2. Diagrama de Conexiones

Se ha diseñado el siguiente esquema de conexión (simulado en Wokwi):

+ DHT22 conectado a pin digital D12 (datos)
+ LDR conectada a A0 con una resistencia de pull-down de 10 kΩ
+ Servomotor 1 (ventilación) conectado a pin digital D2
+ Servomotor 2 (calefacción) conectado a pin digital D3
+ Servomotor adicional para control de humedad conectado a pin digital D4
+ LED de indicación de luminosidad conectado a pin PWM D5
+ Display LCD I2C conectado a los pines SDA (A4) y SCL (A5) del Arduino

## 3. Implementación del Sistema

A continuación se resumen los pasos seguidos para la implementación del sistema:
 
### 3.1. General

Los parámetros de implementación general del sistema son:

+ Se ha definido un tiempo de sincronización aproximado de 100ms, por lo que todas las funciones se ejecutan cada 100ms.
+ Se han definido macros para los parámetros de los actuadores, como valores máximos y mínimos.
+ La única función bloqueante se encuentra en el loop(), para impedir una violación del sincronismo.

### 3.2. Lectura de Sensores y Presentación Inicial (Actividad 1)

Previamente, en la Actividad 1, se había desarrollado la parte de medición y presentación de las variables de temperatura, humedad y luminosidad en el LCD.
 	
Para ello:

+ Se utilizó la librería DHT.h para leer datos de temperatura y humedad desde el sensor DHT22
+ Se implementó la lectura analógica de la LDR para obtener un valor bruto entre 0 y 1023, que se convertía a lux aproximados
+ Se empleó la librería LiquidCrystal_I2C.h para mostrar en pantalla LCD los valores actuales de temperatura, humedad y lux
+ Se validaron las lecturas en Wokwi, asegurando que los datos presentados en la pantalla concordaban con las variaciones simuladas de los sensores
+ Estos pasos iniciales sentaron las bases para la integración de los algoritmos de control y actuación

### 3.3. Control y Actuación sobre Humedad

	En esta sección, se lee el valor de humedad obtenido por el sensor DHT22, activando un sevomotor cuando el nivel de humedad relativa supera el 80%, con una histéresis del 10%, simulando una turbina que permite la liberación de humedad cuando esta es demasiado elevada. 
 
Las macros que definen el comportamiento del actuador son :

 	#define MAX_HUMIDITY 80 //Humedad máxima en porcentaje
	#define HUMIDITY_BIAS 10 //Histéresis

Esta funcionalidad se implementa en la siguiente función:

	bool controlHumidity(float humidity){
	  static int position = 0;
	  static bool positiveTurning = true;
	  static bool isOn = false;
	
	  //If the maximun temperature has been reached
	  //or the system is in ON state and the (MAX_TEMPERATURE - TEMPERATURE_BIAS) has not been reached yet
	  if((humidity > MAX_HUMIDITY) || (isOn && humidity >= (MAX_HUMIDITY - HUMIDITY_BIAS))){
	    moveServo(dryingServo, position, positiveTurning);
	    isOn = true;
	  }
	  else{
	    isOn = false;
	  }
	
	  return isOn;
	}

### 3.4. Control del brillo del LED según la luz ambiental (LDR)

Esta sección del código lee el nivel de luz ambiental utilizando una LDR (resistencia dependiente de la luz), lo convierte en un valor de lux y ajusta el brillo de un LED.

El LED comienza a brillar a un 50% de su potencia total con 6000 lux, de forma progresiva, aumenta este valor al 100% de potencia al llegar a 1000 lux.

Las macros que definen el comportamiento del actuador son :

 	#define MIN_BRIDHTNESS 50 //Porcentaje de brillo del LED mínimo
	#define BRIDHTNESS_PER_THOUSAND_LUX 10 //Incremento de potencia en porcentaje del LED cada 1000 lux
	#define MIN_LUX 6000 //Lux mínima para el encendido del LED
  
La función que implementa este comportamiento es:
	
	//Controls the bridhtness actuator
	//Returns the percent of power the actuator is emiting
	unsigned int controlBrightness(float lux){
	  unsigned int powerPercent = 0;
	
	  //If the minimux level has been reached
	  if(lux < MIN_LUX){
	    //Calculate the power percent
	    powerPercent = MIN_BRIDHTNESS + (BRIDHTNESS_PER_THOUSAND_LUX * (-1 * (lux - MIN_LUX) / 1000));
	
	    if(powerPercent > 100){
	      powerPercent = 100;
	    }
	
	    setPWM_Percent(LED_PIN, powerPercent);
	  }
	  else{
	    powerPercent = 0;
	  }
	
	  setPWM_Percent(LED_PIN, powerPercent);
	
	  return powerPercent;
	}

### 3.5. Control de Temperatura con Dos Servomotores
	
El sistema utiliza un sensor DHT22 para medir la temperatura ambiente. 

Para responder a las variaciones de temperatura, se han incorporado dos servomotores que simulan distintos mecanismos de climatización. 

Cuando la temperatura supera los 28 °C, el servomotor con aspas azules, se activa para simular la ventilación. 

En cambio, si la temperatura baja de los 22 °C, entra en acción el segundo servomotor, con un brazo rojo, que representa un sistema de calefacción.

Se ha añadido una histéresis de 2º a ambos actuadores, por lo que se ha definido una zona muerta entre 22º y 28º, con una histéresis de 2º.

Las macros que definen el comportamiento de los actuadores son :
 
 	#define MIN_TEMPERATURE 21 //Temperatura máxima
	#define MAX_TEMPERATURE 27 //Temperatura mínima
	#define TEMPERATURE_BIAS 2 //Histéresis
  	(Los valores 21 y 27 se han definido ya que el microcontrolador trunca los valores decimales del sensor de temperatura)

Esta función controla el comportamiento del actuador de ventilación:
	
	//Control cooling system (servo)
	//Returns if the heating system is on
	bool controlCooling(int temperature){
	  static int position = 0;
	  static bool positiveTurning = true;
	  static bool isOn = false;
	
	  //If the maximun temperature has been reached
	  //or the system is in ON state and the (MAX_TEMPERATURE - TEMPERATURE_BIAS) has not been reached yet
	  if((temperature > MAX_TEMPERATURE) || (isOn && temperature >= ((MAX_TEMPERATURE - TEMPERATURE_BIAS)))){
	    moveServo(coolingServo, position, positiveTurning);
	    isOn = true;
	  }
	  else{
	    isOn = false;
	  }
	
	  return isOn;
	}


Esta función controla el comportamiento del actuador de calefacción:

	//Control heating system (servo)
	//Returns if the heating system is on
	bool controlHeating(int temperature){
	  static int position = 0;
	  static bool positiveTurning = true;
	  static bool isOn = false;
	
	  //If the minimun temperature has been reached
	  //or the system is in ON state and the (MAX_TEMPERATURE - TEMPERATURE_BIAS) has not been reached yet
	  if((temperature < MIN_TEMPERATURE) || (isOn && temperature <= ((MIN_TEMPERATURE + TEMPERATURE_BIAS)))){
	    moveServo(heatingServo, position, positiveTurning);
	    isOn = true;
	  }
	  else{
	    isOn = false;
	  }
	
	  return isOn;
	}

### 3.6. Integración de la Presentación en LCD

El sistema muestra los valores leidos por los sensores y el estado de los actuadores en un LCD de 4x20 píxeles.

Cada 5s, la pantalla cambia de modo para mostrar los sensores o los actuadores; el modo actual se muestra en la primera línea.

La función de control de la pantalla y sus subalgoritmos son excesivamente largos para ser reflejados aquí, se puede resumir en que hay las siguientes funciones:
 
+ Función principal que recibe todos los valores y decide qué modo (sensores o actuadores) mostrar:

	+ `void lcdPrint(float temperature, float humidity, float lux, bool coolingOn, bool heatingOn, bool dryingOn, unsigned int lightPercent)`
  
+ Función que imprime los valores leidos de los sensores (forcePrint fuerza una actualización de la pantalla, se usa en caso de cambiar de modo o de error):

	+ `void printSensorReadings(float temperature, float humidity, float lux, bool forcePrint)`
  
+ Función que imprime el estado de los actuadores:

	+ `void printActuatorStatus(bool coolingSystemStatus, bool heatingSystemStatus, bool humiditySystemStatus, float lightSystemStatus, bool forcePrint)`
    
+ Función que imprime una cadena de carácteres en una columna y fila específica; devuelve el número de carácteres escritos:

	+ `int lcdPrint(const char* text, const int column, const int row)`
      
+ Función que imprime un número flotante en una columna y fila específica; devuelve el número de carácteres escritos:

	+ `int lcdPrint(const float number, const int column, const int row)`
 
+ Función que imprime un carácter definido por el usuario en una columna y fila específica; devuelve 1 para mantener el estílo de código:

	+ `int lcdPrintCustom(const int characterID, const int column, const int row)`
   
+ Función que imprime un título en la pantalla:

	+ `void printTitle(const char* title, const char character)`

A parte de los 2 modos anteriormente expuestos, la pantalla también muestra errores de lectura del sensor, si es que estos se producen.
	
## 4. Pruebas Realizadas

+ Comprobación de lecturas del sensor DHT22 (temperatura y humedad)
+ Verificación de lectura de valores en lux con LDR
+ Ensayo de activación del servomotor de humedad al superar el umbral
+ Ensayo de control de brillo del LED según diferentes niveles de lux
+ Prueba de activación de ambos servomotores de temperatura en función de los límites establecidos
+ Ejecución continua del sistema para confirmar la integración de todos los componentes en el loop()

## 5. Ventajas y Desventajas del Sistema Creado

### 5.1 Ventajas:

+ Diseño modular: funciones de control separadas facilitan mantenimiento y ampliaciones
+ Componentes económicos y fáciles de conseguir
+ Respuesta rápida a cambios de variables climáticas
+ Simulación previa en Wokwi agiliza pruebas
+ Histéreis en el control de la temperatura y la humedad

### 5.2 Desventajas:

+ La falta de histéresis en el control de iluminación podría causar oscilaciones y encendidos / apagados frecuentes.
+ Movimiento completo de servos en cada activación (ineficiente para un actuador real)
+ En hardware real, consumo de servos puede requerir fuente externa
+ Ausencia de registro histórico de datos (no hay almacenamiento ni transmisión remota)
