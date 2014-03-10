#pragma once

#include <atomic>

class CParseSummary
{
public:
	CParseSummary();
	virtual ~CParseSummary();

	static std::atomic<int> input_rows_number;
	static std::atomic<int> output_rows_number;
};
