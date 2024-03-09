#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Utils.hpp"
#include "Server.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "CgiHandler.hpp"

class Client
{
	private:
		parsedReq		_req;
		HttpResponse	_res;
		struct stat		_fileInfo;
		short int		_status;

		// Parsing Request
		bool		_parsePath(std::string);
		bool		_urlEncoding(std::string & path);
		bool		_matchLocation(std::vector<Location>);
		std::string	_findContent(std::map<std::string, std::string> &, std::string const &);
		bool		_findFile(void);
		bool		_findType(void);
		// Check Header
		bool		_checkRequest(void);
		bool		_checkMethod(std::string);
		bool		_checkVersion(std::string);
	public:
		int					sockFd;
		Server *			serv;
		short int			status;
		struct sockaddr_in	addr;
		socklen_t			addrLen;
		std::string			body;

		Client(void);
		~Client(void) {}
		void	parseRequest(std::string);
		void	genResponse(std::string &);
		void	prtParsedReq(void);
		void	prtRequest(httpReq &);
};

#endif