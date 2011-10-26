#include "suryabrew.h"


double natural_log(const double in)
{
	/*
		Compute the natural log using newtons method!
		x_(n+1) = x_n - (f(x_n)/f'(x_n))
		where f(x) == e^x - in
	*/
	
	double exp_1 = FASSIGN_STR("2.718281828");
	double guess = FASSIGN_INT(10); // start value of 10 provides the right balance
	double thepow = FASSIGN_INT(0);
	int i = 0;
	//AECHAR strout[64];
	//FLOATTOWSTR(in, strout, 64);
	//DBGPRINTF("input is %S", strout);
	for (i = 0; i < 10; i++) { // do 10 iterations. tested out, seems like less is bad
		thepow = FPOW(exp_1, guess);		
		guess = FSUB(guess, FDIV(FSUB(thepow, in), thepow));
	}
	//FLOATTOWSTR(guess, strout, 64);
	//DBGPRINTF("guess %S", strout);
	return guess;
}
int	suryabrew_TempCalcTemp(suryabrew *pMe)
{
	/*
		USE ONLY FOR DISPLAYING TEMPERATURE!
		Upload PCM values instead of converted temp
	*/
	
	//AECHAR strout[64];
	int temp_int = 0;
	double temp_f=0;
	/*
		Convert PCM to ohms using circuit equation, formual is ((res1*inputpcm)/maxPCM) - (res1 + res2)
		res1 is from circuit
		res2 is currently zero... next revision of circuit will have this
		inputpcm is computed during calibration step. Eventually parametet set in menu
		maxPCM is value from audio stream
	*/
	int res1 = RES1;
	int res2 = RES2;
	int inputpcm = INPUTPCM; // hard code for now, eventually set in menus or automatically
//	pMe->maxSound=258;
	double ohms_d = FASSIGN_INT((res1*inputpcm)/pMe->maxSound) - (res1 + res2);
	/*
		A, B, and C are the thermistor parameters for our particular thermistor:
		http://mcshaneinc.com/html/TS165_Specs.html
		Hard code for now, eventually will be set by online config or menus on phone
	*/
	double trA = FASSIGN_STR(TRA); 
	double trB = FASSIGN_STR(TRB);
	double trC = FASSIGN_STR(TRC);
	// we need ln(ohms) and ln^3(ohms)
	double lnohms = natural_log(ohms_d);
	double lnohms_cubed = FPOW(lnohms, FASSIGN_INT(3));
	/*
		convert the ohms into temperature using thermistor equation
		T = (1/(A + B*ln(ohms) + C*ln^3(ohms)) - 273.15
	*/
	double temp_K = FASSIGN_INT(0); // Kelvin
	double temp_C = FASSIGN_INT(0); // Celcius
	temp_K = FDIV(FASSIGN_INT(1),FADD(trA, FADD(FMUL(trB, lnohms), FMUL(trC, lnohms_cubed))));
	temp_C  = FSUB(temp_K,FASSIGN_STR("273.15"));
	//FLOATTOWSTR(temp_C, strout, 64);
	//DBGPRINTF("res is %S", strout);
	//temp_int = FLTTOINT(temp_C);
	temp_f=(temp_C*(1.8))+32;
	temp_int = FLTTOINT(temp_f);
	return temp_int;
	
}
