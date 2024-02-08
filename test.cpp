#include "include/Utils.hpp"
#include <iostream>
#include <unistd.h>
// bad_alloc example
#include <new>          // std::bad_alloc

int main () {
  try
  {
	for (int i = 0; i < 1000; i++) {
		int* myarray= new int[10000000000];
		usleep(5000);
	}
	std::cout << "allocate memory success" << std::endl;
  }
  catch (std::bad_alloc& ba)
  {
    std::cerr << "bad_alloc caught: " << ba.what() << '\n';
  }
// for (int i = 0; i < 1000; i++) {
// 	int* myarray = new int[10000000000];
// 	usleep(50000);
// }
  return 0;
}

// int	main(void)
// {
// 	std::map<std::string, std::string>	set;
// 	set["AUTH_TYPE"] = "test";
// 	set["CONTENT_LENGTH"] = "test";
// 	set["CONTENT_TYPE"] = "test";
// 	set["GATEWAY_INTERFACE"] = "test";
// 	set["PATH_INFO"] = "test";
// 	set["PATH_TRANSLATED"] = "test";
// 	set["QUERY_STRING"] = "test";
// 	set["REMOTE_ADDR"] = "test";
// 	set["REMOTE_HOST"] = "test";
// 	set["REMOTE_IDENT"] = "test";
// 	set["REMOTE_USER"] = "test";
// 	set["REQUEST_METHOD"] = "test";
// 	set["SCRIPT_NAME"] = "test";
// 	set["SERVER_NAME"] = "test";
// 	set["SERVER_PORT"] = "test";
// 	set["SERVER_PROTOCOL"] = "test";
// 	set["SERVER_SOFTWARE"] = "test";
// 	char**	aop = aopdup(set);
// 	for (int i = 0; aop[i]; i++)
// 		std::cout << aop[i] << std::endl;
// }