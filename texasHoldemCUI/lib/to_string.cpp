#include <string>

namespace My {
	std::string to_string(int v)
	{
		if( !v ) return std::string("0");
		std::string str;
		while( v != 0 ) {
			str = std::string(1, v % 10 + '0') + str;
			v /= 10;
		}
		return str;
	}
	std::string to_string(int v, int width)		//	width < 0 の場合は左詰め
	{
		std::string str = to_string(v);
		if( width > 0 && width - str.size() > 0 )
			str = std::string(width - str.size(), ' ') + str;
		else if( width < 0 && -width - str.size() > 0 )
			str += std::string(-width - str.size(), ' ');
		return str;
	}
	std::string to_percent_string(double v)
	{
		int i = (int)(v * 100);
		std::string str = to_string(i);
		str += '.';
		str += to_string((int)(v*1000) - i * 10);
		str += '%';
		return str;
	}
};
