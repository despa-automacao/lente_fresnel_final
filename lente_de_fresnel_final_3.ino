#define botao_desliga 9
#define botao_liga 10
#define led 12

#define FC_HOR1 7
#define FC_HOR2 8
#define FC_VER1 11
#define FC_VER2 2

#define RELAY_HOR 6
#define RELAY_VER 4

#define motor1 5 //PWM motor vertical 
#define motor2 3// PWM motor horizontal

#define horizontal A1
#define vertical A0

int v1, v2;

int acc_motor1 = 190;
int acc_motor2 = 60;
int estado_motor_1;
int estado_motor_2;

//Variaveis media movel
#define nA 3
#define nB 3
double soma_1;
double soma_2;
float filtrada_1;
float filtrada_2;
int vetorA[nA];
int vetorB[nB];

bool ligado = false;
bool sentido_motor_vertical = true;
bool sentido_motor_horizontal = true;
bool motor_vertical = true;
bool motor_horizontal = true;

unsigned long tempo_leitura_botao;
unsigned long tempo_leitura_sensores;
unsigned long tempo_entre_entre_atuacao;
unsigned long tempo_inicializacao;
unsigned long templeitura;
int inicio_da_varredura = 0;
int acc_velocidade_vertical = 0;
int acc_velocidade_horizontal = 0;
int verifH = 0;
int verifV = 0;

void setup() {
  Serial.begin(9600);
  //ligado = true;
  inicio_da_varredura = 1;
  Serial.println("Teste");
  pinMode(FC_HOR1, INPUT); //fim de curso horizontal 1
  pinMode(FC_HOR2, INPUT); //fim de curso horizontal 2
  pinMode(FC_VER1, INPUT); //fim de curso vertical
  pinMode(FC_VER2, INPUT); //fim de curso vertical 2
  pinMode(botao_desliga, INPUT); //botao desliga
  pinMode(botao_liga, INPUT); //botao liga
  pinMode(motor1, OUTPUT); //acionamento motor 1
  pinMode(motor2, OUTPUT); //acionamento motor 2
  pinMode(RELAY_HOR, OUTPUT); //rele de direcionamento horizontal 1
  pinMode(RELAY_VER, OUTPUT); //rele de direcionamento vertical 2
  pinMode(led, OUTPUT);
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  estado_motor_1 = 0;
  estado_motor_2 = 0;

  tempo_leitura_botao = millis();
  tempo_leitura_sensores = millis();
  tempo_entre_entre_atuacao = millis();
  tempo_inicializacao = millis();

  Serial.println("Iniciado");

  //Eixo horizontal RELAY LOW FC_HOR2   //Inferior > superior  // Anti-horário
  //Eixo horixontal RELAY HIGH FC_HOR1  //Superior > inferior  // Horário
  //Eixo vertical RELAY LOW FC_VER2 // direita > esquerda // Horário
  //Eixo Vertical RELAY HIGH FC_VER1 // esquerda > direita // Anti-horário
  //Motor 1, Aceleração 2, inicio 150
  //Motor 2, Aceleração 2, inicio 60
}

void loop() {
  //ligado = true;
  if (millis() - templeitura > 50) {
    v1 = mediaMovel_1();
    //v1 = analogRead(vertical);
    v1 = 512 - v1;
    //Serial.print("V1 = ");
    //Serial.println(v1);
    v2 = mediaMovel_2();
    //v2 = analogRead(horizontal);
    v2 = 512 - v2;
    //Serial.print("V2 = ");
    //Serial.println(v2);
    templeitura = millis();
  }
  if (millis() - tempo_leitura_botao > 500) {
    Serial.println("----");
    Serial.println(v1);
    Serial.println(v2);
    Serial.println("----");
    if (digitalRead(botao_liga) == HIGH) { //liga a maquina
      ligado = true;
      Serial.println("vivo");
      if (inicio_da_varredura == 0) {
        inicio_da_varredura = 1;
        digitalWrite(led, HIGH);
        Serial.begin(9600);
      }
    }
    if (digitalRead(botao_desliga) == LOW) { //desliga a maquinadesliga a maquina no modo onde se encontra
      ligado = false;
      digitalWrite(led, LOW);
      analogWrite(motor1, LOW);
      analogWrite(motor2, LOW);
    }
    if (digitalRead(botao_desliga) == LOW && digitalRead(botao_liga) == HIGH) { //posiciona o reator para ser desativado
      ligado = false;
      // *********** pisca o led
    }
    tempo_leitura_botao = millis();
  }
  if (ligado == true) {  // maquina operando, realiza a varredura de luz
    //Serial.println("Entrou no ligado");
    if (inicio_da_varredura = 1) { //lente se posiciona para o sol nascente
      inicio_da_varredura = 2;
    }
    //  }
    //   else {    // segue o posisicionamento da lente
    if (millis() - tempo_leitura_sensores > 1000) { // tempo entre correções de angulo
      Serial.println("Entrou no millis");
      Serial.print("acc1 = ");
      Serial.println(acc_motor1);
      Serial.print("acc 2 = ");
      Serial.println(acc_motor2);
      v2 = 512 - analogRead(A1) - 25;
      v1 = 512 - analogRead(A0) + 50;
      if ( v1 < -10 ) {  //inferior > superior
        //Eixo horizontal RELAY LOW FC_HOR2
        Serial.println("Entrou desce");
        // if (verifH == 1) {
        if (digitalRead(FC_HOR2) == LOW) {
          digitalWrite(RELAY_HOR, HIGH);
          delay(500);
          analogWrite(motor2, acc_motor2);
          acc_motor2 = acc_motor2 + 1;
          if ( acc_motor2 > 255) {
            acc_motor1 = 255;
          }
          Serial.println("desce");
        }
        else {
          analogWrite(motor2, 0);
          acc_motor2 = 60;
        }
        // }
        if (verifH == 0 || verifH == 2) {
          acc_motor2 = 60;
          analogWrite(motor2, acc_motor2);
          verifH = 1;
        }
      }
      if (v1 > 10) { //superior > inferior
        Serial.println("entrou sobe");
        //if (verifH == 2) {
        // Eixo horixontal RELAY HIGH FC_HOR1
        if (digitalRead(FC_HOR1) == LOW) {
          digitalWrite(RELAY_HOR, LOW);
          delay(500);
          analogWrite(motor2, acc_motor2);
          acc_motor2 = acc_motor2 + 1;
          if ( acc_motor2 > 255) {
            acc_motor2 = 255;
          }
        }
        else {
          analogWrite(motor2, 0);
          acc_motor2 = 60;
        }
        Serial.println("sobe");
        //}
        if (verifH == 0 || verifH == 1) {
          acc_motor2 = 60;
          analogWrite(motor2, acc_motor2);
          verifH = 2;
          Serial.println("Sobe - verifh 1 ou 0");
        }
      }
      if ( v2 > 10) { //esquerdo > direito
        Serial.println("Entrou esquerda");
        //Eixo Vertical RELAY HIGH FC_VER1
        if (verifV == 1) {
          if (digitalRead(FC_VER1) == LOW ) {
            digitalWrite(RELAY_VER, HIGH);
            delay(500);
            analogWrite(motor1, acc_motor1);
            acc_motor1 = acc_motor1 + 1; //aceleração
            if ( acc_motor1 > 255) {
              acc_motor1 = 255;
            }
          }
          else {
            analogWrite(motor1, 0);
            acc_motor1 = 100;
          }
          Serial.println("esquerda");
        }
        if (verifV == 0 || verifV == 2) {
          acc_motor1 = 100;
          analogWrite(motor1, acc_motor1);
          verifV = 1;
        }
      }
      if (v2 < -10) { //direito > esquerdo
        Serial.println("Entrou direita");
        if (verifV == 2) {
          //Eixo vertical RELAY LOW FC_VER2
          if (digitalRead(FC_VER2) == LOW) {
            digitalWrite(RELAY_VER, LOW);
            delay(500);
            analogWrite(motor1, acc_motor1);
            acc_motor1 = acc_motor1 + 1;
            if ( acc_motor1 > 255) {
              acc_motor1 = 255;
            }
          }
          else {
            analogWrite(motor1, 0);
            acc_motor1 = 100;
          }
          Serial.println("direita");
        }
        if (verifV == 0 || verifV == 1) {
          acc_motor1 = 100;
          analogWrite(motor1, acc_motor1);
          verifV = 2;
          Serial.println("Direita com verifv 1 ou 0");
        }
      }

      v1 = abs(v1);
      v2 = abs(v2);
      if ( v2 < 10 ) { // lente perpendicular
        digitalWrite(motor1, LOW);
        acc_motor1 = 90;
        Serial.println("desligou motor 2");
      }
      if ( v1 < 10 ) { // lente perpendicular
        analogWrite(motor2, 0);
        acc_motor2 = 60;
        Serial.println("desligou motor 1");
      }


      tempo_leitura_sensores = millis();

      //      else {
      //        digitalWrite(motor1, LOW);
      //        digitalWrite(motor2, LOW);
      //        digitalWrite(led, LOW);
      //      }
    }
  }
}
double mediaMovel_1() {
  for (int i = nA - 1; i > 0; i--) {
    vetorA[i] = vetorA[i - 1];
  }
  double accA = 0;
  vetorA[0] = analogRead(horizontal);
  for (int i = 0; i < nA; i++) {
    accA += vetorA[i];
  }
  return accA / nA;
  //return 600;
}
double mediaMovel_2() {
  for (int i = nB - 1; i > 0; i--) {
    vetorB[i] = vetorB[i - 1];
  }
  double accB = 0;
  vetorB[0] = analogRead(vertical);
  for (int i = 0; i < nB; i++) {
    accB += vetorB[i];
  }
  return accB / nB;
  //return 400;
}
