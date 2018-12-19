/*
Caculates the inductir parameters for Boost Topology
*/

#include "Boost.h"

Boost::Boost(double Vin, double VinMargin, double Vout, double Iout, double SwitchFreequency)
	:Topology::Topology(Vin, VinMargin, Vout, Iout, SwitchFreequency)
{
	VinWorst = Boost::Vin * (1 - Boost::VinMargin/100);
	Duty = (Boost::Vout - VinWorst) / Boost::Vout;
	double period = 1.0 / Boost::SwitchFreequency;
	double timeOn = Duty * period;
	Et = Boost::Vin * timeOn;
	r = 0.4;//when r = 0.4, inductor has the best performance
	InductorCurrent = Boost::Iout / (1 - Duty);
	Inductor = Et / (r * InductorCurrent);
	InductorCurrentMin = (1 + r / 2) * InductorCurrent;
}

double Boost::GetInductor()
{
	return Inductor * 1e6;//uH
}

double Boost::GetInductorCurrentMin()
{
	return InductorCurrentMin;
}