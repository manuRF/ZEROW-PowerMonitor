/*
 ***********************************************************************
 * 
 */
 #ifndef _PowerMeter_h
 #define _PowerMeter_h 
 /*
 *********************************************************************** 
 */
 #include <stdio.h>
 #include <wiringPi.h>
 #include <mcp3004.h>
 #include <sys/time.h>
 #include <stdlib.h>
 #include <time.h>
 #include <string.h>
 #include <iostream>
 #include <sstream>  
 #include <fstream>
 #include <math.h>
 #include <unistd.h>
  
 #define LOG logger //cout


using namespace std;


 /*
 *********************************************************************** 
 */
float current();
string logTime();
 /*
 *********************************************************************** 
 */
#endif
