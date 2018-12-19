/*
This is a caculator help design the inductor, which significantly reduce the effort in caculating;
*/

#include <iostream>
#include "buck.h"
#include "boost.h"
using namespace std;

int main()
{
	unsigned int Mode = 0;
	double Vin = 0;
	double VinMargin = 0;
	double Vout = 0;
	double Iout = 0;
	double SwitchFreequency = 0;
	cout << "Following the guide will give you the parameter of the inductor." << endl;
	cout << endl;

	while (Mode != 3)
	{
		//input checks
		try {
			cout << "Select mode: 1 for Buck, 2 for Boost" << endl;
			cin >> Mode;
			cout << "Vin (V) :";
			cin >> Vin;
			cout << "VinMargin (??%) :";
			cin >> VinMargin;
			cout << "Vout (V) :";
			cin >> Vout;
			cout << "Iout (A) :";
			cin >> Iout;
			cout << "SwitchFreequency (Hz) :";
			cin >> SwitchFreequency;
		}
		catch (exception exp)
		{
			cout << "Invalid Input" << endl;
		}

		//Caculates based on the mode
		switch (Mode)
		{
		case 1:
		{
			cout << "Buck Inductor Parameters :" << endl;
			Buck myBuck(Vin, VinMargin, Vout, Iout, SwitchFreequency);
			cout << "Inductor: " << myBuck.GetInductor() << "uH" << endl;
			cout << "InductorCurrentMin:" << myBuck.GetInductorCurrentMin() << "A" << endl;
			break;
		}
		case 2:
		{
			cout << "Boost Inductor Parameters" << endl;
			Boost myBoost(Vin, VinMargin, Vout, Iout, SwitchFreequency);
			cout << "Inductor: " << myBoost.GetInductor() << "uH" << endl;
			cout << "InductorCurrentMin:" << myBoost.GetInductorCurrentMin() << "A" << endl;
			break;
		}
		case 3:
			cout << "End" << endl;
			break;
		default:
			break;
		}
	}
	return 0;
}
