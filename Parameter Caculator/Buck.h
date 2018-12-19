#include "Topology.h"

class Buck :public Topology
{
public:
	Buck(double Vin, double VinMargin, double Vout, double Iout, double SwitchFreequency);
	~Buck() {};
	double virtual GetInductor();
	double virtual GetInductorCurrentMin();
private:		//Supporting parameters
	double VinWorst;
	double Duty;
	double Et;
};
