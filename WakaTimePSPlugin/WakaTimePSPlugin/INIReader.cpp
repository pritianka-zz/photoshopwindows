// Read an INI file into easy-to-access name/value pairs.

#include "stdafx.h"
#include <algorithm>
#include <fstream>
#include "INIReader.h"

std::vector<std::wstring> INIReader::Tokanize()
{
	// After a generic GetPrivateProfileString call, there are
	// tokens seperated by NULLs in the buffer. Tokanize them
	// and return a vector of wstrings.
	int end = 0;
	int begin = 0;
	std::vector<std::wstring> retunValue;
	while (end < m_CountOfCharsRead)
	{
		while (buffer[end++] != NULL); // run till the next NULL
		std::wstring temp(buffer + begin, buffer + end - 1);
		begin = end; // Reinitialize begin so as to point to next (w)string.
		retunValue.push_back(temp);
	}

	return retunValue;
}

INIReader::INIReader(std::wstring filename) : m_Filename(filename)
{
	m_CountOfCharsRead = GetPrivateProfileString(NULL, NULL, NULL, buffer,
						sizeof(buffer), filename.c_str());
	if (m_CountOfCharsRead == 0)
		return;
	// At this point buffer will contain all section names in the file. 
	// As per MSDN, this will be NULL terminated (w)strings.
	m_Sections = Tokanize();

	if (m_Sections.empty())
		return;
	
	std::vector<std::wstring>::iterator sectioniterator(m_Sections.begin());
	while (sectioniterator != m_Sections.end())
	{
		m_CountOfCharsRead = GetPrivateProfileString(sectioniterator->c_str(), 
			NULL, NULL, buffer,	sizeof(buffer), filename.c_str());
		// Don't know why reading this section has failed, but continue
		// reading other sections. They might contain data which are readable.
		if (m_CountOfCharsRead == 0)
		{
			// And don't forget to keep an empty map under this secton. This is
			// important as we will be writing back the ini file.
			m_KVPairsUnderASection[sectioniterator->c_str()] = 
											std::map<std::wstring, std::wstring>();
			sectioniterator++;
			continue;
		}

		std::vector<std::wstring> keys = Tokanize();
		// These are the keys in this section. Get their values. We are almost
		// done with reading the ini file.
		std::map<std::wstring, std::wstring> tempMap;
		std::vector<std::wstring>::iterator keyitertor(keys.begin());
		while (keyitertor != keys.end())
		{
			m_CountOfCharsRead = GetPrivateProfileString(sectioniterator->c_str(), 
				keyitertor->c_str(), NULL, buffer, sizeof(buffer), filename.c_str());
			if (m_CountOfCharsRead == 0)
			{
				// Don't lose the key.
				tempMap[keyitertor->c_str()] = std::wstring();
				keyitertor++;
				continue;
			}

			tempMap[keyitertor->c_str()] = std::wstring(buffer);
			keyitertor++;
		}

		m_KVPairsUnderASection[sectioniterator->c_str()] = tempMap;
		sectioniterator++; // Go to the next section.
	}
}

std::wstring INIReader::GetSection(int position)
{
	return m_Sections[position];
}

int INIReader::GetCountOfSections()
{
	return static_cast<int>(m_Sections.size());
}

std::map<std::wstring, std::wstring> INIReader::GetKeyValuePairsUnderASection(std::wstring section)
{
	return m_KVPairsUnderASection.at(section);
}

void INIReader::WriteIniFile(std::wstring filename)
{
	std::wfstream inifile(filename.empty() ? m_Filename : filename, std::ios_base::out);
	int CountOfSections = GetCountOfSections();
	for (int i = 0; i < CountOfSections; ++i)
	{
		std::wstring section = GetSection(i);
		std::wstring towrite = L"[" + section + L"]\n";
		inifile.write(towrite.c_str(), towrite.size());
		std::map<std::wstring, std::wstring> KVPairs = GetKeyValuePairsUnderASection(section);
		std::map<std::wstring, std::wstring>::iterator it(KVPairs.begin());
		while (it != KVPairs.end())
		{
			towrite = it->first + L" = " + it->second + L"\n";
			inifile.write(towrite.c_str(), towrite.size());
			it++;
		}
	}

	inifile.flush();
	inifile.close();
}

void INIReader::ChangeValue(std::wstring section, std::wstring key, std::wstring value)
{
	std::map<std::wstring, std::wstring> KVPairs = m_KVPairsUnderASection.at(section);
	if (KVPairs.empty())
		return;

	KVPairs[key] = value;
	m_KVPairsUnderASection[section] = KVPairs;
}

void INIReader::CreateSectionAndAddKeyValue(std::wstring section, std::wstring key, std::wstring value)
{
	std::map<std::wstring, std::wstring> temp;
	temp.insert(std::make_pair(key, value));
	m_KVPairsUnderASection[section] = temp;
}


