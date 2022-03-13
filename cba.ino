#include <math.h>

float tensao_inicial=1.5;
int tensao_inicial_pwm = round(tensao_inicial*255/5);

float tensao_saida_arduino;
float tensao_saida;
float corrente_entrada;

float controle_anterior=tensao_inicial;
float controle=tensao_inicial;
int controle_pwm = tensao_inicial_pwm;

float rs;
float Ps;
float Ta;

int rin=1643;
int B=3950;
float A=0.005289681;
float Gth = 0.002;
float Cth = 0.024;

//k=0.5;T=0.5
float a1=5.839;
float a2=-5.589;

float alfa=0.95;

float erro[2]={0,0};

float temperatura_ref_C = 35.5;
float temperatura_ref = temperatura_ref_C+273.15;

float temperatura_sensor;
float temperatura_filtro_anterior;

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
