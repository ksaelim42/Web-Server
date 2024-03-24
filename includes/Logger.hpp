#ifndef LOGGER_HPP
# define LOGGER_HPP

# define RED	"\e[0;31m"
# define GRN	"\e[0;32m"
# define YEL	"\e[0;33m"
# define BLU	"\e[0;34m"
# define MAG	"\e[0;35m"
# define CYN	"\e[0;36m"
# define WHT	"\e[0;37m"
# define RESET	"\e[0m"
# define CRLF	"\r\n"

# define BBLU	"\e[1;34m"
# define BYEL	"\e[1;33m"

#include <sstream>
#include <iostream>

enum LogLevel { INFO, DEBUG, WARNING , ERROR };

class Logger
{
	private:
		static LogLevel	_level;
	public:
		static void	setLevel(LogLevel);
		static bool	isLog(LogLevel);

		static bool	log(const char * clr, const char * msg1) {
			std::cout << clr << msg1 << RESET << std::endl;
			return true;
		}

		template<typename T1>
		static bool	log(const char * clr, T1 & msg1) {
			std::cout << clr << msg1 << RESET << std::endl;
			return true;
		}

		template<typename T1, typename T2>
		static bool	log(const char * clr, T1 & msg1, T2 & msg2) {
			std::cout << clr << msg1 << msg2 << RESET << std::endl;
			return true;
		}

		template<typename T1, typename T2, typename T3>
		static bool	log(const char * clr, T1 & msg1, T2 & msg2, T3 & msg3) {
			std::cout << clr << msg1 << msg2 << msg3 << RESET << std::endl;
			return true;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		static bool	log(const char * clr, T1 & msg1, T2 & msg2, T3 & msg3, T4 & msg4) {
			std::cout << clr << msg1 << msg2 << msg3 << msg4 << RESET << std::endl;
			return true;
		}

		template<typename T1, typename T2, typename T3, typename T4, typename T5>
		static bool	log(const char * clr, T1 & msg1, T2 & msg2, T3 & msg3, T4 & msg4, T5 & msg5) {
			std::cout << clr << msg1 << msg2 << msg3 << msg4 << msg5 << RESET << std::endl;
			return true;
		}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
		static bool	log(const char * clr, T1 & msg1, T2 & msg2, T3 & msg3, T4 & msg4, T5 & msg5, T6 & msg6) {
			std::cout << clr << msg1 << msg2 << msg3 << msg4 << msg5 << msg6 << RESET << std::endl;
			return true;
		}
};

#endif