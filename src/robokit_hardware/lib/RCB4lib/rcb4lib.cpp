#include "rcb4lib.hpp" 

bool Rcb4Serial::open(const std::string& name, const long baudrate, const int timeout)
{
	std::unique_ptr<serial::Serial> new_port (new serial::Serial(name, baudrate, serial::Timeout::simpleTimeout(timeout))); 
	
	port_.swap(new_port);

	std::cout << "Opened port " << name << " Maybe..................\n";

	return !!port_;
}

bool Rcb4Serial::synchronize(byte txBuf[],byte txLen,byte rxBuf[],byte rxLen)
{

	if (!port_ || !port_->isOpen()) throw "Your port seems ill";

	_isSynchronize = true;
	port_->flush();
	port_->write(txBuf, txLen);
	
	if(port_->read(rxBuf,rxLen) == rxLen)
	{
		_isSynchronize = false;
		
		if(rxLen != rxBuf[0])
		{
			return	false;
		}
					
		return checkReverceCmd(rxBuf,txBuf[1]);
	}
	
	else
	{	
		_isSynchronize = false;
		return	false;
	}

	return true;
}




