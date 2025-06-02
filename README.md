# Actividad2_Grupo12
Actividad 2 de la asignatura Equipos e Instrumentación Electrónica del Grupo 12

-------------- PARTE DE SAMUEL ------------------<br/>
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
