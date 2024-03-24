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
		void		_initReqParse(void);
		bool		_parseHeader(char *, size_t &);
		bool		_divideHeadBody(std::string &);
		bool		_parsePath(std::string);
		bool		_urlEncoding(std::string & path);
		bool		_matchLocation(std::vector<Location>);
		// Check Header
		bool		_checkRequest(void);
		bool		_checkMethod(std::string);
		bool		_checkVersion(std::string);
		// Find Resourse File
		bool		_redirect(void);
		bool		_findFile(void);
		bool		_findType(void);
		bool		_findBodySize(void);
	public:
		int					sockFd;
		std::string			IPaddr;
		Server *			serv;
		socklen_t			addrLen;
		struct sockaddr_in	addr;

		Client(void);
		~Client(void);
		void		parseRequest(char *, size_t);
		void		genResponse(std::string &);
		void		setResponse(short int);
		void		prtParsedReq(void);
		void		prtRequest(httpReq &);
		// Getter
		short int	getStatus(void) const;
		reqType_e	getReqType(void) const;
};

#endif