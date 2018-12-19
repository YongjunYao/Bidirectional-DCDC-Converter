#ifndef _TOPOLOGY_
#define _TOPOLOGY_

//Topology is the father class of Buck topology and Boost topology
class Topology
{
public:
	Topology();
	Topology(double Vin, double VinMargin, double Vout, double Iout, double SwitchFreequency);
	virtual ~Topology();

	double virtual GetInductor() = 0;
	double virtual GetInductorCurrentMin() = 0;
	void Setr(double);
	double Vin;
	double VinMargin;
	double Vout;
	double Iout;
	double SwitchFreequency;

	double Inductor;
	double r;
	double InductorCurrent;
	double InductorCurrentMin;
};

#endif
