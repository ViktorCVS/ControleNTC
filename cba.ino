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

unsigned long i=0;

void setup() {

pinMode(3, OUTPUT);
Serial.begin(9600);

}

void loop() {
 
analogWrite(3, controle_pwm); 

delay(499);

erro[0] = erro[1];

tensao_saida_arduino=analogRead(A4);

tensao_saida = (float)tensao_saida_arduino*5/1023;

corrente_entrada = controle/rin;

Ps = tensao_saida*corrente_entrada;

rs = tensao_saida/corrente_entrada; //Corrente de saída = Corrente de entrada

temperatura_sensor = B/log(rs/A);

if(i<1){
  
  temperatura_filtro_anterior=temperatura_sensor;
 
  }

temperatura_filtro = temperatura_filtro_anterior*alfa+temperatura_sensor*(1-alfa);

temperatura_filtro_anterior=temperatura_filtro;

erro[1]= temperatura_ref - temperatura_filtro;

controle = controle_anterior+a1*erro[1]+a2*erro[0];

if(controle<0.6){
  
  controle = 0.6;
  
  }
  
else if(controle>5){
  
  controle = 5;
  
  }

controle_anterior = controle;

controle_pwm = round((float)controle*255.0/5.0);

i++;

}
