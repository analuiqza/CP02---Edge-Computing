#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <RTClib.h>
#include <EEPROM.h>
// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Pinos
const int ldrPin = A0;
const int buzzerPin = 8;
const int ledVerde = 3;
const int ledAmarelo = 4;
const int ledVermelho = 5;
const int botaoPin = 2;
const int dhtPin = 9;
const int botaoSimPin = 10;       // Botão para incrementar
const int botaoNaoPin = 11;       // Botão para pular / confirmar no ajuste antigo
const int botaoNaoConfirmarPin = 12;  // Novo botão para decrementar e confirmar
#define DHTTYPE DHT11
DHT dht(dhtPin, DHTTYPE);
RTC_DS3231 rtc;
bool sistemaLigado = false;
bool animacaoFeita = false;
bool estadoBotaoAnterior = HIGH;
int ldrMin = 1023;
int eepromAddress = 0;
byte U1[8] = {0b00000,0b00000,0b00000,0b01000,0b01000,0b01000,0b01000,0b01000};
byte U2[8] = {0b00000,0b00000,0b00000,0b00010,0b00010,0b00010,0b00010,0b00010};
byte U3[8] = {0b01000,0b01000,0b01000,0b01000,0b01000,0b00111,0b00000,0b00000};
byte U4[8] = {0b00010,0b00010,0b00010,0b00010,0b00010,0b11100,0b00000,0b00000};
byte X1[8] = {0b00000,0b00000,0b00000,0b10000,0b01000,0b00100,0b00010,0b00001};
byte X2[8] = {0b00000,0b00000,0b00000,0b00001,0b00010,0b00100,0b01000,0b10000};
byte X3[8] = {0b00001,0b00010,0b00100,0b01000,0b10000,0b00000,0b00000,0b00000};
byte X4[8] = {0b10000,0b01000,0b00100,0b00010,0b00001,0b00000,0b00000,0b00000};
// Funções para salvar e ler float da EEPROM
void EEPROM_writeFloat(int address, float value) {
 byte *p = (byte*)(void*)&value;
 for (int i = 0; i < 4; i++) {
   EEPROM.write(address + i, *(p + i));
 }
}
float EEPROM_readFloat(int address) {
 float value = 0.0;
 byte *p = (byte*)(void*)&value;
 for (int i = 0; i < 4; i++) {
   *(p + i) = EEPROM.read(address + i);
 }
 return value;
}
void mostrarBoasVindas() {
 lcd.clear();
 lcd.setCursor(0, 0); lcd.write(byte(0));
 lcd.setCursor(1, 0); lcd.write(byte(1));
 lcd.setCursor(0, 1); lcd.write(byte(2));
 lcd.setCursor(1, 1); lcd.write(byte(3));
 tone(buzzerPin, 600, 120);
 delay(400);
 lcd.setCursor(2, 0); lcd.write(byte(4));
 lcd.setCursor(3, 0); lcd.write(byte(5));
 lcd.setCursor(2, 1); lcd.write(byte(6));
 lcd.setCursor(3, 1); lcd.write(byte(7));
 tone(buzzerPin, 800, 120);
 delay(400);
}
void animacaoCellence() {
 lcd.setCursor(5, 1);
 const char* texto = "CELLENCE";
 int frequenciaBase = 1500;
 for (int i = 0; texto[i] != '\0'; i++) {
   lcd.print(texto[i]);
   tone(buzzerPin, frequenciaBase + i * 100, 100);
   delay(150);
 }
 delay(600);
 lcd.print(" *");
 delay(300);
 lcd.setCursor(0, 1);
 for (int i = 0; i < 16; i++) lcd.print(" ");
}
void mostrarBemVindo() {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Bem Vindo(a)!");
  lcd.setCursor(0, 1);
 lcd.print("     :)    ");
}
bool botaoPressionado(int pin) {
 return digitalRead(pin) == LOW;
}
void esperarBotaoSolto(int pin) {
 while (digitalRead(pin) == LOW);
 delay(50);
}
// Função geral para ajuste de um valor com incremento e decremento
int ajustarValor(const char* nome, int valor, int minVal, int maxVal) {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print(nome);
 lcd.print(": ");
 lcd.print(valor);
 while (true) {
   if (botaoPressionado(botaoSimPin)) {  // Incrementa
     valor++;
     if (valor > maxVal) valor = minVal;
     lcd.setCursor(strlen(nome) + 2, 0);
     lcd.print("   "); // Limpa
     lcd.setCursor(strlen(nome) + 2, 0);
     lcd.print(valor);
     esperarBotaoSolto(botaoSimPin);
   }
   if (botaoPressionado(botaoNaoConfirmarPin)) { // Decrementa
     valor--;
     if (valor < minVal) valor = maxVal;
     lcd.setCursor(strlen(nome) + 2, 0);
     lcd.print("   ");
     lcd.setCursor(strlen(nome) + 2, 0);
     lcd.print(valor);
     esperarBotaoSolto(botaoNaoConfirmarPin);
   }
   if (botaoPressionado(botaoNaoPin)) { // Confirma e sai
     esperarBotaoSolto(botaoNaoPin);
     delay(200);
     break;
   }
 }
 return valor;
}
void ajustarDataHora() {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("AjustarData/Hora?");
 lcd.setCursor(0, 1);
 lcd.print("Sim     Nao");
 while (true) {
   if (botaoPressionado(botaoSimPin)) {
     delay(300);
     DateTime now = rtc.now();
     int ano = now.year();
     int mes = now.month();
     int dia = now.day();
     int hora = now.hour();
     int minuto = now.minute();
     int segundo = now.second();
     ano = ajustarValor("Ano", ano, 2000, 2099);
     mes = ajustarValor("Mes", mes, 1, 12);
     dia = ajustarValor("Dia", dia, 1, 31);
     hora = ajustarValor("Hora", hora, 0, 23);
     minuto = ajustarValor("Min", minuto, 0, 59);
     segundo = ajustarValor("Seg", segundo, 0, 59);
     rtc.adjust(DateTime(ano, mes, dia, hora, minuto, segundo));
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Data/Hora ajustada");
     delay(1500);
     lcd.clear();
     break;
   }
   if (botaoPressionado(botaoNaoPin)) {
     delay(300);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Pulando ajuste...");
     delay(1500);
     lcd.clear();
     break;
   }
 }
}
void calibrarLDR() {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Luz Maxima");
 lcd.setCursor(0, 1);
 lcd.print("Sim:Confirmar");
 while (!botaoPressionado(botaoSimPin));
 delay(300);
 ldrMin = analogRead(ldrPin);
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Luz Minima");
 lcd.setCursor(0, 1);
 lcd.print("Sim:Confirmar");
 while (!botaoPressionado(botaoSimPin));
 delay(300);
 ldrMax = analogRead(ldrPin);
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Calibrado!");
 delay(1000);
 lcd.clear();
}
void perguntarCalibracaoLDR() {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Calibrar Luz?");
 lcd.setCursor(0, 1);
 lcd.print("Sim    Nao");
 while (true) {
   if (botaoPressionado(botaoSimPin)) {
     delay(300);
     calibrarLDR();
     break;
   }
   if (botaoPressionado(botaoNaoPin)) {
     delay(300);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Não calibrado");
     delay(1000);
     lcd.clear();
     break;
   }
 }
}
void salvarDadoInesperado(float temp, float hum, int luz) {
 // Salva dados sequencialmente na EEPROM
 // Estrutura: temp(float), hum(float), luz(int como float)
 EEPROM_writeFloat(eepromAddress, temp);
 eepromAddress += 4;
 EEPROM_writeFloat(eepromAddress, hum);
 eepromAddress += 4;
 EEPROM_writeFloat(eepromAddress, (float)luz);
 eepromAddress += 4;
 // Se ultrapassar o limite da EEPROM, reinicia o endereço (para EEPROM de 1024 bytes)
 if (eepromAddress > 1020) eepromAddress = 0;
}
void imprimirDadosEEPROM() {
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Dados guardados:");
 Serial.println("\nDados inesperados armazenados:");
 int addr = 0;
 while (addr < eepromAddress) {
   float temp = EEPROM_readFloat(addr);
   addr += 4;
   float hum = EEPROM_readFloat(addr);
   addr += 4;
   float luz = EEPROM_readFloat(addr);
   addr += 4;
   Serial.print("Temperatura: ");
   Serial.print(temp);
   Serial.print(" C, Umidade: ");
   Serial.print(hum);
   Serial.print(" %, Luz: ");
   Serial.print(luz);
   Serial.println(" %");
   delay(500);
 }
 delay(3000);
 lcd.clear();
}
void setup() {
 Serial.begin(9600);
 lcd.init();
 lcd.backlight();
 pinMode(buzzerPin, OUTPUT);
 pinMode(ledVerde, OUTPUT);
 pinMode(ledAmarelo, OUTPUT);
 pinMode(ledVermelho, OUTPUT);
 pinMode(botaoPin, INPUT_PULLUP);
 pinMode(botaoSimPin, INPUT_PULLUP);
 pinMode(botaoNaoPin, INPUT_PULLUP);
 pinMode(botaoNaoConfirmarPin, INPUT_PULLUP); // Novo botão
 Serial.begin(9600);
 dht.begin();
 if (!rtc.begin()) {
   Serial.println("RTC nao encontrado!");
   while (1);
 }
 if (rtc.lostPower()) {
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 }
 lcd.clear();
 lcd.noBacklight();
 lcd.setCursor(0, 0);
 lcd.print("Sistema Desligado");
}
void loop() {
 bool estadoBotao = digitalRead(botaoPin);
 if (estadoBotao == LOW && estadoBotaoAnterior == HIGH) {
   sistemaLigado = !sistemaLigado;
   animacaoFeita = false;
   if (!sistemaLigado) {
     lcd.clear();
     lcd.noBacklight();
     lcd.setCursor(0, 0);
     lcd.print("Sistema Desligado");
     digitalWrite(ledVerde, LOW);
     digitalWrite(ledAmarelo, LOW);
     digitalWrite(ledVermelho, LOW);
     noTone(buzzerPin);
   } else {
     lcd.backlight();
     lcd.clear();
     mostrarBoasVindas();
     animacaoCellence();
     mostrarBemVindo();
     ajustarDataHora();
     perguntarCalibracaoLDR();
   }
   delay(300);
 }
 estadoBotaoAnterior = estadoBotao;
 if (sistemaLigado) {
   DateTime now = rtc.now();
   int ldrValor = analogRead(ldrPin);
   int porcentagemLuz = map(ldrValor, ldrMax, ldrMin, 0, 100);
   porcentagemLuz = constrain(porcentagemLuz, 0, 100);
   float humidade = dht.readHumidity();
   float temperatura = dht.readTemperature();
   static unsigned long previousMillis = 0;
   const unsigned long interval = 2000;
   static int estado = 0;
   unsigned long currentMillis = millis();
   if (currentMillis - previousMillis >= interval) {
     previousMillis = currentMillis;
     lcd.clear();
     if (estado == 0) {
       // Tela de umidade
       lcd.setCursor(0, 0);
       lcd.print("Umidade");
       lcd.setCursor(0, 1);
       if (isnan(humidade)) {
         lcd.print("Erro leitura");
       } else {
         lcd.print(humidade, 1);
         lcd.print(" %");
         if (humidade < 40 || humidade > 70) {
           lcd.setCursor(13, 1);
           lcd.print(":("); // carinha triste
         }
       }
     }
     else if (estado == 1) {
       // Tela de temperatura
       lcd.setCursor(0, 0);
       lcd.print("Temperatura");
       lcd.setCursor(0, 1);
       if (isnan(temperatura)) {
         lcd.print("Erro leitura");
       } else {
         lcd.print(temperatura, 1);
         lcd.print(" C");
         if (temperatura < 20 || temperatura > 30) {
           lcd.setCursor(13, 1);
           lcd.print(":(");
         }
       }
     }
     else if (estado == 2) {
       // Tela de luminosidade
       lcd.setCursor(0, 0);
       lcd.print("Luminosidade");
       lcd.setCursor(0, 1);
       lcd.print(porcentagemLuz);
       lcd.print(" %");
       if (porcentagemLuz > 75) {
         lcd.setCursor(13, 1);
         lcd.print(":(");
       }
     }
     estado++;
     if (estado > 2) estado = 0;
   }
   // Controle de leds e buzzer conforme luminosidade
   digitalWrite(ledVerde, LOW);
   digitalWrite(ledAmarelo, LOW);
   digitalWrite(ledVermelho, LOW);
   noTone(buzzerPin);
   // LED verde para umidade alta (> 75%)
   if (humidade >= 75) {
     digitalWrite(ledVerde, HIGH);
   } else {
     digitalWrite(ledVerde, LOW);
   }
   // LED amarelo para temperatura alta (> 25C)
   if (temperatura >= 25) {
     digitalWrite(ledAmarelo, HIGH);
   } else {
     digitalWrite(ledAmarelo, LOW);
   }
   // LED vermelho para alta luminosidade (> 75%)
   if (porcentagemLuz > 75) {
     digitalWrite(ledVermelho, HIGH);
   } else {
     digitalWrite(ledVermelho, LOW);
   }
   bool dadoInesperado = false;
   if (!isnan(temperatura) && (temperatura < 15 || temperatura > 35)) {
     dadoInesperado = true;
   }
   if (!isnan(humidade) && (humidade < 30 || humidade > 80)) {
     dadoInesperado = true;
   }
   if (porcentagemLuz < 10 || porcentagemLuz > 90) {
     dadoInesperado = true;
   }
   if (dadoInesperado) {
     salvarDadoInesperado(temperatura, humidade, porcentagemLuz);
     digitalWrite(ledVermelho, HIGH);
     digitalWrite(ledAmarelo, LOW);
     digitalWrite(ledVerde, LOW);
     tone(buzzerPin, 2000, 200);
   } else {
     digitalWrite(ledVermelho, LOW);
     if (porcentagemLuz < 40) {
       digitalWrite(ledVerde, LOW);
       digitalWrite(ledAmarelo, LOW);
       digitalWrite(ledVermelho, HIGH);
     } else if (porcentagemLuz < 70) {
       digitalWrite(ledVerde, LOW);
       digitalWrite(ledAmarelo, HIGH);
       digitalWrite(ledVermelho, LOW);
     } else {
       digitalWrite(ledVerde, HIGH);
       digitalWrite(ledAmarelo, LOW);
       digitalWrite(ledVermelho, LOW);
     }
     noTone(buzzerPin);
   }
   delay(1000); // Atualiza a cada 1 segundo
   // Desligar sistema se botão pressionado novamente
   if (digitalRead(botaoPin) == LOW) {
     sistemaLigado = false;
     lcd.clear();
     lcd.print("Desligando...");
     delay(1000);
     imprimirDadosEEPROM();
     // Reseta endereço para novos dados
     eepromAddress = 0;
     lcd.clear();
     lcd.print("Sistema Desligado");
     delay(2000);
     lcd.clear();
   }
 }
}
