
#include "MpcProperty.hh"

#include <sstream> 
#include <iostream>

int main() 
{ 
	MpcProperty<int> Integer; 
	Integer.on_change().connect([](int val) 
		{ 
			std::cout << "Value changed to: " << val << std::endl; 
		}); 

	std::cout << "Value: " << Integer << std::endl; 
	std::stringstream sstr("42"); 
	sstr >> Integer; 
	return 0; 
}
