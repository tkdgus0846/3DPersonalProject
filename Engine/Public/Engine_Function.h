#pragma once

namespace Engine
{
	template <typename T>			// Ŭ���� �����͵��� ����
	unsigned long Safe_AddRef(T& pointer)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pointer)
			dwRefCnt = pointer->AddRef();

		return dwRefCnt;
	}


	template <typename T>			// Ŭ���� �����͵��� ����
	unsigned long Safe_Release(T& pointer)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pointer)
		{
			dwRefCnt = pointer->Release();

			if (dwRefCnt == 0)
				pointer = nullptr;
		}
		return dwRefCnt;
	}



	template <typename T>			// ���� �ڷ���, ����ü ������ ����
	void Safe_Delete(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template <typename T>			// ���� �迭�� �����ϴ� �뵵
	void Safe_Delete_Array(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////
	/////////////////////////////////Functor �Լ���ü//////////////////////////

	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wchar_t* pTag)
			: m_pTargetTag(pTag)
		{
		}
		~CTag_Finder() {		}
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
			{
				return true;
			}

			return false;
		}

	private:
		const wchar_t*		m_pTargetTag = nullptr;
	};

#ifndef __MODEL_LOADER
	class CConversion
	{
	public:
		static string WstringToString(const wstring& uniMsg)
		{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
			std::string strString = converter.to_bytes(uniMsg);

			return strString;
		}

		static wstring StringToWstring(const string& str)
		{
			wstring retWstring;
			retWstring.assign(str.begin(), str.end());
			return retWstring;
		}

		static vector<string> SplitStringA(string str, const char* delimeter)
		{
			char ch[MAX_PATH];
			char* context = NULL;
			strcpy_s(ch, str.c_str());
			char* ptr = strtok_s(ch, delimeter, &context);
			vector<string> v;

			//ptr�� null�� �ƴ� ������ �ݺ�
			while (ptr != NULL) {
				//string���� ��ȯ�Ͽ� ���Ϳ� �־���
				v.push_back(string(ptr));
				ptr = strtok_s(NULL, delimeter, &context);
			}

			return v;
		}
	};
#endif

}