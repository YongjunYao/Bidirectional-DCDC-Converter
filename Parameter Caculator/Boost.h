#include "Topology.h"

class Boost: public Topology
{
public:
	Boost(double Vin, double VinMargin, double Vout, double Iout, double SwitchFreequency);
	~Boost() {};
	double virtual GetInductor();
	double virtual GetInductorCurrentMin();
private:	     //Supporting parameters
	double VinWorst;
	double Duty;
	double Et;
};
