/*
Caculates the inductir parameters for Buck Topology 
*/

#include "Buck.h"

Buck::Buck(double Vin, double VinMargin, double Vout, double Iout, double SwitchFreequency)
	:Topology::Topology(Vin, VinMargin, Vout, Iout, SwitchFreequency)
{
	VinWorst = Buck::Vin * (1 + Buck::VinMargin / 100);
	Duty = (Buck::Vout + 1.1 - 1.1) / VinWorst;
	double period = 1.0 / Buck::SwitchFreequency;
	double timeOff = (1 - Duty) * period;
	Et = Buck::Vout * timeOff;
	r = 0.4;//when r = 0.4, inductor has the best performance
	InductorCurrent = Buck::Iout;
	Inductor = Et /(r * InductorCurrent);
	InductorCurrentMin = (1 + r / 2) * InductorCurrent;
}

double Buck::GetInductor()
{
	return Inductor * 1e6;//uH
}

double Buck::GetInductorCurrentMin()
{
	return InductorCurrentMin;
}