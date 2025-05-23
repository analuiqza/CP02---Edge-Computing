# CP02---Edge-Computing
Ana Luiza Rinaldi - rm564061
Giovanna Sayama - rm565901
Rayssa Aquino - rm562024
Giovana Larocca - rm564965


## Sistema de Monitoramento da Qualidade do Ambiente para Vinhos

O projeto CP02 tem como objetivo garantir que as condições de armazenamento do vinho estejam sempre nos padrões ideais de temperatura, umidade e luminosidade, fatores fundamentais para preservar a qualidade dos vinhos. Utilizamos um sistema embarcado com Arduino e sensores para realizar a leitura dos dados ambientais, exibi-los em tempo real e emitir alertas quando houver risco para o vinho.


## Objetivos do Projeto:

1. Monitorar em tempo real os valores da:
   - Temperatura
   - Umidade
   - Luminosidade

2. Exibir os dados em um display LCD(Liquid Crystal Display).

3. Alertar com LEDs e Buzzer quando os valores estiverem fora do normal.

4. Registrar os dados com data e hora usando um módulo RTC(Real Time Clock) e armazená-los em uma EEPROM(Electrically Erasable Programmable Read-Only Memory).

5. Calcular a média de luminosidade a cada 10 segundos usando a função `map()` para calibrar os valores.


## Componentes utilizados:

### Hardware:
- Arduino Uno.
- Sensor DHT11 (Digital Humidity and Temperature sensor 11).
- Sensor LDR (Light Dependent Resistor).
- Display LCD (Liquid Crystal Display) 16x2 com interface I2C.
- Módulo RTC (Real Time Clock).
- EEPROM interno ou externo <!MUDAR! (Electrically Erasable Programmable Read-Only Memory).
- LEDs (vermelho, amarelo, verde).
- Buzzer (alarme sonoro).
- Jumpers, resistores e protoboard.

### Bibliotecas Arduino:
- `DHT.h`- leitura do sensor DHT11.
- `LiquidCrystal_I2C.h`- controle do display LCD.
- `RTClib.h`- controle de tempo real.
- `EEPROM.h`- armazenamento de dados.


## Como montar e executar o projeto:

1.Separe os componentes
   - Microcontrolador, sensores (DHT11 e LDR), Display LCD, RTC, EEPROM, LEDs, Buzzer, protoboard, jumpers e resistores.
     
     ![imagem componentes](https://github.com/user-attachments/assets/a2284b4b-4e92-49cd-be85-7868dbaa60c3)

2. Monte o circuito:
   - Monte o circuito com o sensor (DHT11 de maneira que possa medir a temperatura e umidade e LDR de forma que possa medir a luminosidade), o display, os LEDs, buzzer, módulo RTC e a EEPROM.
     
     ![imagem circuito](https://github.com/user-attachments/assets/9f5362f3-f790-48ff-b176-37d1f0c6a1d8)

3. Configure a IDE do Arduino:
   - Instale as bibliotecas necessárias.
   - Selecione a a placa e a porta corretas.

4. Carregue o código:
   - Faça upload do programa que lê os sensores, mostra os dados no LCD e ativa os alertas.
   - O código também salvará leituras no RTC e EEPROM.

5. Acompanhe o funcionamento:
   - Veja os dados no LCD.
   - Se a temperatura ultrapassar 16°C ou a umidade sair da faixa 60–80% <!MUDAR! , um LED ou o buzzer será ativado.
   - A luminosidade será lida continuamente e suavizada com médias calculadas a cada 10 segundos.


##Simulação no Wokwi:

1. Monte o circuito virtual com todos os componentes do projeto no site Wokwi.

2. Cole o código no simulador.

3. Verifique se tudo funciona como esperado, mesmo sem os componentes físicos.
	-(foto da simulação)


## Faixas de armazenamento:

    Fator              Valor ideal                          
 Temperatura   -  ~13°C (não passar de 16°C)            
 Umidade       -  ~70% (entre 60% e 80%)                
 Luminosidade  -  Baixa, ambiente escuro ou com penumbra
		 ^!MUDAR!^ 
  	-(foto dos valores colocados)


## Função `map()`:

O sensor LDR retorna um valor analógico que pode variar de 0 a 1023. A função `map()` é usada para transformar esse valor em uma escala útil, e a cada 10 segundos calcular a média para suavizar oscilações rápidas da luz ambiente.
	-(foto função map no código)
