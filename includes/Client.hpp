#ifndef CLIENT_HPP
# define CLIENT_HPP

# define REQ_HEADER			0x0
# define REQ_BODY_LENGTH	0x1
# define REQ_BODY_CHUNK		0x2

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
		// bool		_parseHeader(std::string &, std::string &, std::string &);
		bool		_divideHeadBody(std::string &);
		bool		_parsePath(std::string);
		bool		_urlEncoding(std::string & path);
		bool		_matchLocation(std::vector<Location>);
		bool		_redirect(void);
		bool		_findFile(void);
		bool		_findType(void);
		bool		_findBodySize(void);
		int			_unChunk(char * &, size_t &);
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
		short int	getStatus(void) const;
		reqType_e	getReqType(void) const;
		void		parseRequest(char *, size_t);
		void		genResponse(std::string &);
		void		setResponse(short int);
		void		prtParsedReq(void);
		void		prtRequest(httpReq &);
};

#endif