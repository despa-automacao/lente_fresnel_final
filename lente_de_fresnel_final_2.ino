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

#define ldr_sup_esq  A0
#define ldr_sup_dir  A3
#define ldr_inf_esq  A2
#define ldr_inf_dir  A1

int v1, v2, v3, v4;

int acc_motor1 = 190;
int acc_motor2 = 60;

bool ligado = false;
bool sentido_motor_vertical = true;
bool sentido_motor_horizontal = true;
bool motor_vertical = true;
bool motor_horizontal = true;

unsigned long tempo_leitura_botao;
unsigned long tempo_leitura_sensores;
unsigned long tempo_entre_entre_atuacao;
//unsigned long tempo_

int inicio_da_varredura = 0;
int acc_velocidade_vertical = 0;
int acc_velocidade_horizontal = 0;


void setup() {
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
  tempo_leitura_botao = millis();
  tempo_leitura_sensores = millis();
  tempo_entre_entre_atuacao = millis();

  //Eixo horizontal RELAY LOW FC_HOR2   //Inferior > superior  // Anti-horário
  //Eixo horixontal RELAY HIGH FC_HOR1  //Superior > inferior  // Horário
  //Eixo vertical RELAY LOW FC_VER2 // direita > esquerda // Horário
  //Eixo Vertical RELAY HIGH FC_VER1 // esquerda > direita // Anti-horário
  //Motor 1, Aceleração 2, inicio 150
  //Motor 2, Aceleração 2, inicio 60
}

void loop() {
  if (millis() - tempo_leitura_botao > 500) {
    if (digitalRead(botao_liga) == HIGH) { //liga a maquina
      ligado = true;
      if (inicio_da_varredura == 0) {
        inicio_da_varredura = 1;
        digitalWrite(led, HIGH);
        Serial.begin(9600);
      }
    }
    if (digitalRead(botao_desliga) == LOW) { //desliga a maquinadesliga a maquina no modo onde se encontra
      ligado = false;
      digitalWrite(led, LOW);
    }
    if (digitalRead(botao_desliga) == LOW && digitalRead(botao_liga) == HIGH) { //posiciona o reator para ser desativado
      ligado = false;
      // *********** pisca o led
    }
    tempo_leitura_botao = millis();
  }
  if (ligado == true) {  // maquina operando, realiza a varredura de luz
    if (inicio_da_varredura = 1) { //lente se posiciona para o sol nascente
      inicio_da_varredura = 2;
    }
    //  }
    //   else {    // segue o posisicionamento da lente
    if (millis() - tempo_leitura_sensores > 500) { // tempo entre correções de angulo
      v1 = analogRead(ldr_inf_esq) + analogRead(ldr_inf_dir) - analogRead(ldr_sup_esq) - analogRead(ldr_sup_dir);
      v2 = analogRead(ldr_sup_esq) + analogRead(ldr_sup_dir) - analogRead(ldr_inf_esq) - analogRead(ldr_inf_dir);
      v3 = analogRead(ldr_sup_esq) + analogRead(ldr_inf_esq) - analogRead(ldr_sup_dir) - analogRead(ldr_inf_dir);
      v4 = analogRead(ldr_sup_dir) + analogRead(ldr_inf_dir) - analogRead(ldr_sup_esq) - analogRead(ldr_inf_esq);
      if ( v1 > 25) { //inferior > superior
        //Eixo horizontal RELAY LOW FC_HOR2
        if (digitalRead(FC_HOR2) == LOW) {
          digitalWrite(RELAY_HOR, LOW);
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
          acc_motor1 = 190;
        }
      }

      if (v2 > 25) { //superior > inferior
        //Eixo horixontal RELAY HIGH FC_HOR1
        if (digitalRead(FC_HOR1) == LOW) {
          digitalWrite(RELAY_HOR, HIGH);
          delay(500);
          analogWrite(motor2, acc_motor2);
          acc_motor2 = acc_motor2 + 1;
          if ( acc_motor2 > 255) {
            acc_motor1 = 255;
          }
        }
        else {
          analogWrite(motor2, 0);
          acc_motor1 = 190;
        }
        Serial.println("sobe");
      }

      if ( v4 > 25) { //esquerdo > direito
        //Eixo Vertical RELAY HIGH FC_VER1
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
          acc_motor1 = 190;
        }

        Serial.println("esquerda");
      }
      
      if (v3 > 25) { //direito > esquerdo
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
          acc_motor1 = 190;
        }
        Serial.println("direita");
      }
      if (v1 < 50 && v2 < 50 && v3 < 50 && v4 < 25) { // lente perpendicular
        analogWrite(motor1, 0);
        analogWrite(motor2, 0);
        acc_motor1 = 190;
        acc_motor2 = 60;
        Serial.println("desligou motores");
      }
      tempo_leitura_sensores = millis();
      Serial.print( analogRead(ldr_sup_esq)); Serial.print(" -- "); Serial.println(analogRead(ldr_sup_dir));
      Serial.print( analogRead(ldr_inf_esq)); Serial.print(" -- "); Serial.println(analogRead(ldr_inf_dir));
      Serial.println("---------------------------------------------");
    }
  }
  else {
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
    digitalWrite(led, LOW);
  }
}
