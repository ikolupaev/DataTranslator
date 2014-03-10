#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <queue>
#include <mutex>

#include "ConfigMap.h"
#include "DataParser.h"
#include "ParseSummary.h"

using namespace std;

atomic<int> active_threads;
atomic<int> files_processed;
atomic<int> files_failed;

//mutex is for keep output ordered
mutex output_mutex;

//thread function to run parser
void parse_data(CDataParser *parser)
{
	active_threads++;

	try
	{
		parser->Parse();
	}
	catch (exception& ex)
	{
		output_mutex.lock();

		files_failed++;
		cerr << "unexpected error in processing data file: " << parser->GetDataFileName() << endl;
		cerr << "error message: " << ex.what() << endl;

		output_mutex.unlock();
	}

	delete parser;

	files_processed++;
	active_threads--;
}

void update_status()
{
	for (;;)
	{
		if (active_threads > 0)
		{
			output_mutex.lock();

			std::cout << "active threads: " << active_threads;
			std::cout << "; parsed/failed: " << files_processed << "/" << files_failed;
			std::cout << "; rows (in/out): " << CParseSummary::input_rows_number << "/" << CParseSummary::output_rows_number;

			std::cout << endl;

			output_mutex.unlock();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void normalize_path(string& path)
{
	char ch = path[path.size() - 1];
	if (ch != '/' && ch != '\\')
	{
		path.append("/");
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "Error: no input data path. Usage:" << endl << endl;
		std::cout << "translator.exe <path>" << endl;
		std::cout << "<path> -- path, where input data is located" << endl;

		return 1;
	}

	string path(argv[1]);
	normalize_path(path);

	try
	{
		cout << "Reading column_mapping.tsv...";
		CConfigMap columns_map(path + "column_mapping.tsv");
		cout << "done." << endl;

		cout << "Reading identifier_mapping.tsv...";
		CConfigMap ids_map(path + "identifier_mapping.tsv");
		cout << "done." << endl;

		int max_threads = thread::hardware_concurrency();
		if (max_threads == 0)
		{
			max_threads = 1;
		};

		cout << "Max threads set according to harwdare config: " << max_threads << endl;

		int file_index = 0;
		int data_open_errors_inrow = 0;

		//To don't bother user with long list of missed files at the end at first missed files in the middle 
		//are stored to the queue and then show if next file is present.
		//
		//I deside to don't use find_file_first/next to avoid OS specific behaviour. 
		queue<string> missed_files;

		for (;;)
		{
			if (active_threads < max_threads)
			{
				auto parser = new CDataParser(path, file_index++, columns_map, ids_map);

				if (parser->Init())
				{
					data_open_errors_inrow = 0;

					while (!missed_files.empty())
					{
						output_mutex.lock();
						cerr << "Data file is bad or missed: " << missed_files.front() << endl;
						missed_files.pop();
						output_mutex.unlock();
					}

					new thread(parse_data, parser);

					//run summary thread
					std::thread(update_status).detach();
				}
				else
				{
					data_open_errors_inrow++;

					missed_files.push(parser->GetDataFileName());

					delete parser;
				}
			}

			//if 20 files are missed in the row we suppose that no more files are in the folder
			if (data_open_errors_inrow > 20)
			{
				break;
			}
		}

		output_mutex.lock();

		cout << endl;
		cout << "More than 20 files are missed in a row." << endl;
		cout << "Supposed that no more files are in the folder." << endl;
		cout << "Waiting for the active " << active_threads << " thread(s) to complete." << endl;
		cout << endl;

		output_mutex.unlock();

		while (active_threads > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		}
	}
	catch (logic_error& error)
	{
		std::cout << error.what() << endl;
	}

	output_mutex.lock();

	cout << endl;
	cout << "All threads are completed." << endl;
	std::cout << "data files processed successfully: " << files_processed << endl;
	std::cout << "data files processed with errors:  " << files_failed << endl;
	std::cout << "rows read:                         " << CParseSummary::input_rows_number << endl;
	std::cout << "rows written:                      " << CParseSummary::output_rows_number << endl;

	output_mutex.unlock();
}