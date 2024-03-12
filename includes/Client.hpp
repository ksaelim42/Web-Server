#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Utils.hpp"
#include "Struct.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "CgiHandler.hpp"

class Client
{
	private:
		short int		_status;
		struct stat		_fileInfo;
		parsedReq		_req;
		HttpResponse	_res;
		CgiHandler		_cgi;

		// Parsing Request
		bool		_parsePath(std::string);
		bool		_urlEncoding(std::string & path);
		bool		_matchLocation(std::vector<Location>);
		bool		_findFile(void);
		bool		_findType(void);
		// Check Header
		bool		_checkRequest(void);
		bool		_checkMethod(std::string);
		bool		_checkVersion(std::string);
	public:
		int					sockFd;
		Server *			serv;
		struct sockaddr_in	addr;
		socklen_t			addrLen;

		Client(void);
		~Client(void) {}
		void	parseRequest(std::string);
		void	genResponse(std::string &);
		void	prtParsedReq(void);
		void	prtRequest(httpReq &);
};

#endif