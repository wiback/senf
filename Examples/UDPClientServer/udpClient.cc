#include <iostream>
#include <sstream>
#include <senf/Socket/Protocols/INet.hh>

int main(int argc, char const * argv[])
{
	try 
	{
		for (int i=0; i<=10; i++) 
		{
			senf::UDPv4ClientSocketHandle sock;
			std::stringstream s;
			s << i;
			sock.writeto(senf::INet4SocketAddress("127.0.0.1:4243"),s.str());
			std::cout << i << std::endl;
		}
	}
	catch (std::exception const & ex) 
	{
		std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
	}
	
	return 0;
}
