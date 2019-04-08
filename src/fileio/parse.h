#ifndef PARSE_H_
#define PARSE_H_
// Stupid MSVC complains (for like 1 million lines) that the ugly identifiers for template
// classes are truncated.  Not my problem, eh?
#pragma warning( disable : 4786 )

#include <string>
#include <utility>
#include <vector>
#include <map>

class Exception : public std::exception
{
public:
	explicit Exception(std::string m)
		: msg(std::move(m))
	{
	}

	std::string getMsg() const { return msg; }

	const char* what() const override { return msg.c_str(); }

private:
	std::string msg;
};

inline std::ostream& operator <<(std::ostream& os, const Exception& x)
{
	return os << x.getMsg();
}

class Obj;

typedef std::vector<Obj*> MyTuple;
typedef std::map<std::string, Obj*> Dict;

class ParseError
	: public Exception
{
public:
	explicit ParseError(const std::string& msg)
		: Exception(msg)
	{
	}
};

class ObjTypeMismatch
	: public ParseError
{
public:
	ObjTypeMismatch(const std::string&& expected, const std::string&& got)
		: ParseError(std::string("Type mismatch error during parse: expected ") +
			expected + std::string(", got ") + got + std::string("."))
	{
	}
};

class Obj
{
public:
	virtual ~Obj() = default;

	virtual std::string getTypeName() const { return std::string("token"); }

	virtual void printOn(std::ostream& os) const
	{
	};

	virtual double getScalar() const
	{
		throw ObjTypeMismatch(std::string("scalar"), getTypeName());
	}

	virtual bool getBoolean() const
	{
		throw ObjTypeMismatch(std::string("bool"), getTypeName());
	}

	virtual std::string getId() const
	{
		throw ObjTypeMismatch(std::string("id"), getTypeName());
	}

	virtual std::string getString() const
	{
		throw ObjTypeMismatch(std::string("std::string"), getTypeName());
	}

	virtual const MyTuple& getTuple() const
	{
		throw ObjTypeMismatch(std::string("tuple"), getTypeName());
	}

	virtual const Dict& getDict() const
	{
		throw ObjTypeMismatch(std::string("dict"), getTypeName());
	}

	virtual std::string getName() const
	{
		throw ObjTypeMismatch(std::string("named"), getTypeName());
	}

	virtual Obj* getChild() const
	{
		throw ObjTypeMismatch(std::string("named"), getTypeName());
	}

protected:
	Obj() = default;
};

class ScalarObj
	: public Obj
{
public:
	explicit ScalarObj(const double v)
		: val(v)
	{
	}

	virtual ~ScalarObj()
	= default;

	std::string getTypeName() const override { return std::string("scalar"); }
	void printOn(std::ostream& os) const override { os << val; }

	double getScalar() const override { return val; }

private:
	double val;
};

class BooleanObj
	: public Obj
{
public:
	explicit BooleanObj(bool b)
		: val(b)
	{
	}

	virtual ~BooleanObj()
	= default;

	std::string getTypeName() const override { return std::string("bool"); }
	void printOn(std::ostream& os) const override { os << (val ? "true" : "false"); }

	bool getBoolean() const override { return val; }

private:
	bool val;
};

class IdObj
	: public Obj
{
public:
	explicit IdObj(std::string s)
		: val(std::move(s))
	{
	}

	virtual ~IdObj()
	= default;

	std::string getTypeName() const override { return std::string("id"); }
	void printOn(std::ostream& os) const override { os << val; }
	std::string getId() const override { return val; }

private:
	std::string val;
};

class StringObj
	: public Obj
{
public:
	explicit StringObj(std::string s)
		: val(std::move(s))
	{
	}

	virtual ~StringObj()
	= default;

	std::string getTypeName() const override { return std::string("std::string"); }
	void printOn(std::ostream& os) const override { os << '"' << val << '"'; }
	std::string getString() const override { return val; }

private:
	std::string val;
};

class TupleObj
	: public Obj
{
public:
	explicit TupleObj(MyTuple vec)
		: val(std::move(vec))
	{
	}

	virtual ~TupleObj()
	{
		for (auto i = val.begin(); i != val.end(); ++i)
		{
			delete *i;
		}
	}

	std::string getTypeName() const override { return std::string("tuple"); }

	void printOn(std::ostream& os) const override
	{
		auto first = true;
		os << '(';
		for (auto idx : val)
		{
			if (first)
			{
				first = false;
			}
			else
			{
				os << ", ";
			}
			idx->printOn(os);
		}
		os << ')';
	}

	const MyTuple& getTuple() const override { return val; }

private:
	MyTuple val;
};

class DictObj
	: public Obj
{
public:
	explicit DictObj(Dict m)
		: val(std::move(m))
	{
	}

	virtual ~DictObj()
	{
		for (auto i = val.begin(); i != val.end(); ++i)
		{
			delete (*i).second;
		}
	}

	std::string getTypeName() const override { return std::string("dict"); }

	void printOn(std::ostream& os) const override
	{
		auto first = true;
		os << '{';
		for (auto ci = val.begin();
		     ci != val.end(); ++ci)
		{
			if (first)
			{
				first = false;
			}
			else
			{
				os << "; ";
			}
			os << (*ci).first << " = ";
			(*ci).second->printOn(os);
		}
		os << '}';
	}

	const Dict& getDict() const override { return val; }

private:
	Dict val;
};

class NamedObj
	: public Obj
{
public:
	NamedObj(std::string n, Obj* ch)
		: name(std::move(n))
		  , child(ch)
	{
	}

	virtual ~NamedObj()
	{
		delete child;
	}

	std::string getTypeName() const override { return std::string("named"); }

	void printOn(std::ostream& os) const override
	{
		os << name << ' ';
		child->printOn(os);
	}

	std::string getName() const override { return name; }
	Obj* getChild() const override { return child; }

private:
	std::string name;
	Obj* child;
};

Obj* readFile(std::istream& is);

#endif // PARSE_H_
