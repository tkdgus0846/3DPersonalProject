#pragma once

namespace Engine
{
	template <typename T>			// 클래스 포인터들을 해제
	unsigned long Safe_AddRef(T& pointer)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pointer)
			dwRefCnt = pointer->AddRef();

		return dwRefCnt;
	}


	template <typename T>			// 클래스 포인터들을 해제
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



	template <typename T>			// 원시 자료형, 구조체 포인터 해제
	void Safe_Delete(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template <typename T>			// 동적 배열을 삭제하는 용도
	void Safe_Delete_Array(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////
	/////////////////////////////////Functor 함수객체//////////////////////////

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

			//ptr이 null이 아닐 때까지 반복
			while (ptr != NULL) {
				//string으로 변환하여 벡터에 넣어줌
				v.push_back(string(ptr));
				ptr = strtok_s(NULL, delimeter, &context);
			}

			return v;
		}
	};
#endif

}