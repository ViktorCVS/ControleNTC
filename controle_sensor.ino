//---------- Definição de módulos ----------

#include <math.h> // Como foi necessário usar a função logaritmo na base de euler, incluímos a biblioteca 'math.h' no projeto

//---------- Entradas ----------

float corrente_inicial=(float)1.5*1000/rin;   // Definindo a corrente (em mili ampère) aplicada no circuito no tempo zero. Usou-se a corrente que corresponde a 1.5V em rin
float temperatura_ref_C = 35.5;               // Definindo a temperatura de referência do sensor, em graus Celsius
float alfa=0.92;                              // Definindo a constante do filtro na variável controlada
float a1=0.5243;                              // Definindo o primeiro parâmetro do controlador
float a2=-0.5043;                             // Definindo o segundo parâmetro do controlador

//---------- Variáveis ----------

int tensao_inicial_pwm = round(1.5*255/5); // Passando a tensão inicial desejada (1.5V) para o valor aceito pelo Arduino UNO (inteiro entre 0 e 255)

int tensao_saida_arduino;   // Definindo a variável que receberá a medida da tensão nos terminais do sensor (inteiro entre 0 e 1023)
float tensao_saida;         // Definindo a variável que receberá a tensão do sensor em volts
float corrente_entrada;     // Definindo a variável que receberá a corrente que passa na entrada e no sensor

float controle=corrente_inicial;           // Definindo a variável de controle, que receberá o valor em mili ampère a ser aplicado no sensor no próximo instante de tempo
float controle_anterior=corrente_inicial;  // Definindo a variável que receberá o valor antigo da variável 'controle'
int controle_pwm = tensao_inicial_pwm;     // Definindo a variável que receberá a variável de controle para o valor aceito pelo Arduino UNO (inteiro entre 0 e 255)

float rs; // Definindo a variável que receberá o valor da resistência do sensor

int rin=1644;         // Definindo a variável que contém o valor da resistência de entrada
int B=3950;           // Definindo a variável que contém o valor da constante B do sensor NTC
float A=0.005289681;  // Definindo a variável que contém o valor da constante A do sensor NTC

float erro[2]={0,0};  // Definindo o vetor que acumulará o erro atual e passado da temperatura do sensor com a referência

float temperatura_ref = temperatura_ref_C+273.15; // Definindo a variável que receberá a temperatura de referência em Kelvin

float temperatura_sensor;           // Definindo a variável que receberá a temperatura do sensor
float temperatura_filtro;           // Definindo a variável que receberá a temperatura filtrada do sensor no instante atual
float temperatura_filtro_anterior;  // Definindo a variável que receberá a temperatura filtrada do sensor no instante anterior

unsigned long i=0;  // Definindo a variável que guarda o número de vezes que o laço de repetição foi executado

//---------- Iniciando a programação ----------

void setup() {
 
 //---------- Definindo entrada, saída e porta serial ----------

pinMode(A5, INPUT); // Linha de código para definir o pino A5 como entrada
pinMode(3, OUTPUT); // Linha de código para definir o pino 3 como saída
 
Serial.begin(9600); // Linha de código para iniciar a porta serial com a velocidade 9600

}

void loop() {
 
 //---------- Início do laço de repetição ----------
 
analogWrite(3, controle_pwm); // Enviando o sinal PWM através do pino 3

delay(497);                  // Usando uma função nativa de atraso, para manter o tempo de amostragem estipulado (o tempo de execução do código ficou entorno de 3 mili segundos)

erro[0] = erro[1];           // Atribuindo ao erro passado o valor do erro atual

tensao_saida_arduino=analogRead(A5);               // Medindo a tensão nos terminais do sensor

tensao_saida = (float)tensao_saida_arduino*5/1023; // Passando do formato inteiro 0-1023 para volts

corrente_entrada = (float)controle/1000;    // Calculando a corrente de entrada com a variável de controle (passando de mA para A)

rs = (float)tensao_saida/corrente_entrada; // Calculando a resistência do sensor, dividindo a tensão medida pela corrente de entrada (que é igual a de saída)

temperatura_sensor = (float)B/log(rs/A);   // Encontrando a temperatura do sensor pela equação estática, usando as constantes e a resistência deste
 
if(i<1){
   
    /* Condicional para, na primeira iteração, atribuir à temperatura anterior 
    o valor da temperatura atual do sensor */
   
    temperatura_filtro_anterior=temperatura_sensor; // Fazendo a atribuição
 
    }

temperatura_filtro = temperatura_filtro_anterior*alfa+temperatura_sensor*(1-alfa); // Aplicando o filtro projetado para a temperatura do sensor

temperatura_filtro_anterior=temperatura_filtro; // Passando o valor atual da temperatura filtrada para a variável do valor anterior deste

erro[1]= temperatura_ref - temperatura_filtro;  // Calculando o erro atual pela diferença entre a temperatura de referência e a medida no sensor (filtrada)

controle = controle_anterior+a1*erro[1]+a2*erro[0]; // Cálculo da saída do controlador

controle = (float)(controle/1000)*rin; //Passando o controle (em mA) para Volts, visto que as limitações do arduino são melhor controladas nessa unidade
 
if(controle<0.6){
 
    /* Condicional para impedir que a tensão aplicada
    no Arduino seja inferior à 0.6V */
  
    controle = 0.6;  // Se o valor de controle for inferior a 0.6V, atribuir a este 0.6V
  
    }
  
else if(controle>5){
 
    /* Condicional para impedir que a tensão aplicada
    no Arduino seja superior à 5V (limite do Arduino UNO) */
  
    controle = 5;    // Se o valor de controle for superior a 5V, atribuir a este 5V
  
    }

controle_pwm = round((float)controle*255.0/5.0); // Passando o valor atual do controle (após condicionais) para um inteiro entre 0 e 255 (para o sinal PWM)
 
controle = (float)controle*1000/rin; // Voltando a unidade da variável de controle para mA

controle_anterior = controle;        // Passando o valor atual do controle (após condicionais) para a variável que acumula o valor anterior de controle

Serial.print(temperatura_filtro-273.15);   // Enviando para a porta serial o valor da temperatura atual do sensor em graus Celcius
Serial.print(",");                         // Enviando para a porta serial um separador (opção visual para visualizar e capturar os dados posteriormente)
Serial.print(temperatura_ref_C);           // Enviando para a porta serial o valor da temperatura de referência, em graus Celcius
Serial.print(",");                         // Enviando para a porta serial um separador (opção visual para visualizar e capturar os dados posteriormente)
Serial.println(corrente_entrada*1000);     // Enviando para a porta serial o valor da corrente de entrada no instante atual

i++; // Incrementando o valor da variável que contém o número de vezes que o laço de repetição foi executado

}
