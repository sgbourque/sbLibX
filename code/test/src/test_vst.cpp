#include <common/include/sb_library.h>
#include <common/include/sb_interface_vst.h>


#include <string>
#include <iostream>
#include <iomanip>

	template <typename _CHAR_TYPE_>
inline std::basic_ostream<_CHAR_TYPE_>& operator << ( std::basic_ostream<_CHAR_TYPE_>& os, sbLibX::IUnknown::GUID::CLSID clsid )
{
	os << std::setw(8) << std::hex << clsid._0 << "-" << std::setw(4) << clsid._1 << "-" << clsid._2 << "-";
	for (size_t i = 0; i < sizeof( clsid._3 ); ++i)
		os << std::setw(2) << std::hex << static_cast<uint32_t>(clsid._3[i]);
	return os;
}


SB_EXPORT_TYPE int SB_STDCALL vst( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
	std::string moduleName;// e.g., "C:\\Program Files\\Steinberg\\VstPlugins\\dev\\Surge.vst3";
	if( argc > 1 )
	{
		const char* moduleNameBegin = argv[argc - 1];
		while( *moduleNameBegin == '"' )
			++moduleNameBegin;
		const char* moduleNameEnd = moduleNameBegin;
		while( *moduleNameEnd != '\0' && *moduleNameEnd != '"' )
			++moduleNameEnd;

		moduleName = std::basic_string_view<char>{ moduleNameBegin, size_t(moduleNameEnd - moduleNameBegin) };
	}

	// Listing exported names seems to be the easiest way to find the VST version...
	// There's quite a big difference between VST 2.3 and VST 2.4 and even more at VST 3.
	sbLibX::unique_dll lib( moduleName.c_str() );
	if( lib )
	{
		std::cout << "'" << moduleName << "':" << std::endl;
		for( auto it : lib )
			std::cout << "\t" << it.first << std::endl;

		namespace sbVST = sbLibX::VST;
		using namespace sbVST;
		sbVST::GetPluginFactory_t GetFactory = lib[std::string_view("GetPluginFactory")];
		IPluginFactory* factory = GetFactory ? GetFactory() : nullptr;
		if( factory )
		{
			//factory = sbLibX::query_interface<IPluginFactory>( factory );
			FactoryInfo factoryInfo{};
			factory->getFactoryInfo( &factoryInfo );

			const int32_t vstClassCount = factory->countClasses();
			for (int32_t index = 0; index < vstClassCount; ++index)
			{
				ClassInfo classInfo{};
				factory->getClassInfo( index, &classInfo );
				std::cout
					<< "name: " << classInfo.name << "\n"
					<< "cat:  " << classInfo.category << "\n"
					<< "id:   " << classInfo.cid << "\n"
					<< "card: " << classInfo.cardinality << "\n";

				sbLibX::IUnknown* obj{};
				auto result = factory->createInstance( ( const char* )reinterpret_cast<sbLibX::IUnknown::GUID&>( classInfo.cid ).uid, (const char*)IPluginBase::iid.uid, reinterpret_cast<void**>( &obj ) );
				if( result >= 0 )
				{
					IPluginBase* plugin = sbLibX::query_interface<IPluginBase>( obj );
					plugin->Release();
				}
			}
		}
	}
	return 0;
}
