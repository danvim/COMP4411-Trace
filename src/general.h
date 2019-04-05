#ifndef GENERAL_H_
#define GENERAL_H_

#include <string>
#include <utility>

class Exception
{
public:
	explicit Exception(std::string m ) 
		: msg(std::move(m)) {}

	std::string getMsg() const { return msg; }

private:
	std::string msg;
};

inline std::ostream& operator <<(std::ostream& os, const Exception& x )
{
	return os << x.getMsg();
}

#endif // GENERAL_H_
