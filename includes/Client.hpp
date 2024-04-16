#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <signal.h>

#include "Utils.hpp"
#include "Struct.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

enum pipe_e {
	PIPE_IN,
	PIPE_OUT
};

class Client
{
	private:
		parsedReq		_req;
		HttpResponse	_res;
		int				_pipeIn;
		int				_pipeOut;

		// Parsing Request
		void		_initRequest(void);
		bool		_parseHeader(char *, size_t &);
		bool		_divideHeadBody(std::string &);
		bool		_parsePath(std::string);
		bool		_urlEncoding(std::string &);
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
		short int			status;
		pid_t				pid;
		char				buffer[BUFFERSIZE];
		size_t				bufSize;
		std::string			resMsg;
		std::string			IPaddr;
		Server *			serv;
		socklen_t			addrLen;
		struct sockaddr_in	addr;
		std::time_t			lastTimeConnected;

		static std::map<int, Client*>	pipeFds; // Pipe monitoring

		Client(void);
		~Client(void);
		bool		parseHeader(char *, size_t &);
		void		genResponse(void);
		int			openFile(void);
		bool		readFile(int, char*);
		void		updateTime(void);
		bool		isSizeTooLarge(ssize_t);
		// Getter & Setter
		int				getPipeIn(void) const;
		int				getPipeOut(void) const;
		void			setReqType(reqType_e);
		void			setResType(resType_e);
		reqType_e		getReqType(void) const;
		resType_e		getResType(void) const;
		parsedReq&		getRequest(void);
		HttpResponse&	getResponse(void);
		// Pipe Fd
		void	addPipeFd(int, pipe_e);
		void	delPipeFd(int, pipe_e);
		void	clearPipeFd(void);
		// debugging
		void	prtParsedReq(void);
		void	prtRequest(httpReq &);
};

#endif