//---------- Definição de módulos ----------

#include <math.h> // Como foi necessário usar a função logaritmo na base de euler, incluímos a biblioteca 'math.h' no projeto

//---------- Entradas ----------

float tensao_inicial=1.5;         // Definindo a tensão aplicada no circuito no tempo zero
float temperatura_ref_C = 35.5;   // Definindo a temperatura de referência do sensor, em graus Celsius
float alfa=0.95;                  // Definindo a constante do filtro na variável controlada
float a1=5.839;                   // Definindo o primeiro parâmetro do controlador
float a2=-5.589;                  // Definindo o segundo parâmetro do controlador

//---------- Variáveis ----------

int tensao_inicial_pwm = round(tensao_inicial*255/5); // Passando a tensão inicial desejada para o valor aceito pelo Arduino UNO (inteiro entre 0 e 255)

int tensao_saida_arduino;   // Definindo a variável que receberá a medida da tensão nos terminais do sensor (inteiro entre 0 e 1023)
float tensao_saida;         // Definindo a variável que receberá a tensão do sensor em volts
float corrente_entrada;     // Definindo a variável que receberá a corrente que passa na entrada e no sensor

float controle=tensao_inicial;            // Definindo a variável de controle, que receberá o valor em Volts a ser aplicado no sensor no próximo instante de tempo
float controle_anterior=tensao_inicial;   // Definindo a variável que receberá o valor antigo da variável 'controle'
int controle_pwm = tensao_inicial_pwm;    // Definindo a variável que receberá a variável de controle para o valor aceito pelo Arduino UNO (inteiro entre 0 e 255)

float rs; // Definindo a variável que receberá o valor da resistência do sensor

int rin=1643;         // Definindo a variável que contém o valor da resistência de entrada
int B=3950;           // Definindo a variável que contém o valor da constante B do sensor NTC
float A=0.005289681;  // Definindo a variável que contém o valor da constante A do sensor NTC

float erro[2]={0,0};  // Definindo o vetor que acumulará o erro atual e passado da temperatura do sensor com a referência


float temperatura_ref = temperatura_ref_C+273.15; // Definindo a variável que receberá a temperatura de referência em Kelvin

float temperatura_sensor;           // Definindo a variável que receberá a temperatura do sensor
float temperatura_filtro_anterior;  // Definindo a variável que receberá a temperatura filtrada do sensor no instante anterior

//---------- Iniciando a programação ----------

void setup() {
 
 //---------- Definindo entrada, saída e porta serial ----------

pinMode(3, OUTPUT); // Linha de código para definir o pino 3 como saída
Serial.begin(9600); // Linha de código para iniciar a porta serial com a velocidade 9600
 
 //---------- Bloco especial ----------
 
 /* O bloco seguinte foi criado, parecido com as primeiras linhas
 do código em repetição, para a definição prévia da variável
 "temperatura_filtro_anterior", pois ela precisa ser igual
 a temperatura do sensor no primeiro ciclo, que só pode ser
 definida após essa série de operações. Para economia de memória,
 não foram usadas funções. As operações serão detalhadas no
 laço de repetição. */
 
analogWrite(3, controle_pwm); 

delay(499);

tensao_saida_arduino=analogRead(A4);

tensao_saida = (float)tensao_saida_arduino*5/1023;

corrente_entrada = controle/rin;

rs = tensao_saida/corrente_entrada; 

temperatura_sensor = B/log(rs/A);
 
temperatura_filtro_anterior=temperatura_sensor;

}

void loop() {
 
 //---------- Início do laço de repetição ----------
 
analogWrite(3, controle_pwm); // Enviando o sinal PWM através do pino 3, com a tensão inicial escolhida previamente

delay(499);                  // Usando uma função nativa de atraso, para manter o tempo de amostragem estipulado

erro[0] = erro[1];           // Atribuindo ao erro passado o valor do erro atual

tensao_saida_arduino=analogRead(A4);               // Medindo a tensão nos terminais do sensor

tensao_saida = (float)tensao_saida_arduino*5/1023; // Passando do formato inteiro 0-1023 para volts

corrente_entrada = controle/rin;    // Calculando a corrente de entrada pela divisão entre a tensão utilizada e a resistência de entrada 

rs = tensao_saida/corrente_entrada; // Calculando a resistência do sensor, dividindo a tensão medida pela corrente de entrada (que é igual a de saída)

temperatura_sensor = B/log(rs/A);   // Encontrando a temperatura do sensor pela equação estática, usando as constantes e a resistência deste

temperatura_filtro = temperatura_filtro_anterior*alfa+temperatura_sensor*(1-alfa); // Aplicando o filtro projetado para a temperatura do sensor

temperatura_filtro_anterior=temperatura_filtro; // Passando o valor atual da temperatura filtrada para a variável do valor anterior deste

erro[1]= temperatura_ref - temperatura_filtro;  // Calculando o erro pela diferença entre a temperatura de referência e a medida (filtrada)

controle = controle_anterior+a1*erro[1]+a2*erro[0]; // Cálculo da saída do controlador

if(controle<0.6){
 
 /* Condicional para impedir que a tensão do
 controlador seja inferior à 0.6V */
  
  controle = 0.6;
  
  }
  
else if(controle>5){
 
 /* Condicional para impedir que a tensão do
 controlador seja superior à 5V (limite do Arduino UNO */
  
  controle = 5;
  
  }

controle_anterior = controle;                    // Passando o valor atual do controle (após condicionais) para a variável do valor anterior deste

controle_pwm = round((float)controle*255.0/5.0); // Passando o valor atual do controle (após condicionais) para um inteiro entre 0 e 255 (para o sinal PWM)

}
