#include "../include/Server.hpp"

Server::Server(void) {
	// if didn't specify Types context Server will use default Types
	if (1) { //:TODO fix to check config file
		initMineTypeDefault();
	}
}

bool	readFile(std::string name, std::string &str)
{
	std::ifstream	inFile;
	char*			buffer;
	int				length;

	inFile.open(name.c_str());		// Convert string to char* by c_str() function
	if (!inFile.is_open())
	{
		std::cerr << name << " :Can not open." << std::endl;
		return (false);
	}
	inFile.seekg(0, inFile.end);	// Set position to end of the stream
	length = inFile.tellg();		// Get current position
	inFile.seekg(0, inFile.beg);	// Set position back to begining of the stream
	buffer = new char [length];		// Allocate Size of Buffer as size of inFile
	inFile.read(buffer, length);	// Read all data in inFile to Buffer
	str = buffer;					// Copy string from Buffer to str
	delete [] buffer;				// Free Buffer
	inFile.close();					// Close inFile
	return (true);
}

int	Server::acceptConnection(int serverSock) {
	int					client_fd;
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrLen = sizeof(clientAddr);

	// Accept connection
	client_fd	= accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (client_fd < 0)
		throw ServerException("Accept fail");
	else {
		std::cout << GREEN << "connection accepted from " << RESET << std::endl;
		std::cout << "client fd: " << client_fd \
		<< ", client IP address: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
	}
	return client_fd;
}

void	handle_client(int client_fd) {
	char	buffer[1024];

	// Receive data from client
	ssize_t	bytes_received =  recv(client_fd, buffer, 1024 - 1, 0);
	if (bytes_received < 0) {
		std::cerr << "Error receiving data" << std::endl;
		close(client_fd);
		exit(1);
	} else if (bytes_received == 0) {
		std::cerr << "Client disconnected" << std::endl;
		close(client_fd);
		exit(1);
	}
	std::cout << "Data receive: " << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << buffer << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	return ;
}

void	http_reponse(int client_fd, const char *path) {
	int	status;

	std::string	statusLine = "HTTP/1.1 200 OK\r\n";
	// std::string	headers = "Content-Type: text/html\r\n\r\n";
	// std::string	headers = "Accept-Ranges: bytes\r\nContent-Length: 72660\r\nContent-Type: text/plain\r\n\r\n";
	std::string	headers = IMAGE_HEADER;
	std::string	body;

	if (readFile(path, body) == 0) {
		close(client_fd);
		exit(1);
	}
	else 
		std::cout << "Read file success" << std::endl;
	std::string	content = statusLine + headers + body;
	status = send(client_fd, content.c_str(), content.length(), 0);
	if (status < 0) {
		std::cerr << "Error to response data" << std::endl;
		close(client_fd);
		exit(1);
	}
	std::cout << "Sent data success" << std::endl;
	return ;
}

int	Server::initServer() {
	int					status;
	int					server_fd;
	unsigned short int	netPort;
	int					enable;
	struct sockaddr_in	server_address;

	// Creating socket file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw ServerException("Create socket fail");

	// To manipulate option for socket and check address is used
	enable = 1;
	status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	if (status < 0)
		throw ServerException("Setup socket fail");

	// Set address and port
	server_address.sin_family = AF_INET; // set for IPv4
	server_address.sin_port = htons(PORT); // convert host byte order to network byte order
	server_address.sin_addr.s_addr = INADDR_ANY; // Bind to all available network interfaces

	// Bind the socket to the specified address and port
	status = bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address));
	if (status < 0)
		throw ServerException("Bind socket fail");

	// Prepare socket for incoming connection
	status = listen(server_fd, 10);
	if (status < 0)
		throw ServerException("Listen socket fail");

	std::cout << GREEN << "Success to create server" << RESET << std::endl;
	std::cout << "Domain name : localhost" << ", port : " << PORT << std::endl;
	return server_fd;
}

int	Server::runServer(int serverSock) {
	int	client_fd;

	while (1) {
		client_fd = acceptConnection(serverSock);

		handle_client(client_fd);
		// http_reponse(client_fd, HTML_FILE);
		http_reponse(client_fd, IMAGE_FILE);
		// usleep(500000);
		// http_reponse(client_fd, IMAGE_FILE);
		close(client_fd);
	}
	sleep(3);
	close(serverSock);
	std::cout << "close server" << std::endl;
	return 1;
}

bool	Server::initMineTypeDefault(void) {
	mimeType["default"] = "text/plain";

	mimeType["html"] = "text/html";
	mimeType["htm"] = "text/html";
	mimeType["shtml"] = "text/html";
	mimeType["css"] = "text/css";
	mimeType["xml"] = "text/xml";
	mimeType["mml"] = "text/mathml";
	mimeType["txt"] = "text/plain";
	mimeType["jad"] = "text/vnd.sun.j2me.app-descriptor";
	mimeType["wml"] = "text/vnd.wap.wml";
	mimeType["htc"] = "text/x-component";

	mimeType["gif"] = "image/gif";
	mimeType["jpeg"] = "image/jpeg";
	mimeType["jpg"] = "image/jpeg";
	mimeType["png"] = "image/png";
	mimeType["tif"] = "image/tiff";
	mimeType["tiff"] = "image/tiff";
	mimeType["wbmp"] = "image/vnd.wap.wbmp";
	mimeType["ico"] = "image/x-icon";
	mimeType["jng"] = "image/x-jng";
	mimeType["bmp"] = "image/x-ms-bmp";
	mimeType["svg"] = "image/svg+xml";
	mimeType["svgz"] = "image/svg+xml";
	mimeType["webp"] = "image/webp";

	mimeType["js"] = "application/javascript";
	mimeType["atom"] = "application/atom+xml";
	mimeType["rss"] = "application/rss+xml";
	mimeType["woff"] = "application/font-woff";
	mimeType["jar"] = "application/java-archive";
	mimeType["war"] = "application/java-archive";
	mimeType["ear"] = "application/java-archive";
	mimeType["json"] = "application/json";
	mimeType["hqx"] = "application/mac-binhex40";
	mimeType["doc"] = "application/msword";
	mimeType["pdf"] = "application/pdf";
	mimeType["ps"] = "application/postscript";
	mimeType["eps"] = "application/postscript";
	mimeType["ai"] = "application/postscript";
	mimeType["rtf"] = "application/rtf";
	mimeType["m3u8"] = "application/vnd.apple.mpegurl";
	mimeType["xls"] = "application/vnd.ms-excel";
	mimeType["eot"] = "application/vnd.ms-fontobject";
	mimeType["ppt"] = "application/vnd.ms-powerpoint";
	mimeType["wmlc"] = "application/vnd.wap.wmlc";
	mimeType["kml"] = "application/vnd.google-earth.kml+xml";
	mimeType["kmz"] = "application/vnd.google-earth.kmz";
	mimeType["7z"] = "application/x-7z-compressed";
	mimeType["cco"] = "application/x-cocoa";
	mimeType["jardiff"] = "application/x-java-archive-diff";
	mimeType["jnlp"] = "application/x-java-jnlp-file";
	mimeType["run"] = "application/x-makeself";
	mimeType["pl"] = "application/x-perl";
	mimeType["pm"] = "application/x-perl";
	mimeType["prc"] = "application/x-pilot";
	mimeType["pdb"] = "application/x-pilot";
	mimeType["rar"] = "application/x-rar-compressed";
	mimeType["rpm"] = "application/x-redhat-package-manager";
	mimeType["sea"] = "application/x-sea";
	mimeType["swf"] = "application/x-shockwave-flash";
	mimeType["sit"] = "application/x-stuffit";
	mimeType["tcl"] = "application/x-tcl";
	mimeType["tk"] = "application/x-tcl";
	mimeType["der"] = "application/x-x509-ca-cert";
	mimeType["pem"] = "application/x-x509-ca-cert";
	mimeType["crt"] = "application/x-x509-ca-cert";
	mimeType["xpi"] = "application/x-xpinstall";
	mimeType["xhtml"] = "application/xhtml+xml";
	mimeType["xspf"] = "application/xspf+xml";
	mimeType["zip"] = "application/zip";
	mimeType["bin"] = "application/octet-stream";
	mimeType["exe"] = "application/octet-stream";
	mimeType["dll"] = "application/octet-stream";
	mimeType["deb"] = "application/octet-stream";
	mimeType["dmg"] = "application/octet-stream";
	mimeType["iso"] = "application/octet-stream";
	mimeType["img"] = "application/octet-stream";
	mimeType["msi"] = "application/octet-stream";
	mimeType["msp"] = "application/octet-stream";
	mimeType["msm"] = "application/octet-stream";
	mimeType["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mimeType["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mimeType["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";

	mimeType["mid"] = "audio/midi";
	mimeType["midi"] = "audio/midi";
	mimeType["kar"] = "audio/midi";
	mimeType["mp3"] = "audio/mpeg";
	mimeType["ogg"] = "audio/ogg";
	mimeType["m4a"] = "audio/x-m4a";
	mimeType["ra"] = "audio/x-realaudio";

	mimeType["3gpp"] = "video/3gpp";
	mimeType["3gp"] = "video/3gpp";
	mimeType["ts"] = "video/mp2t";
	mimeType["mp4"] = "video/mp4";
	mimeType["mpeg"] = "video/mpeg";
	mimeType["mpg"] = "video/mpeg";
	mimeType["mov"] = "video/quicktime";
	mimeType["webm"] = "video/webm";
	mimeType["flv"] = "video/x-flv";
	mimeType["m4v"] = "video/x-m4v";
	mimeType["mng"] = "video/x-mng";
	mimeType["asx"] = "video/x-ms-asf";
	mimeType["asf"] = "video/x-ms-asf";
	mimeType["wmv"] = "video/x-ms-wmv";
	mimeType["avi"] = "video/x-msvideo";
}
