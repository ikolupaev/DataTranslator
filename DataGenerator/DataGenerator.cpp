#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{
	if (argc < 4)
	{
		std::cout << "usage:" << std::endl;
		std::cout << "\ttest.exe <path> <columns_number> <rows_number>" << std::endl;
	
		return 1;
	}

	std::ofstream of(argv[1]);

	size_t columns_number = atoi(argv[2]);
	size_t rows_number = atoi(argv[3]);

	of << "COL0";

	for (size_t i = 1; i < columns_number; i++)
	{
		of << '\t' << "COL" << i;
	}

	of << std::endl;

	for (size_t i = 0; i < rows_number; i++)
	{
		if (i % 100 == 0)
		{
			std::cout << i << std::endl;
		}

		of << "ID3";

		for (size_t i = 1; i < columns_number; i++)
		{
			of << '\t' << "V" << rand() * 100;
		}

		of << std::endl;
	}

	of.close();

	return 0;
}
