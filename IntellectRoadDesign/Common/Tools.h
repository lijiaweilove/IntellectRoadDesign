#pragma once
#include <strstream>
#include <sstream>
#include <string>
#include <io.h>
namespace BIM
{
	template <typename T>
	std::wstring NumToWstring(const T& t)
	{
		std::wostringstream iss;
		iss << t;
		std::wstring wstrValue = iss.str();
		return wstrValue;
	}

	class Tools
	{
	public:

		static bool whetherValidFolder(std::string const& folderName);

		static bool whetherValidLocalFile(const char* fileName);

		static bool whetherValidFolder(std::wstring const& folderName);

		static bool whetherValidLocalFile(const wchar_t* fileName);
		/*!
		\brief	删除本地文件
		\param param1
		\return
		*/
		static bool deleteLocalFile(const char* fileName);
		/*!
		\brief	将所有字符串全转为小写
		\param str	 需要转换的字符串
		\return  无
		*/
		static void lowerString(std::string& str);
		static void lowerString(char* str);
		static void lowerWstring(std::wstring& wstr);

		static void upperString(std::string& str);
		static void upperString(char* str);
		static void upperWstring(std::wstring& wstr);
		/*!
		\brief	多字节转宽字节
		\param str	 需要转换的字符串
		\return  返回转换后的字符串
		*/
		static std::wstring toWstring(const std::string& str);
		static std::wstring toWstring(const char* str);
		
		static std::wstring	NtoWstring(int value);
		static std::wstring	NtoWstring(double value);
		/*!
		\brief	utf8字符串转宽字节
		\param str	 需要转换的字符串
		\return  返回转换后的字符串
		*/
		static std::wstring utf8ToWstring(const std::string& str);
		static std::wstring utf8ToWstring(const char* str);
		/*!
		\brief	宽字节转多字节
		\param str	 需要转换的字符串
		\return  返回转换后的字符串
		*/
		static std::string toString(const std::wstring& wstr);
		static std::string toString(const wchar_t* wstr);
		/*!
		\brief	utf8字符串转多字节
		\param str	 需要转换的字符串
		\return  返回转换后的字符串
		*/
		static std::string utf8ToString(const std::string& str);
		static std::string utf8ToString(const char* str);
		/*!
		\brief	转到utf8字符串
		\param str	 需要转换的的字符
		\return  返回utf8的字符
		*/
		static std::string toUtf8(const std::string& str);
		static std::string toUtf8(const char* str);
		static std::string toUtf8(const std::wstring& wstr);
		static std::string toUtf8(const wchar_t* wstr);
		/*!
		\brief	解析字符串
		\param strFrom	需要被解析字符串
		\param strFormat	格式
		\return  解析的单个字符串
		*/
		static std::string parseString(std::string& strFrom, std::string const& strFormat);
		static std::wstring parseWstring(std::wstring& wstrFrom, std::wstring const& wstrFormat);
		static bool parseMatrix(std::wstring const& wstrMatrix, float fmatrix[16]);
		/*!
		\brief	获取guid字符串
		\return  返回guid字符串
		*/
		static std::string getGUIDString();
		static std::string getLowGUIDString();
		static std::string getLowNoneGUIDString();
		static std::wstring getGUIDWString();
		static std::wstring getLowGUIDWString();
		static std::wstring getLowNoneGUIDWString();
		/*!
		\brief	修改文件后缀
		\param oldFile
		\param newFile
		\return  无
		*/
		static void editFileExtension(std::string oldFile, std::string& newFile, std::string const& extension);
		static void editFileExtension(std::wstring oldFile, std::wstring& newFile, std::wstring const& extension);
		static void editFileExtensionAndPutTempPath(std::wstring oldFile, std::wstring& newFile, std::wstring const& extension);
		/*!
		\brief	删除文件后缀
		\param filepath	文件后缀
		\param length	字符串长度
		\return
		*/
		static void eraseFileExtension(wchar_t* filepath);
		static void  eraseFileExtension(std::wstring& filepath);

		//! 格式化整数到0-255
		static void formatInt32(long& value);
		// 获取文件后缀
		static std::string getFileExtention(std::string const& target);
		static std::wstring getFileExtention(std::wstring const& target);
		//把字符串根据规则拆分为vector
		static std::vector<std::wstring> SplitString(std::wstring value, std::wstring splitChar);

		static std::vector<int> SplitStringToInt(std::wstring value, std::wstring splitChar);

			
		static std::string IntToString(int n);

		static std::string lltoString(long long t);

		static std::wstring IntToWstring(int i);

		static bool IsNum(std::wstring const& wstr);
		static bool IsNum(std::string const& str);

		static bool IsCapital(std::wstring const& wstr);
		static bool IsCapital(std::string const& str);

		static 	void getFiles( std::string path, std::vector<std::wstring>& files,std::wstring const& fileName = L"" ,bool bCotains = false)  ;

		static bool CompareWstring(std::wstring const& wstr1,std::wstring const& wstr2);
	};
}