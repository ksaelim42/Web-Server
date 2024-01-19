#include <iostream>
#include <netdb.h>

int	main(void) {
	struct protoent	var;

	std::cout << "size of struct: " << sizeof(var) << std::endl;
	std::cout << sizeof(var.p_name) << std::endl;
	std::cout << sizeof(var.p_aliases) << std::endl;
	std::cout << sizeof(var.p_proto) << std::endl;
}
