#include "serial/serial.h"
#include "rcb4serial/Rcb4BaseClass.hpp"

#include <string>
#include <iostream>
#include <memory>

class Rcb4Serial : public Rcb4BaseClass
{
	protected:
		std::unique_ptr<serial::Serial> port_;
		bool active_ = false;
		 	
	public:
		bool open(const std::string& name, const long baudrate, const int timeout);
		bool synchronize(byte txBuf[],byte txLen,byte rxBuf[],byte rxLen) override;
};