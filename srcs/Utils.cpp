#include "Utils.hpp"

std::string	toProtoEnv(std::string var) {
	std::string	key = "HTTP_";
	for (int i = 0; i < var.length(); i++) {
		if (var[i] == '-')
			var[i] = '_';
		var[i] = std::toupper(var[i]);
	}
	return key + var;
}

std::string	numToStr(size_t num) {
	std::stringstream	ss;
	ss << num;
	return ss.str();
}

size_t	strToNum(std::string str) {
	size_t				num;
	std::stringstream	ss(str);
	ss >> num;
	return num;
}

char*	strdup(std::string src) {
	char*	dest = new char[src.length() + 1];
	size_t	i = 0;
	for (; i < src.length(); i++)
		dest[i] = src[i];
	dest[i] = '\0';
	return (dest);
}

char**	aopEnv(std::map<std::string, std::string> set) {
	char**	aop = new char*[set.size() + 1];
	std::map<std::string, std::string>::const_iterator	it = set.begin();
	size_t	i = 0;
	for (;it != set.end(); i++, it++) {
		aop[i] = strdup(it->first + "=" + it->second);
	}
	aop[i] = NULL;
	return aop;
}

char**	aopArgs(std::vector<std::string> set) {
	char**	aop = new char*[set.size() + 1];
	size_t	i = 0;
	for (;i < set.size(); i++)
		aop[i] = strdup(set[i]);
	aop[i] = NULL;
	return aop;
}

std::map<std::string, std::string>	initMineTypeDefault(void) {
	std::map<std::string, std::string>	mimeType;

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

	// application it is a top-level media type (not Text or Multimedia)
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
	return mimeType;
}

void	prtMap(std::map<std::string, std::string> set) {
	std::map<std::string, std::string>::const_iterator	it;
	size_t	i = 0;
	for (it = set.begin(); it != set.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
}