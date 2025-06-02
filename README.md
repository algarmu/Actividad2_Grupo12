# Actividad2_Grupo12
Actividad 2 de la asignatura Equipos e Instrumentación Electrónica del Grupo 12

1. Introducción

	El objetivo de la presente práctica ha sido diseñar e implementar un sistema integrado que permita la medición de variables climáticas (temperatura, humedad y luminosidad), el control y actuación sobre actuadores (servomotores y LEDs) en función de dichas mediciones, y la presentación de la información relevante en una interfaz LCD. A lo largo del desarrollo, se han seguido los pasos recomendados para:
	
	Extender el sistema de medición de la Actividad 1 incorporando nuevos sensores y acondicionadores
	Definir e implementar algoritmos de control ON-OFF para regular temperatura y humedad
	Programar la lectura de una LDR para ajustar el brillo de un LED de forma proporcional
	Integrar todos los componentes en una sola plataforma (Arduino Uno) y presentar la información en un display LCD
	Realizar pruebas de funcionamiento y documentar resultados
	Evaluar ventajas y desventajas del sistema creado

2. Diseño del Sistema

2.1. Componentes Hardware

	Microcontrolador:
		Arduino Uno
	
	Sensores:
		DHT22 (temperatura y humedad)
		LDR (resistencia dependiente de la luz)
	
	Actuadores:
		Dos servomotores para climatización (calefacción/ventilación)
		Servomotor para control de humedad (simulación de turbina)
		LED cuya luminosidad se regula según nivel de luz ambiente

	Pantalla de Presentación:
		Display LCD 16×2 con interfaz I2C
	
	Alimentación:
		Fuente de 5 V (del propio Arduino o módulo externo en caso de demanda mayor)

2.2. Diagrama de Conexiones

	Se ha diseñado el siguiente esquema de conexión (simulado en Wokwi):
		DHT22 conectado a pin digital D4 (datos)
		LDR conectada a A0 con una resistencia de pull-down de 10 kΩ
		Servomotor 1 (ventilación) conectado a pin digital D3
		Servomotor 2 (calefacción) conectado a pin digital D2
		Servomotor adicional para control de humedad conectado a pin digital D9
		LED de indicación de luminosidad conectado a pin PWM D5
		Display LCD I2C conectado a los pines SDA (A4) y SCL (A5) del Arduino

3. Implementación del Sistema

	A continuación se resumen los pasos seguidos para la implementación del sistema:

3.1. Lectura de Sensores y Presentación Inicial (Actividad 1)

	Previamente, en la Actividad 1, se había desarrollado la parte de medición y presentación de las variables de temperatura, humedad y luminosidad en el LCD. Para ello:
		Se utilizó la librería DHT.h para leer datos de temperatura y humedad desde el sensor DHT22
		Se implementó la lectura analógica de la LDR para obtener un valor bruto entre 0 y 1023, que se convertía a lux aproximados
		Se empleó la librería LiquidCrystal_I2C.h para mostrar en pantalla LCD los valores actuales de temperatura, humedad y lux
		Se validaron las lecturas en Wokwi, asegurando que los datos presentados en la pantalla concordaban con las variaciones simuladas de los sensores
		Estos pasos iniciales sentaron las bases para la integración de los algoritmos de control y actuación

3.2. Control y Actuación sobre Humedad

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



3.3. Control del brillo del LED según la luz ambiental (LDR)

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

3.4. Control de Temperatura con Dos Servomotores
	
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


3.5. Integración de la Presentación en LCD

##################################cristobal##################################################

4. Pruebas Realizadas

	Comprobación de lecturas del sensor DHT22 (temperatura y humedad)
	Verificación de lectura de valores en lux con LDR
	Ensayo de activación del servomotor de humedad al superar el umbral
	Ensayo de control de brillo del LED según diferentes niveles de lux
	Prueba de activación de ambos servomotores de temperatura en función de los límites establecidos
	Ejecución continua del sistema para confirmar la integración de todos los componentes en el loop()

5. Ventajas y Desventajas del Sistema Creado

5.1 Ventajas:

	Diseño modular: funciones de control separadas facilitan mantenimiento y ampliaciones
	Componentes económicos y fáciles de conseguir
	Respuesta rápida a cambios de variables climáticas
	Simulación previa en Wokwi agiliza pruebas

5.2 Desventajas:

	Control ON-OFF sin histéresis ni regulación proporcional → posibles oscilaciones frecuentes
	Movimiento completo de servos en cada activación (ineficiente para un actuador real)
	Brillo de LED con solo tres niveles → transiciones bruscas
	En hardware real, consumo de servos puede requerir fuente externa
	Ausencia de registro histórico de datos (no hay almacenamiento ni transmisión remota)
