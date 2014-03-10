#pragma once

#include <string>
#include <map>

class CConfigMap
{
public:
	CConfigMap(const std::string path);
	const std::string map(std::string key);
	bool Contains(std::string key);

private:
	std::map<std::string, std::string> _map;
};