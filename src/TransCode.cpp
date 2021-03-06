#include "TransCode.h"

namespace CppJieba
{
	vector<string> TransCode::_encVec;
	bool TransCode::_isInitted = TransCode::init();
	TransCode::pf_decode_t TransCode::_pf_decode = NULL;
	TransCode::pf_encode_t TransCode::_pf_encode = NULL;

	bool TransCode::init()
	{
		_pf_decode = gbkToVec;
		_pf_encode = vecToGbk;
		return true;
	}

	TransCode::TransCode()
	{
		
	}
	TransCode::~TransCode()
	{
	}

	void TransCode::setGbkEnc()
	{
		_pf_decode = gbkToVec;
		_pf_encode = vecToGbk;
	}

	void TransCode::setUtf8Enc()
	{
		_pf_decode = utf8ToVec;
		_pf_encode = vecToUtf8;
	}
	

	bool TransCode::utf8ToVec(const string& str, vector<uint16_t>& vec)
	{
		char ch1, ch2;
		if(str.empty())
		{
			return false;
		}
		vec.clear();
		size_t siz = str.size();
		for(uint i = 0;i < siz;)
		{
			if(!(str[i] & 0x80)) // 0xxxxxxx
			{
				vec.push_back(str[i]);
				i++;
			}
			else if ((unsigned char)str[i] <= 0xdf && i + 1 < siz) // 110xxxxxx
			{
				ch1 = (str[i] >> 2) & 0x07;
				ch2 = (str[i+1] & 0x3f) | ((str[i] & 0x03) << 6 );
				vec.push_back(twocharToUint16(ch1, ch2));
				i += 2;
			}
			else if((unsigned char)str[i] <= 0xef && i + 2 < siz)
			{
				ch1 = (str[i] << 4) | ((str[i+1] >> 2) & 0x0f );
				ch2 = ((str[i+1]<<6) & 0xc0) | (str[i+2] & 0x3f); 
				vec.push_back(twocharToUint16(ch1, ch2));
				i += 3;
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	bool TransCode::gbkToVec(const string& str, vector<uint16_t>& vec)
	{
		vec.clear();
		if(str.empty())
		{
			return false;
		}
		uint i = 0;
		while(i < str.size())
		{
			if(0 == (str[i] & 0x80))
			{
				vec.push_back(uint16_t(str[i]));
				i++;
			}
			else
			{
				if(i + 1 < str.size()) //&& (str[i+1] & 0x80))
				{
					vec.push_back(twocharToUint16(str[i], str[i + 1]));
					i += 2;
				}
				else
				{
					return false;
				}
			}
		}
		return true;
	}
    

	bool TransCode::vecToUtf8(Unicode::const_iterator begin, Unicode::const_iterator end, string& res)
	{
		if(begin >= end)
		{
			return false;
		}
        res.clear();
		uint16_t ui;
		while(begin != end)
		{
			ui = *begin;
			if(ui <= 0x7f)
			{
				res += char(ui);
			}
			else if(ui <= 0x7ff)
			{
				res += char(((ui>>6) & 0x1f) | 0xc0);
				res += char((ui & 0x3f) | 0x80);
			}
			else
			{
				res += char(((ui >> 12) & 0x0f )| 0xe0);
				res += char(((ui>>6) & 0x3f )| 0x80 );
				res += char((ui & 0x3f) | 0x80);
			}
			begin ++;
		}
		return true;
	}

	bool TransCode::vecToGbk(Unicode::const_iterator begin, Unicode::const_iterator end, string& res)
	{
		if(begin >= end)
		{
			return false;
		}
        res.clear();
		pair<char, char> pa;
		while(begin != end)
		{
			pa = uint16ToChar2(*begin);
			if(pa.first & 0x80)
			{
				res += pa.first;
				res += pa.second;
			}
			else
			{
				res += pa.second;
			}
			begin++;
		}
		return true;
	}
}


#ifdef CPPJIEBA_TRANSCODE_UT
using namespace CPPCOMMON;
using namespace CppJieba;
int main()
{
	string a("abd你好世界!a");
	vector<uint16_t> vec;
	//TransCode::setUtf8Enc();
	cout<<TransCode::decode(a, vec)<<endl;
	PRINT_VECTOR(vec);

	cout<<TransCode::encode(vec.begin(), vec.end())<<endl;
	
	return 0;
}
#endif
