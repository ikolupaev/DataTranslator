#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "ConfigMap.h"

class CDataParser
{
public:
	CDataParser(const std::string& path, int file_index, CConfigMap& columns_map, CConfigMap& ids_map);
	void Parse();
	bool Init();
	std::string& GetDataFileName() { return _data_filename; };

	virtual ~CDataParser();

	static int output_rows_number;

private:
	void ProcessHeader();
	bool WriteIdColumn();
	void WriteRestColumns(std::istringstream& in);

	CConfigMap& _columns_map;
	CConfigMap& _ids_map;
	
	std::vector<int> _columns_indexes;

	std::ifstream _inf;
	std::ofstream _outf;

	const std::string& _path;
	const int _file_index;

	std::string _data_filename;
};