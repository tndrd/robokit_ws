#include "serial/serial.h"
#include "Rcb4BaseClass.h"

#include <string>
#include <iostream>
#include <memory>

class Rcb4Serial : public Rcb4BaseClass
{
	protected:
		std::unique_ptr<serial::Serial> port_;
		bool active_ = false;
		 	
	public:
		Rcb4Serial();
		~Rcb4Serial();

		bool open(const std::string& name, const long baudrate, const int timeout);
		
		//void activate();
		//bool deactivate();
		
		virtual bool synchronize(byte txBuf[],byte txLen,byte rxBuf[],byte rxLen);
};