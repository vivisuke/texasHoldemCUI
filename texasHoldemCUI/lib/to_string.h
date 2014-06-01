#include <string>
namespace My {
	std::string to_string(int v);
	std::string to_string(int v, int width);		//	width < 0 の場合は左詰め
	std::string to_percent_string(double v);
};
