 /*
 *********************************************************************** 
 */
#include "PowerMeter.h"
#define BASE 100
#define SPI_CHAN 0		//mcp3004Setup (BASE, SPI_CHAN) ; 
#define AN_CHAN_DAC 0
#define BIAS 512		//Substract CC component (3.3VDC/2)
#define VOLTS 220
#define FACTOR  22.75	//Convesion 512 digital values to current

using namespace std;

ofstream logger;
ofstream DB;
 /*
 ******************************************************************************************************** 
 */
int main(void){
int value;
float Irms;
float Potencia;	
char FileName[128];
char FileNameDB[128];	
timeval currtime;	
wiringPiSetup();
mcp3004Setup (BASE, SPI_CHAN) ; 
char TimeString[3];
timeval currTime;
bool Mes = 1;
bool Dia = 1;
bool Hora =1;
bool Minuto = 1;
int D = 0;
int H = 0;
int M = 0;
float PotenciaMediaMinuto = 0;
float PotenciaMediaHora  = 0;
float PotenciaMediaDia = 0;
while(Mes){	
	gettimeofday(&currtime, NULL);
	strftime(FileName, 128, "/home/pi/PowerLogs/%d.%m.%y.%H.%M.log",localtime(&currtime.tv_sec));	
	strftime(FileNameDB,128,"/home/pi/PowerLogs/%d.%m.%y.csv",localtime(&currtime.tv_sec));
	logger.open(FileName);
	DB.open(FileNameDB);
	while(Dia){
		while(Hora){
			while(Minuto){
				Irms = current()/FACTOR;
				Potencia = Irms*VOLTS;
				PotenciaMediaMinuto = Potencia+PotenciaMediaMinuto;
				M++;
				gettimeofday(&currTime,NULL);
				strftime(TimeString,3,"%S",localtime(&currTime.tv_sec));
				if(strcmp(TimeString,"00")==0){
					Minuto =0;
					}
				}
			PotenciaMediaMinuto = PotenciaMediaMinuto/M;
			LOG<<logTime()<<"PotenciaMediaMinuto : "<<PotenciaMediaMinuto<<endl;
			PotenciaMediaHora = PotenciaMediaHora + PotenciaMediaMinuto;	
			H++;
			PotenciaMediaMinuto = 0; M= 0;
			gettimeofday(&currTime,NULL);
			strftime(TimeString,3,"%M",localtime(&currTime.tv_sec));
			LOG<<logTime()<<"Lazo de horas : "<<TimeString<<" minuto"<<endl;
			if(strcmp(TimeString,"00")==0){
				Hora = 0;
				}				
			Minuto = 1;
			}
		PotenciaMediaHora = PotenciaMediaHora/H;
		LOG<<logTime()<<"PotenciaMediaHora : "<<PotenciaMediaHora<<endl;	
		gettimeofday(&currTime,NULL);
		strftime(TimeString,3,"%H",localtime(&currTime.tv_sec));		
		DB<<TimeString<<";"<<PotenciaMediaHora<<endl;
		PotenciaMediaDia = PotenciaMediaDia + PotenciaMediaHora;	
		PotenciaMediaHora = 0; H= 0;
		gettimeofday(&currTime,NULL);
		strftime(TimeString,3,"%H",localtime(&currTime.tv_sec));
		LOG<<logTime()<<"Lazo de dias : "<<TimeString<<" hora"<<endl;
		if(strcmp(TimeString,"00")==0){
				Dia = 0;
				logger.close();
				DB.close();
				// ToDo Send FileNameDB & FileName to dropBox.
				// ToDo Delete FileNameDB & FileName.
			}
		Hora = 1;		
		}
	Dia=1;
	}	
logger.close();
DB.close();
return 0;
}
/*
***************************************************************************
*/
float current(void)
{
  float voltajeSensor;
  float current=0;
  float Sumatoria=0;
  long  tiempo=millis();
  long  start_aqu, end_aqu;
  int N=0;
  timeval curTime;
  gettimeofday(&curTime, NULL);
 while(millis()-tiempo < 1000){ 
	voltajeSensor = analogRead (BASE + AN_CHAN_DAC) - BIAS;
	Sumatoria=Sumatoria+pow(voltajeSensor,2);
    N++;
  }
return(sqrt((Sumatoria)/N));
}
/*
* *******************************************************************************************************
*/
string logTime(void){
	char TimeString[128];
	timeval curTime;
	gettimeofday(&curTime, NULL);
	strftime(TimeString, 80, "%d-%m-%Y %H:%M:%S-> ", localtime(&curTime.tv_sec));
	return(TimeString);
}
/*
* *******************************************************************************************************
*/
