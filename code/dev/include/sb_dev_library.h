#pragma once
#include <dev/include/sb_dev.h>

#include <string>

namespace SB { namespace LibX
{

class library
{
public:
	library() {}
	virtual ~library()
	{
		free();
	}

	bool load(const char* libName);
	bool free();

	std::string name() const { return library_name; }
	std::string alias() const { return library_alias; }

		template<typename data_type>
	data_type get_data() const { return reinterpret_cast<data_type>(data); };

		template<typename fct_type>
	fct_type get(const char* fctName) { return reinterpret_cast<fct_type>( get_internal(fctName) ); }

	operator bool () const { return data != 0; }

private:
	using raw_data_t = void*;
	raw_data_t get_internal(const char* fctName);

private:
	std::string library_name{};
	std::string library_alias{};
	raw_data_t data = nullptr;
};

}} // namespace SB::LibX
