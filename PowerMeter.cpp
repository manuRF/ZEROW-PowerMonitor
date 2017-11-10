/*********************************************************************************
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 *	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	Thanks to andreafabrizi/Dropbox-Uploader for the dropbox script
 *	Thanks to git.drogon.net for the wiringpi
 *
 **********************************************************************************/
/*
 *********************************************************************** 
 */
#include "PowerMeter.h"
#define BASE 100
#define SPI_CHAN 0		//mcp3004Setup (BASE, SPI_CHAN) ; 
#define AN_CHAN_DAC 0
#define BIAS 511		//Resta la componente CC añadida (3.3VDC/2)
#define OFFSET 0.945    // No tendria que ser.
#define VOLTS 220
#define FACTOR  22.75	//22.85	//23 //22.72 //23.25 //20.23

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
char UpLoad[128];	
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
				Irms = corriente();
				Irms = (Irms-OFFSET)/FACTOR;
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
				DB.close();
				strcpy(UpLoad,"dropbox_uploader -f /home/pi/.dropbox_uploader upload ");
				strcat(UpLoad,FileNameDB);
				strcat(UpLoad," \"PowerMonitoring/\"");
				system(UpLoad);	
				LOG<<logTime()<<"Realizado : "<<UpLoad<<endl;
				logger.close();
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
float corriente(void)
{
  float voltajeSensor;
  float corriente=0;
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
