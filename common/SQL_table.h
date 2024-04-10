#pragma once
#include <iostream>
#include <vector>

namespace common
{
	class SQL_table
	{
		int columns = 0;
		char** col_names;
		std::vector<char**> records;

	private:
		static void* safeSetMemory(int size)
		{
			void* alloc = (char**)malloc(size);
			if (!alloc)
			{
				std::cout << "Malloc error\n";
				exit(-1);
			}
			return alloc;
		}

		// Serialization
		int tableSizeInBytes()
		{
			int size = 0;

			// Header
			for (int c = 0; c < columns; c++)
				if (col_names[c])
					size += strlen(col_names[c]) + 1;

			// Records
			for (int r = 0; r < records.size(); r++)
				for (int c = 0; c < columns; c++)
					if (records[r][c])
						size += strlen(records[r][c]) + 1;

			return size;
		}
		void* setData(int size)
		{
			void* data = safeSetMemory(size);
			void* index = data;

			int rows = records.size();

			memcpy(index, &columns, sizeof(int)); index = (char*)index + sizeof(int);
			memcpy(index, &rows,	sizeof(int)); index = (char*)index + sizeof(int);

			// Header
			for (int c = 0; c < columns; c++)
				setString(&index, col_names[c]);

			// Records
			for (int r = 0; r < records.size(); r++)
				for (int c = 0; c < columns; c++)
					setString(&index, records[r][c]);

			return data;
		}
		void setString(void** data, char* str)
		{
			char* ptr = (char*)*data;
			int str_size = 1;
			*ptr = '\0';

			if (str)
			{
				str_size += strlen(str);
				memcpy(ptr, str, str_size);
			}
			*data = (char*)ptr + str_size;
		}

		static char** getStringArray(char** from, int size)
		{
			char** array = (char**)safeSetMemory(size * sizeof(char*));
			int offset = 0;

			for (int i = 0; i < size; i++)
			{
				array[i] = *from;
				*from += strlen(*from) + 1;
			}

			return array;
		}

		// Printing
		void verticalLine(int* widths, char sep = '-', char con = '+')
		{
			for (int i = 0; i < columns; i++)
			{
				std::cout << con << sep << sep;
				for (int j = 0; j < widths[i]; j++)
					std::cout << sep;
			}
			std::cout << con << "\n";
		}

	public:
		SQL_table() {}

		void SetColNames(char** names, int cols)
		{
			if (col_names)
				return;

			columns = cols;
			col_names = (char**)safeSetMemory(columns * sizeof(char*));

			for (int i = 0; i < cols; i++)
			{
				int len = strlen(names[i]) + 1;
				col_names[i] = (char*)safeSetMemory(len * sizeof(char));
				memcpy(col_names[i], names[i], len);
			}
		}

		void addRow(char** values, int cols) 
		{
			if (columns != cols)
				return;

			char** row = (char**)safeSetMemory(cols * sizeof(char*));
			for (int i = 0; i < cols; i++)
			{
				if (!values[i])
				{
					row[i] = nullptr;
					continue;
				}

				int len = strlen(values[i]) + 1;
				row[i] = (char*)safeSetMemory(len * sizeof(char));
				memcpy(row[i], values[i], len);
			}
			records.push_back(row);
		}

		int getResultsNum()
		{
			return records.size();
		}

		char* getValue(int record, int column)
		{
			return records[record][column];
		}

		int Serialize(void** data)
		{
			int size = 2 * sizeof(int);

			if (columns == 0)
				return 0;

			size += tableSizeInBytes();

			int index = 0;

			*data = setData(size);
			return size;
		}

		static SQL_table Deserialize(void* data, int size)
		{
			SQL_table table = SQL_table();

			if (size == 0)
				return table;

			char* index = (char*)data;

			int columns, rows;
			memcpy(&columns,	index, sizeof(int)); index += sizeof(int);
			memcpy(&rows,		index, sizeof(int)); index += sizeof(int);

			table.SetColNames(getStringArray(&index, columns), columns);

			for (int r = 0; r < rows; r++)
				table.addRow(getStringArray(&index, columns), columns);

			return table;
		}

		void print()
		{
			if (records.size() == 0)
			{
				std::cout << "No results\n";
				return;
			}

			int* widths = (int*)safeSetMemory(columns * sizeof(int));

			for (int i = 0; i < columns; i++)
			{
				widths[i] = strlen(col_names[i]);
				for (char** row : records)
					if(row[i])
						widths[i] = std::max<int>(widths[i], strlen(row[i]));
			}

			verticalLine(widths);

			// Header
			for (int i = 0; i < columns; i++)
			{
				std::cout << "| " << col_names[i] << ' ';
				for (int j = strlen(col_names[i]); j < widths[i]; j++)
					std::cout << ' ';
			}

			std::cout << "|\n";

			verticalLine(widths);
			verticalLine(widths);

			//Records
			for (int r = 0; r < records.size(); r++)
			{
				for (int c = 0; c < columns; c++)
				{
					if (records[r][c])
					{
						std::cout << "| " << records[r][c] << ' ';
						for (int i = strlen(records[r][c]); i < widths[c]; i++)
							std::cout << ' ';
					}
				}
				std::cout << "|\n";
				verticalLine(widths);
			}

		}
	};
} // common