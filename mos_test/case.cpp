#include <iostream>

enum Day
{
	Mon,
	Tue,
	Wen,
	Thr,
	Fri,
	Sat,
	San
};

int	main(void) {

	int	a = 20;
	Day	day = Mon;

	switch (day) {
		case Mon:
		{
			if (a > 20) {
				std::cout << "Hello" << std::endl;
			}
			else if (a > 10){
				std::cout << "World" << std::endl;
				day = Tue;
				break;
			}
			else
				std::cout << "New" << std::endl;
			std::cout << "One Piece" << std::endl;
		}
		case Tue:
		{
			if (a < 30) {
				std::cout << "Me" << std::endl;
			}
		}
	}
	
}