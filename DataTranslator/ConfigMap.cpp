#include <fstream>
#include <stdexcept>

#include "ConfigMap.h"

//simple mapper for configs
CConfigMap::CConfigMap(const std::string path) 
{
	std::ifstream is(path);

	if (!is.is_open())
	{
		throw std::logic_error("config file error: " + path);
	}

	std::string line;
	std::string column;
	std::string our_column;

	while (!is.eof())
	{
		std::getline(is, column, '\t');
		std::getline(is, our_column);

		_map[column] = our_column;
	}

	is.close();
}

const std::string CConfigMap::map(std::string key)
{
	return _map[key];
}

bool CConfigMap::Contains(std::string key)
{
	return (_map.find(key) != _map.end());
}
