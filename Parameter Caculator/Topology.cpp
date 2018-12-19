#include "Topology.h"

Topology::Topology()
{
	//TODO
}

Topology::Topology(double Vin, double VinMargin, double Vout, double Iout, double SwitchFreequency) :
	Vin(Vin), VinMargin(VinMargin), Vout(Vout), Iout(Iout), SwitchFreequency(SwitchFreequency)
{
	//TODO
}

Topology::~Topology()
{
	//TODO
}

void Topology::Setr(double r)
{
	this->r = r;
}