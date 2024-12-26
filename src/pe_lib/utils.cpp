#include <string.h>
#include "utils.h"
#include "pe_exception.h"

namespace pe_bliss
{
const double pe_utils::log_2 = 1.44269504088896340736; //instead of using M_LOG2E

//Returns stream size
std::streamoff pe_utils::get_file_size(std::istream& file)
{
	//Get old istream offset
	std::streamoff old_offset = file.tellg();
	file.seekg(0, std::ios::end);
	std::streamoff filesize = file.tellg();
	//Set old istream offset
	file.seekg(old_offset);
	return filesize;
}

bool operator==(const pe_win::guid& guid1, const pe_win::guid& guid2)
{
	return guid1.Data1 == guid2.Data1
		&& guid1.Data2 == guid2.Data2
		&& guid1.Data3 == guid2.Data3
		&& !memcmp(guid1.Data4, guid2.Data4, sizeof(guid1.Data4));
}
}
