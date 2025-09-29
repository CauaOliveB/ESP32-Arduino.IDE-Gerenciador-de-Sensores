// Olá, bem-vindo ao sensor de umidade

const int pinHumiditySensor = 34;
const int ledG = 27;
const int ledY = 26;
const int ledR = 25;

void setup() {  
  Serial.begin(115200);
  pinMode(pinHumiditySensor, INPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledY, OUTPUT);
  pinMode(ledR, OUTPUT);
}

void loop() {
  int valorUmidade = analogRead(pinHumiditySensor);
  Serial.println(valorUmidade);

  // Apaga todos os LEDs antes de ligar o correto
  digitalWrite(ledG, LOW);
  digitalWrite(ledY, LOW);
  digitalWrite(ledR, LOW);

  // Defina faixas para os níveis de umidade
  if (valorUmidade >= 4000) {
    // Muito seco
    digitalWrite(ledR, HIGH);
  }
  else if (valorUmidade >= 2100) {
    // Umidade média
    digitalWrite(ledY, HIGH);
  }
  else {
    // Solo úmido
    digitalWrite(ledG, HIGH);
  }

  delay(500);  // Pequeno delay para estabilidade
}
