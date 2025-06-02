# Actividad2_Grupo12
Actividad 2 de la asignatura Equipos e Instrumentación Electrónica del Grupo 12

Sistema de control de humedad mediante un servomotor<br/>
En esta sección, se lee el valor de humedad obtenido por el sensor DHT22, activando un sevomotor cuando el nivel de humedad relativa supera el 50%,simulando una turbina que permite la liberación de humedad cuando esta es demasiado elevada, esta funcionalidad se implementa en la siguiente función:

void humidityServo(float humidity){
  int pos; // int variable for controlling the servo position
  //if humidity > 50% -> servo start acting
  if ((float)humidity>=50){ 
    //logic for the movement of the servo
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 360; pos += 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  }
}<br/>


-------------- PARTE DE LUCÍA ------------------<br/>
Control del brillo del LED según la luz ambiental (LDR)

Esta sección del código lee el nivel de luz ambiental utilizando una LDR (resistencia dependiente de la luz), lo convierte en un valor de lux y ajusta el brillo de un LED de acuerdo a dicho valor:

loat lux = readLux();
int brightness = 0;

if (lux <= 20) {
    // Si la luz (lux) es baja, el LED se enciende al máximo brillo
    brightness = 255;
} else if (lux >= 20 && lux <= 2000) {
    // Si la luz es moderada, el LED se enciende con brillo medio
    brightness = 60;
} else {
    // Si hay mucha luz, el LED se apaga
    brightness = 0;
}

Explicación:
	•	Luz baja (lux ≤ 20):
El entorno está oscuro, por lo que el LED se enciende con brillo máximo (brightness = 255) para aportar visibilidad.
	•	Luz moderada (20 < lux ≤ 2000):
En condiciones de luz intermedia, el brillo del LED se ajusta a un nivel medio (brightness = 60), ya que no se requiere tanta iluminación.
	•	Luz alta (lux > 2000):
Cuando hay mucha luz ambiente, el LED se apaga (brightness = 0) porque no es necesario iluminar más.

Este comportamiento permite que el LED actúe como un indicador dinámico o fuente de luz ambiental, reaccionando de forma inteligente a los cambios en la iluminación del entorno.

-------------- PARTE DE LÍA ------------------<br/>

El sistema utiliza un sensor DHT22 para medir la temperatura ambiente, esta lectura se muestra en una pantalla LCD. Para responder a las variaciones de temperatura, se han incorporado dos servomotores conectados a los pines digitales 3 y 2, que simulan distintos mecanismos de climatización. Cuando la temperatura supera los 28 °C, el servomotor con aspas rojas, se activa para simular la ventilación. En cambio, si la temperatura baja de los 22 °C, entra en acción el segundo servomotor, con un brazo amarillo, que representa un sistema de calentamiento. En el rango intermedio (22 °C - 28 °C), ambos servomotores permanecen inactivos.

void tempServo(float temperature){
    // Control of the fan (high temperature)
    if (temperature > 28) {
      for (int p = 0; p <= 180; p += 10) {
      fanServo.write(p);
      delay(15);
      }
      for (int p = 180; p >= 0; p -= 10) {
      fanServo.write(p);
      delay(15);
      }
    } else if (temperature < 22) {
      for (int p = 0; p <= 180; p += 10) {
      fanServo2.write(p);
      delay(15);
      }
      for (int p = 180; p >= 0; p -= 10) {
      fanServo2.write(p);
      delay(15);
      }
    } else {
      fanServo.write(0);
      fanServo2.write(0);
    }
}

Explicación:
	•	Temperatura baja (temp < 22ºC):
El entorno es demasiado frío, por lo que se activa el servomotor amarillo que acciona un sistema en respuesta al frío.
	•	Temperatura alta (temp > 28ºC):
En condiciones cálidas se activa el servomotor rojo, que representa un ventilador en respuesta al calor.
