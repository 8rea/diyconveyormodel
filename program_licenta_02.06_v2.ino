#define enA 9 //initializam portul pentru viteza motorului
#define in1 5 //initializam pini pentru sensul motorului
#define in2 6 //initializam pini pentru sensul motorului
#define hallSensor1 2 // initializam pini pentru primul sensor Hall
#define hallSensor2 3 // initializam pini pentru al doilea sensor Hall
#define button 8 // initializam pini pentru butonul de avarie

int motorDirection = 1; // initializam functia pentru directia motorului,1 insemnand inainte si -1 insemnand inapoi
int motorPaused = 0; // initializam functia pentru oprirea motorului de urgenta, 1 insemnand oprit, 0 insemnand activ
int buttonState = HIGH; // initializam functia pentru starea a butonului
int lastButtonState = HIGH; // initializam functia pentru ultima starea a butonului

unsigned long previousMillisPWM = 0; // pentru temporizare cu millis() pentru afisarea PWM Output
unsigned long previousMillisHall = 0; // pentru temporizare cu millis() pentru afisarea starii senzorilor Hall
const long intervalPWM = 1000; // intervalul la care actualizam Serial Monitorul pentru PWM Output (1000 ms)
const long intervalHall = 500; // intervalul la care actualizam Serial Monitorul pentru starea senzorilor Hall (500 ms)

void setup() {
  pinMode(enA, OUTPUT); // setam pinii ca iesiri
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  pinMode(hallSensor1, INPUT); // setam pinii ca intrari
  pinMode(hallSensor2, INPUT);
  pinMode(button, INPUT_PULLUP); // setam pinul cu un rezistor intern de ridicare

  Serial.begin(9600); // Initializam comunicarea seriala la 9600 bps
  
  digitalWrite(in1, LOW); //setam directia initala a motorului
  digitalWrite(in2, HIGH);
}

void loop() {
  unsigned long currentMillis = millis(); // Citim timpul curent

  int hallSensor1State = digitalRead(hallSensor1); //citim valoarea senzorilor Hall
  int hallSensor2State = digitalRead(hallSensor2);
  
  buttonState = digitalRead(button); //citim valoarea butonului

  // Verificam daca butonul si-a schimbat starea
  if (buttonState != lastButtonState) {
    // Daca butonul nu si-a schimbat starea, rescrie ultima valoare a butonului
    if (buttonState == LOW) {
      // Daca butonul si-a schimbat starea, opreste motorul
      motorPaused = !motorPaused;
      Serial.print("Motor paused: ");
      Serial.println(motorPaused);
    }
    // Rescrie ultima valoare a butonului
    lastButtonState = buttonState;
  }

  // Daca motorul nu este in avarie, schimba directia motorului
  if (!motorPaused) {
    if (hallSensor1State == HIGH && hallSensor2State == LOW) {
      // Daca primul senzor Hall este activat si al doilea senzor Hall nu este activat, directia motorului este inainte
      if (motorDirection != 1) {
        motorDirection = 1;
        Serial.println("Motor direction: Forward");
      }
    } else if (hallSensor1State == LOW && hallSensor2State == HIGH) {
      // Daca primul senzor Hall nu este activat si al doilea senzor Hall este activat, directia motorului este inapoi
      if (motorDirection != -1) {
        motorDirection = -1;
        Serial.println("Motor direction: Backward");
      }
    }

    // Seteaza directia motorului
    if (motorDirection == 1) {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    } else {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    }

    // Afisam PWM Output la fiecare interval de timp specificat
    if (currentMillis - previousMillisPWM >= intervalPWM) {
      previousMillisPWM = currentMillis; // actualizam ultima perioada pentru afisarea PWM Output

      // Controleaza viteza motorului
      int potValue = analogRead(A0); // citeste valoarea potentiometrului
      int pwmOutput = map(potValue, 1023, 0, 0, 255); // Tradu valoarea potentiometrului in valoarea semnalului PWM
      Serial.print("PWM Output: ");
      Serial.println(pwmOutput);
    }
  } else {
    // Daca motorul este in avarie, opreste motorul
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enA, 0); // Set PWM to 0
  }

  // Afisam starea senzorilor Hall la fiecare interval de timp specificat
  if (currentMillis - previousMillisHall >= intervalHall) {
    previousMillisHall = currentMillis; // actualizam ultima perioada pentru afisarea starii senzorilor Hall

    Serial.print("Hall Sensor 1: ");
    Serial.print(hallSensor1State);
    Serial.print(", Hall Sensor 2: ");
    Serial.println(hallSensor2State);
  }
}