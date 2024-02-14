#ifndef LOCATION_HPP
# define LOCATION_HPP

class Location
{
	private:
		std::string					_uri;	// path
		std::string					_root;
		std::vector<std::string>	_index;
		std::vector<std::string>	_return;
};

#endif