#ifndef __WakaTime_Plugin_Ini_Reader
#define __WakaTime_Plugin_Ini_Reader

#include <map>
#include <string>
#include <vector>

// A class to read/modify/write INI files using windows APIs.
// Actually only one windows API - GetPrivateProfileString - is used.
class INIReader
{
public:
	enum {MAX_LEN = 5096};
    INIReader(std::wstring filename);

	inline int GetCountOfSections();
	inline std::wstring GetSection(int position);
	inline std::map<std::wstring, std::wstring> 
		GetKeyValuePairsUnderASection(std::wstring section);

	void WriteIniFile(std::wstring filename = L"");
	void ChangeValue(std::wstring section, std::wstring key, std::wstring value);
	void CreateSectionAndAddKeyValue(std::wstring section, std::wstring key, std::wstring value);

private:
	std::vector<std::wstring> Tokanize();
	std::wstring m_Filename;
	int m_CountOfCharsRead;
	std::map<std::wstring, std::map<std::wstring, std::wstring>> m_KVPairsUnderASection;
	TCHAR buffer[MAX_LEN];
	std::vector<std::wstring> m_Sections;
};

#endif  
