#pragma once
#include <exception>
#include <stdexcept>

namespace pe_bliss
{
//PE exception class
class pe_exception : public std::runtime_error
{
public:
	//Exception IDs
	enum exception_id
	{
		unknown_error,
		bad_pe_file,
		bad_dos_header,
		image_nt_headers_not_found,
		error_reading_image_nt_headers,
		error_reading_data_directories,
		error_reading_file,
		pe_signature_incorrect,
		incorrect_number_of_rva_and_sizes,
		error_changing_section_virtual_size,
		section_number_incorrect,
		incorrect_section_alignment,
		incorrect_file_alignment,
		incorrect_size_of_image,
		incorrect_size_of_headers,
		image_section_headers_not_found,
		zero_section_sizes,
		section_incorrect_addr_or_size,
		section_not_found,
		image_section_data_not_found,
		no_section_found,
		image_section_table_incorrect,
		directory_does_not_exist,
		rva_not_exists,
		error_reading_section_header,
		error_reading_overlay,
		incorrect_address_conversion,
		section_is_empty,
		no_more_sections_can_be_added,
		error_expanding_section
	};

public:
	//Class constructors
	explicit pe_exception(const char* text, exception_id id = unknown_error);
	explicit pe_exception(const std::string& text, exception_id id = unknown_error);

	//Returns exception ID from exception_id enumeration
	exception_id get_id() const;

	//Destructor
	virtual ~pe_exception() throw()
	{}

private:
	exception_id id_;
};
}
