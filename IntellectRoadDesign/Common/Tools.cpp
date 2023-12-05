#include "stdafx.h"
#include "Tools.h"

namespace BIM {

	bool Tools::whetherValidFolder(std::string const& folderName)
	{
		if (!PathIsDirectoryA(folderName.c_str()))
			return false;
		return true;
	}

	bool Tools::whetherValidLocalFile(const char* fileName)
	{
		bool bReturn = false;
#if defined(_WINDOWS_)
		if (PathFileExistsA(fileName))
		{
			// MessageBox(NULL, _T("whetherValidLocalFile - true"), _T("test"), MB_OK);
			bReturn = true;
		}
#elif defined(__linux__)
#elif defined(__APPLE__)
#endif
		return bReturn;
	}

	bool Tools::whetherValidFolder(std::wstring const& folderName)
	{
		if (!PathIsDirectoryW(folderName.c_str()))
			return false;
		return true;
	}

	bool Tools::whetherValidLocalFile(const wchar_t* fileName)
	{
		bool bReturn = false;
		if (PathFileExistsW(fileName))
		{
			// MessageBox(NULL, _T("whetherValidLocalFile - true"), _T("test"), MB_OK);
			bReturn = true;
		}
		return bReturn;
	}


	void Tools::lowerString(std::string& str)
	{
#ifdef _MSC_VER
#if _MSC_VER >= 1800
		std::for_each(str.begin(), str.end(), [=](char &ochar)
		{
			ochar = (char)tolower(ochar);
		});
#else
		for (size_t nIndex = 0; nIndex != str.size(); ++nIndex)
		{
			str[nIndex] = (char)tolower(str[nIndex]);
		}
#endif
#endif
	}

	void Tools::lowerString(char* str)
	{
		size_t nLength = strlen(str);
		for (size_t nIndex = 0; nIndex != nLength; ++nIndex)
		{
			str[nIndex] = (char)tolower(str[nIndex]);
		} 
	}

	void Tools::lowerWstring(std::wstring& wstr)
	{
#ifdef _MSC_VER
#if _MSC_VER >= 1800
		std::for_each(wstr.begin(), wstr.end(), [=](wchar_t &ochar)
		{
			ochar = (wchar_t)towlower(ochar);
		});
#else
		for (size_t nIndex = 0; nIndex != wstr.size(); ++nIndex)
		{
			wstr[nIndex] = (wchar_t)towlower(wstr[nIndex]);
		}
#endif
#endif
	}

	void Tools::upperString(std::string& str)
	{
#ifdef _MSC_VER
#if _MSC_VER >= 1800
		std::for_each(str.begin(), str.end(), [=](char &ochar)
		{
			ochar = (char)toupper(ochar);
		});
#else
		for (size_t nIndex = 0; nIndex != str.size(); ++nIndex)
		{
			str[nIndex] = (char)toupper(str[nIndex]);
		}
#endif
#endif

	}

	void Tools::upperString(char* str)
	{

		size_t nLength = strlen(str);
		for (size_t nIndex = 0; nIndex != nLength; ++nIndex)
		{
			str[nIndex] = (char)toupper(str[nIndex]);
		} 
	}

	void Tools::upperWstring(std::wstring& wstr)
	{
#ifdef _MSC_VER
#if _MSC_VER >= 1800
		std::for_each(wstr.begin(), wstr.end(), [=](wchar_t &ochar)
		{
			ochar = (wchar_t)toupper(ochar);
		});
#else
		for (size_t nIndex = 0; nIndex != wstr.size(); ++nIndex)
		{
			wstr[nIndex] = (wchar_t)toupper(wstr[nIndex]);
		}
#endif
#endif
	}


	std::wstring Tools::toWstring(const std::string& str)
	{
		int nLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), NULL, 0);
		if (nLen <= 0) return std::wstring(L"");
		wchar_t* pBuffer = new wchar_t[nLen + 1];
		pBuffer[nLen] = L'\0';
		int nRtn = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), pBuffer, nLen);
		if (nRtn != nLen)
		{
			delete pBuffer;
			pBuffer = 0;
			return false;
		}
		std::wstring strReturn(pBuffer);
		delete[] pBuffer;
		pBuffer = 0;
		return strReturn;
	}

	std::wstring Tools::toWstring(const char* str)
	{
		int nLen = MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str), NULL, 0);
		if (nLen <= 0) return std::wstring(L"");
		wchar_t* pBuffer = new wchar_t[nLen + 1];
		pBuffer[nLen] = '\0';
		int nRtn = MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str), pBuffer, nLen);
		if (nRtn != nLen)
		{
			delete pBuffer;
			pBuffer = 0;
			return false;
		}
		std::wstring strReturn(pBuffer);
		delete[] pBuffer;
		pBuffer = 0;
		return strReturn;
	}

	std::wstring Tools::NtoWstring(int  value)
	{
		std::wostringstream oss;
		oss<<value;
		std::wstring wstr(oss.str());
		return wstr;
	}

  	std::wstring Tools::NtoWstring(double value)
  	{
  		std::wostringstream oss;
  		oss<<value;
  		std::wstring wstr(oss.str());
  		return wstr;
  	}



	std::wstring Tools::utf8ToWstring(const std::string& str)
	{
		int nLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
		if (nLen <= 0) return std::wstring(L"");
		wchar_t* pBuffer = new wchar_t[nLen + 1];
		pBuffer[nLen] = '\0';
		int nRtn = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), pBuffer, nLen);
		if (nRtn != nLen)
		{
			delete pBuffer;
			pBuffer = 0;
			return false;
		}
		std::wstring strReturn(pBuffer);
		delete[] pBuffer;
		pBuffer = 0;
		return strReturn;
	}

	std::wstring Tools::utf8ToWstring(const char* str)
	{
		int nLen = MultiByteToWideChar(CP_UTF8, 0, str, (int)strlen(str), NULL, 0);
		if (nLen <= 0) return std::wstring(L"");
		wchar_t* pBuffer = new wchar_t[nLen + 1];
		pBuffer[nLen] = '\0';
		int nRtn = MultiByteToWideChar(CP_UTF8, 0, str, (int)strlen(str), pBuffer, nLen);
		if (nRtn != nLen)
		{
			delete pBuffer;
			pBuffer = 0;
			return false;
		}
		std::wstring strReturn(pBuffer);
		delete[] pBuffer;
		pBuffer = 0;
		return strReturn;
	}

	std::string Tools::toString(const std::wstring& wstr)
	{
		int nLen = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
		char* str = new char[nLen + 1];
		::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), str, nLen, NULL, NULL);
		str[nLen] = '\0';
		std::string sDest(str);
		delete[] str;
		str = NULL;
		return sDest;
	}

	std::string Tools::toString(const wchar_t* wstr)
	{
		int nLen = ::WideCharToMultiByte(CP_ACP, 0, wstr, (int)wcslen(wstr), NULL, 0, NULL, NULL);
		char* str = new char[nLen + 1];
		::WideCharToMultiByte(CP_ACP, 0, wstr, (int)wcslen(wstr), str, nLen, NULL, NULL);
		str[nLen] = '\0';
		std::string sDest(str);
		delete[] str;
		str = NULL;
		return sDest;
	}

	std::string Tools::utf8ToString(const std::string& str)
	{
		return toString(utf8ToWstring(str));
	}

	std::string Tools::utf8ToString(const char* str)
	{
		if (str == NULL)
			return "";
		return toString(utf8ToWstring(str));
	}

	std::string Tools::toUtf8(const std::string& str)
	{
		return toUtf8(toWstring(str));
	}

	std::string Tools::toUtf8(const char* str)
	{
		if (str == NULL)
			return "";
		return toUtf8(toWstring(str));
	}

	std::string Tools::toUtf8(const std::wstring& wstr)
	{
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
		char* str = new char[nLen + 1];
		::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), str, nLen, NULL, NULL);
		str[nLen] = '\0';
		std::string strOut(str);
		delete[] str;
		str = NULL;
		return strOut;
	}

	std::string Tools::toUtf8(const wchar_t* wstr)
	{
		if (wstr == NULL)
			return "";
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, wstr, (int)wcslen(wstr), NULL, 0, NULL, NULL);
		char* str = new char[nLen + 1];
		::WideCharToMultiByte(CP_UTF8, 0, wstr, (int)wcslen(wstr), str, nLen, NULL, NULL);
		str[nLen] = '\0';
		std::string strOut(str);
		delete[] str;
		str = NULL;
		return strOut;
	}

	std::string Tools::parseString(std::string& strFrom, std::string const& strFormat)
	{
		std::string strReturn("");
		std::string::size_type find = strFrom.find(strFormat);
		if (strFrom.npos != find)
		{
			if (0 == find)
			{
				std::string::size_type second = strFrom.find(strFormat, find + 1);
				if (strFrom.npos == second)
				{
					strReturn = strFrom.substr(find + 1, strFrom.size() - find - 1);
					strFrom.clear();
				}
				else
				{
					strReturn = strFrom.substr(find + 1, second - find - 1);
					strFrom = strFrom.substr(second + 1, strFrom.size() - second - 1);
				}
			}
			else
			{
				strReturn = strFrom.substr(0, find);
				strFrom = strFrom.substr(find + 1, strFrom.size() - find - 1);
			}
		}
		else
		{
			if (0 != strFrom.size())
			{
				strReturn = strFrom;
				strFrom.clear();
			}
		}
		return strReturn;
	}

	std::wstring Tools::parseWstring(std::wstring& wstrFrom, std::wstring const& wstrFormat)
	{
		std::wstring wstrReturn(L"");
		std::wstring::size_type find = wstrFrom.find(wstrFormat);
		if (wstrFrom.npos != find)
		{
			if (0 == find)
			{
				std::wstring::size_type second = wstrFrom.find(wstrFormat, find + 1);
				if (wstrFrom.npos == second)
				{
					wstrReturn = wstrFrom.substr(find + 1, wstrFrom.size() - find - 1);
					wstrFrom.clear();
				}
				else
				{
					wstrReturn = wstrFrom.substr(find + 1, second - find - 1);
					wstrFrom = wstrFrom.substr(second + 1, wstrFrom.size() - second - 1);
				}
			}
			else
			{
				wstrReturn = wstrFrom.substr(0, find);
				wstrFrom = wstrFrom.substr(find + 1, wstrFrom.size() - find - 1);
			}
		}
		else
		{
			if (0 != wstrFrom.size())
			{
				wstrReturn = wstrFrom;
				wstrFrom.clear();
			}
		}
		return wstrReturn;
	}

	bool Tools::parseMatrix(std::wstring const&wstrMatrix, float fmatrix[16])
	{
		bool bReturn = true;
		try
		{
			swscanf_s(wstrMatrix.c_str(), L"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
				&fmatrix[0], &fmatrix[1], &fmatrix[2], &fmatrix[3],
				&fmatrix[4], &fmatrix[5], &fmatrix[6], &fmatrix[7],
				&fmatrix[8], &fmatrix[9], &fmatrix[10], &fmatrix[11],
				&fmatrix[12], &fmatrix[13], &fmatrix[14], &fmatrix[15]);
		}
		catch (const std::exception& e)
		{
			e.what();
			bReturn = false;
		}
		catch (...)
		{
			bReturn = false;
		}
		return bReturn;
	}

	std::string Tools::getGUIDString()
	{
		std::string strGUID("");
		GUID guid;
		if (S_OK == CoCreateGuid(&guid))
		{
			char cGUID[128]; cGUID[0] = '\0';
			sprintf_s(cGUID, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1],
				guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5],
				guid.Data4[6], guid.Data4[7]);
			strGUID = cGUID;
		}
		return strGUID;
	}

	std::string Tools::getLowGUIDString()
	{
		std::string strGUID("");
		GUID guid;
		if (S_OK == CoCreateGuid(&guid))
		{
			char cGUID[128]; cGUID[0] = '\0';
			sprintf_s(cGUID, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1],
				guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5],
				guid.Data4[6], guid.Data4[7]);
			strGUID = cGUID;
		}
		return strGUID;
	}

	std::string Tools::getLowNoneGUIDString()
	{
		std::string strGUID("");
		GUID guid;
		if (S_OK == CoCreateGuid(&guid))
		{
			char cGUID[128]; cGUID[0] = '\0';
			sprintf_s(cGUID, "%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x", guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1],
				guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5],
				guid.Data4[6], guid.Data4[7]);
			strGUID = cGUID;
		}
		return strGUID;
	}

	std::wstring Tools::getGUIDWString()
	{
		std::wstring wstrGUID(L"");
		GUID guid;
		if (S_OK == CoCreateGuid(&guid))
		{
			wchar_t wcGUID[128]; wcGUID[0] = L'\0';
			swprintf_s(wcGUID, L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1],
				guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5],
				guid.Data4[6], guid.Data4[7]);
			wstrGUID = wcGUID;
		}
		return wstrGUID;
	}

	std::wstring Tools::getLowGUIDWString()
	{
		std::wstring wstrGUID(L"");
		GUID guid;
		if (S_OK == CoCreateGuid(&guid))
		{
			wchar_t wcGUID[128]; wcGUID[0] = L'\0';
			swprintf_s(wcGUID, L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1],
				guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5],
				guid.Data4[6], guid.Data4[7]);
			wstrGUID = wcGUID;
		}
		return wstrGUID;
	}

	std::wstring Tools::getLowNoneGUIDWString()
	{
		std::wstring wstrGUID(L"");
		GUID guid;
		if (S_OK == CoCreateGuid(&guid))
		{
			wchar_t wcGUID[128]; wcGUID[0] = L'\0';
			swprintf_s(wcGUID, L"%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x", guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1],
				guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5],
				guid.Data4[6], guid.Data4[7]);
			wstrGUID = wcGUID;
		}
		return wstrGUID;
	}

	void Tools::editFileExtension(std::string oldFile, std::string& newFile, std::string const& extension)
	{
		std::string::size_type findIndex = oldFile.find_last_of('.');
		if (findIndex == oldFile.npos)
		{
			newFile = "";
			return;
		}
		oldFile.erase(findIndex);
		newFile = oldFile;
		newFile.append(extension);
	}

	void Tools::editFileExtension(std::wstring oldFile, std::wstring& newFile, std::wstring const& extension)
	{
		std::wstring::size_type findIndex = oldFile.find_last_of('.');
		if (findIndex == oldFile.npos)
		{
			newFile = L"";
			return;
		}
		oldFile.erase(findIndex);
		newFile = oldFile;
		newFile.append(extension);
	}

	void Tools::editFileExtensionAndPutTempPath(std::wstring oldFile, std::wstring& newFile, std::wstring const& extension)
	{
		WCHAR   szTempPath[MAX_PATH];
		GetTempPathW(MAX_PATH, szTempPath);
		wcscat_s(szTempPath, L"bimu\\");
		//! 路径不存在
		if (!PathIsDirectoryW(szTempPath))
		{
			if (!CreateDirectory(szTempPath, NULL))
				return;
		}
		newFile = szTempPath;
		std::wstring::size_type findIndex = oldFile.find_last_of(L'/');
		if (findIndex == oldFile.npos)
			findIndex = oldFile.find_last_of(L'\\');
		if (findIndex == oldFile.npos)
			return;
		std::wstring fileName = oldFile.substr(findIndex + 1, oldFile.size() - findIndex - 1);
		findIndex = fileName.find_last_of(L'.');
		if (findIndex == fileName.npos)
			return;
		fileName.erase(findIndex, fileName.size() - findIndex);
		fileName.append(extension);
		newFile.append(fileName);
	}

	void Tools::eraseFileExtension(wchar_t* filepath)
	{
		size_t length = wcslen(filepath);
		if (0 == length)
			return;
		size_t sFind = 0;
		for (size_t index = length - 1; index > 0; --index)
		{
			wchar_t const * cur = filepath + index;
			if (*cur == '.')
			{
				sFind = index;
				break;
			}
		}
		if (sFind != 0)
		{
			for (size_t index = length - 1; index >= sFind; --index)
			{
				wchar_t * pcur = filepath + index;
				*pcur = '\0';
			}
		}
	}

	void Tools::eraseFileExtension(std::wstring& filepath)
	{
		size_t length = wcslen(filepath.c_str());
		if (0 == length)
			return;
		
		size_t iPos = filepath.find_last_of('.');
		if (iPos != std::wstring::npos) {
			filepath = filepath.substr(0, iPos);
		}
	}
	//! 格式化整数到0-255
	void Tools::formatInt32(long& value)
	{
		if (value < 0)
			value = 0;
		else if (value > 255)
			value = 255;
	}

	std::string Tools::getFileExtention(std::string const& target)
	{
		std::string::size_type findIndex = target.find_last_of('.');
		if (findIndex == target.npos)
		{
			return "";
		}
		std::string str = target.substr(findIndex, target.size() - findIndex);
		return str;
	}

	std::wstring Tools::getFileExtention(std::wstring const& target)
	{
		std::wstring::size_type findIndex = target.find_last_of('.');
		if (findIndex == target.npos)
		{
			return L"";
		}
		std::wstring str = target.substr(findIndex);
		return str;
	}

	std::vector<std::wstring> Tools::SplitString(std::wstring value,std::wstring splitChar)
	{
		size_t length = splitChar.length();
		std::wstring tempValue(value);
		std::vector<std::wstring> vec_value;
		size_t ipos = tempValue.find_first_of(splitChar);
		while (ipos != std::wstring::npos)
		{
			std::wstring val = tempValue.substr(0, ipos);
			vec_value.push_back(val);
			tempValue = tempValue.substr(ipos + length);
			ipos = tempValue.find_first_of(splitChar);
		}
		vec_value.push_back(tempValue);
		return vec_value;
	}

	std::vector<int> Tools::SplitStringToInt(std::wstring value, std::wstring splitChar)
	{
		size_t length = splitChar.length();
		std::wstring tempValue(value); 
// 		int tempValue = _wtoi(value.c_str());
		std::vector<int> vec_value;
		size_t ipos = tempValue.find_first_of(splitChar);
		while (ipos != std::wstring::npos)
		{
			std::wstring val = tempValue.substr(0, ipos);
			int iVal = _wtoi(val.c_str());
			vec_value.push_back(iVal);
			tempValue = tempValue.substr(ipos + length);
			ipos = tempValue.find_first_of(splitChar);
		}
		int lastValue = _wtoi(tempValue.c_str());
		vec_value.push_back(lastValue);
		return vec_value;
	}



	std::string Tools::IntToString(int n)
	{
		std::string result;
		std::strstream ss;
		ss <<  n;
		ss >> result;
		return result;
	}

	std::string Tools::lltoString(long long t)
	{
		std::string result;
		std::strstream ss;
		ss <<  t;
		ss >> result;
		return result;
	}
	std::wstring Tools::IntToWstring(int i)
	{
		std::wstringstream ss;
		ss << i;
		return ss.str();
	}

	bool Tools::IsNum(std::wstring const& wstr)
	{
		for (int index = 0; index < wstr.size(); ++index)
		{
			int tmp = (int)wstr[index];
			if (tmp >= 48 && tmp <= 57)
			{
				continue;
			}
			else
			{
				return false;
			}
		} 
		return true;
	}
	
	bool Tools::IsNum(std::string const& str)
	{
		for (int index = 0; index < str.size(); ++index)
		{
			int tmp = (int)str[index];
			if (tmp >= 48 && tmp <= 57)
			{
				continue;
			}
			else
			{
				return false;
			}
		} 
		return true;
	}

	bool Tools::IsCapital(std::wstring const& wstr)
	{
		for (int index = 0; index < wstr.size(); ++index)
		{
			int tmp = (int)wstr[index];
			if (tmp >= 65 && tmp <= 90)
			{
				continue;
			}
			else
			{
				return false;
			}
		} 
		return true;
	}

	bool Tools::IsCapital(std::string const& str)
	{
		for (int index = 0; index < str.size(); ++index)
		{
			int tmp = (int)str[index];
			if (tmp >= 65 && tmp <= 90)
			{
				continue;
			}
			else
			{
				return false;
			}
		} 
		return true;

	}



	void Tools::getFiles( std::string path, std::vector<std::wstring>& files 
		,std::wstring const& fileName ,bool bCotains )  
	{  
		//文件句柄  
		long   hFile   =   0;  
		//文件信息  
		struct _finddata_t fileinfo;  
		std::string p = path;
		p = p.append("\\*");
		if((hFile = _findfirst(p.c_str(),&fileinfo)) !=  -1)  
		{  
			do  
			{  
				std::string vvv = fileinfo.name;
				std::string _subPath = path + "\\" + vvv;
				//如果是目录,迭代之  
				if((fileinfo.attrib &  _A_SUBDIR))  
				{  
					if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
						getFiles(_subPath , files,fileName ,bCotains);  
				}  
				else  
				{  
					std::wstring wstrfileName = toWstring(vvv);
					std::string strFIleName = path + "\\" + fileinfo.name;
					std::wstring wstrfilePathName = toWstring(strFIleName);
					if(!fileName.empty())
					{
						if(bCotains)
						{
							if(wstrfileName.find(fileName) != std::wstring::npos)
							{
								files.push_back(wstrfilePathName );  
							}
						}
						else
						{
							if(wstrfileName == fileName)
								files.push_back(wstrfilePathName );  
						}
					}
					else{
							files.push_back(wstrfilePathName);  
					}
				}  
			}while(_findnext(hFile, &fileinfo)  == 0);  
			_findclose(hFile);  
		}  
	}

	bool Tools::CompareWstring(std::wstring const& wstr1,std::wstring const& wstr2)
	{

		if( (IsCapital(wstr1) && IsCapital(wstr2)) || (IsNum(wstr1) && IsNum(wstr2)))
		{
			int length1 = wstr1.length();
			int length2 = wstr2.length();
			if(length1 > length2)
			{
				return true;
			}
			else if(length1 == length2){
				for (int index = 0; index < length1; ++index)
				{
					int tmp1 = (int)wstr1[index];
					int tmp2 = (int)wstr2[index];
					if(tmp1 < tmp2)
					{
						return false;
					}
				} 
				return true;
			}
		}
		else if(IsCapital(wstr1)  && IsNum(wstr2))
		{
			return true;
		}
		return false;
	}
}