#include <sstream>
#include <fstream>

#include "DataParser.h"
#include "ParseSummary.h"

//As I was not sure if all files have same set of columns or not
//parser process first line in each data file
//to optimize data processing it builds columns indexes for referense while parsing
void CDataParser::ProcessHeader()
{
	using namespace std;

	string s;
	getline(_inf, s);
	istringstream iss(s);

	//read vendor id column
	getline(iss, s, '\t');

	//save vendor id
	_outf << _columns_map.map(s);

	int i = 0;
	while (!iss.eof())
	{
		getline(iss, s, '\t');

		if (_columns_map.Contains(s))
		{
			_outf << '\t' << _columns_map.map(s);
			_columns_indexes.push_back(i);
		}

		++i;
	}

	_outf << endl;
}

CDataParser::CDataParser(const std::string& path, int file_index, CConfigMap& columns_map, CConfigMap& ids_map)
: _columns_map(columns_map), _ids_map(ids_map), _path(path), _file_index(file_index)
{
}

bool CDataParser::Init()
{
	std::ostringstream data_filename;
	data_filename << _path << "dataFile" << _file_index << ".tsv";
	_data_filename = data_filename.str();
	_inf = std::ifstream(_data_filename);

	if (!_inf.is_open())
	{
		return false;
	}

	std::ostringstream out_filename;
	out_filename << _path << "outputFile" << _file_index << ".tsv";
	_outf = std::ofstream(out_filename.str());

	return true;
}

void CDataParser::Parse()
{
	if (!_inf.is_open())
	{
		return;
	}
		
	//build columns indexes and write first line of the output file
	ProcessHeader();
	
	std::string s;
	while (!_inf.eof())
	{
		//read and process first column
		bool id_exist = WriteIdColumn();

		//then read rest of the line
		std::getline(_inf, s); 

		//if id is skipped we sont need to process rest data in the line
		if (id_exist)
		{
			//istringstream is used to avoid situation when some tabs are missed 
			//as a result of data corrruption
			std::istringstream iss(s);
			WriteRestColumns(iss);
			CParseSummary::output_rows_number++;
		}

		CParseSummary::input_rows_number++;
	}
}

bool CDataParser::WriteIdColumn()
{
	using namespace std;

	string vendor_id;

	getline(_inf, vendor_id, '\t');

	if (!_ids_map.Contains(vendor_id))
	{
		return false;
	}

	_outf << _ids_map.map(vendor_id);

	return true;
}

//read ress of the line from the stringstream
//then skip columns which are not in the config and write down 
//selected columns
void CDataParser::WriteRestColumns(std::istringstream& in)
{
	using namespace std;

	string s;
	int index = 0;
	auto columns_iterator = _columns_indexes.begin();

	while (!in.eof() && columns_iterator != _columns_indexes.end())
	{
		getline(in, s, '\t');

		if (index == *columns_iterator)
		{
			_outf << '\t' << s;
			++columns_iterator;
		}

		++index;
	}

	_outf << endl;
}

CDataParser::~CDataParser()
{
	_outf.close();
	_inf.close();
}
