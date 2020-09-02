#include "pch.h"
#include <istream>
#include <string>
#include <vector>

enum class CSVState {
	UnquotedField,
	QuotedField,
	QuotedQuote
};

std::vector<std::wstring> readCSVRow(const std::wstring& row) {
	CSVState state = CSVState::UnquotedField;
	std::vector<std::wstring> fields{ L"" };
	size_t i = 0; // index of the current field
	for (char c : row) {
		switch (state) {
		case CSVState::UnquotedField:
			switch (c) {
			case ',': // end of field
				fields.push_back(L""); i++;
				break;
			case '"': state = CSVState::QuotedField;
				break;
			default:  fields[i].push_back(c);
				break;
			}
			break;
		case CSVState::QuotedField:
			switch (c) {
			case '"': state = CSVState::QuotedQuote;
				break;
			default:  fields[i].push_back(c);
				break;
			}
			break;
		case CSVState::QuotedQuote:
			switch (c) {
			case ',': // , after closing quote
				fields.push_back(L""); i++;
				state = CSVState::UnquotedField;
				break;
			case '"': // "" -> "
				fields[i].push_back('"');
				state = CSVState::QuotedField;
				break;
			default:  // end of quote
				state = CSVState::UnquotedField;
				break;
			}
			break;
		}
	}
	return fields;
}

/// Read CSV file, Excel dialect. Accept "quoted fields ""with quotes"""
std::vector<std::vector<std::wstring>> readCSV(std::wifstream& in) {
	std::vector<std::vector<std::wstring>> table;
	std::wstring row;
	while (!in.eof()) {
		std::getline(in, row);
		if (in.bad() || in.fail()) {
			break;
		}

		if (row.find(L"//") != 0)
		{
			auto fields = readCSVRow(row);
			table.push_back(fields);
		}
	}
	return table;
}

std::vector<std::vector<std::wstring>> readCSVFile(const wchar_t* filename)
{
	std::wifstream file_stream(filename);
	if (file_stream)
	{
		return readCSV(file_stream);
	}
	else
	{
		return std::vector<std::vector<std::wstring>>();
	}
}