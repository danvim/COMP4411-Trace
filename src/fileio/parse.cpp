// Stupid MSVC complains (for like 1 million lines) that the ugly identifiers for templated
// classes are truncated.  Not my problem, eh?
#ifdef WIN32
#pragma warning( disable : 4786 )
#endif

#include <cstring>

#include "parse.h"

static std::string readId( std::istream& is );
static Obj *readString( std::istream& is );
static Obj *readScalar( std::istream& is );
static Obj *readTuple( std::istream& is );
static Obj *readDict( std::istream& is );
static Obj *readObject( std::istream& is );
static Obj *readName( std::istream& is );
static void eatWs( std::istream& is );
static void eatNl( std::istream& is );

Obj *readFile( std::istream& is )
{
	return readObject( is );
}

static void eatWs( std::istream& is )
{
	auto ch = is.peek();
	while( ch == ' ' || ch == '\t' || ch == '\n' || ch == 0x0D || ch == 0x0A) {
		is.get();
		if( !is ) {
			return;
		}
		ch = is.peek();
	}
}

static void eatNl( std::istream& is )
{
	auto ch = is.peek();
	while( ch != '\n' ) {
		is.get();
		if( !is ) {
			return;
		}
		ch = is.peek();
	}
}

static bool eat( std::istream& is ) 
{
	while( is ) {
		eatWs( is );
		if( is ) {
			const auto c = is.peek();
			if( c == '/' ) {
				is.get();
				auto ch = is.peek();
				if( ch == '/' ) {
					eatNl( is );
				} else if( ch == '*' ) {
					while( true ) {
						is.get();
						ch = is.peek();
						if( ch == '*' ) {
							is.get();
							ch = is.peek();
							if( ch == '/' ) {
								is.get();
								break;
							}
							if( ch == -1 ) {	
								is.get();
								throw ParseError( 
									"Parse Error: unterminated comment" );
							}
						} else if( ch == -1 ) {
							is.get();
							throw ParseError( 
								"Parse Error: unterminated comment" );
						}
					}
				} else {
					return true;
				}
			} else if( c == -1 ) {
				is.get();
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	}
	return false;
}

static Obj *readName( std::istream& is )
{
	const auto s = readId( is );

	if( s == "true" ) {
		return new BooleanObj( true );
	}
	if( s == "false" ) {
		return new BooleanObj( false );
	}
	if( !eat( is ) ) {
		return new IdObj( s );
	}

	const auto ch = is.peek();
	if( strchr( "}),;", ch ) != nullptr ) {
		return new IdObj( s );
	}
	return new NamedObj( s, readObject( is ) );
}

static std::string readId( std::istream& is )
{
	std::string ret;

	ret += char( is.get() );

	while( is ) {
		const auto ch = is.peek();
		if( strchr( " \t\n={}();,/", ch ) != nullptr ) {
			break;
		}
		ret += char( ch );
		is.get();
	}

	return ret;
}

static Obj *readString( std::istream& is ) 
{
	std::string ret;

	is.get();

	while( true ) {
		const auto ch = is.peek();
		if( ch == '"' ) {
			is.get();
			return new StringObj( ret );
		}
		ret += char( ch );
		is.get();
	}
}

static Obj *readScalar( std::istream& is )
{
	std::string ret;

	while( true ) {
		auto ch = is.peek();
		if( (ch == '-') || (ch == '.') || (ch == 'e') || (ch == 'E')
				|| (ch >= '0' && ch <= '9') ) {
			ret += char( ch );
			is.get();
		} else {
			break;
		}
	}

	return new ScalarObj( atof( ret.c_str() ) );
}

static Obj *readTuple( std::istream& is )
{
	std::vector<Obj*> ret;

	is.get();

	while( true ) {
		eat( is );
		ret.push_back( readObject( is ) );	
		eat( is );
		auto ch = is.get();
		if( ch == ')' ) {
			return new TupleObj( ret );
		}
		if( ch != ',' ) {
			throw ParseError( "Parse error: expected comma." );
		}
	}
}

static Obj *readDict( std::istream& is )
{
	std::map<std::string,Obj*> ret;

	is.get();

	while( true ) {
		eat( is );
		if( is.peek() == '}' ) {
			is.get();
			return new DictObj( ret );
		}
		auto lhs = readId(is);
		eat( is );
		if( is.get() != '=' ) {
			throw ParseError( "Parse error: expected equals." );
		}
		auto* rhs = readObject(is);
		ret[ lhs ] = rhs;
		eat( is );
		auto ch = is.peek();
		if( ch == ';' ) {
			is.get();
		} else if( ch != '}' ) {
			throw ParseError( "Parse error: expected semicolon or brace." );
		}
	}
}

static Obj *readObject( std::istream& is )
{
	if( !eat( is ) ) {
		return nullptr;
	}

	auto ch = is.peek();

	if( (ch == '-') || (ch >= '0' && ch <= '9') ) {
		return readScalar( is );
	}
	if( ch == '"' ) {
		return readString( is );
	}
	if( ch == '(' ) {
		return readTuple( is );
	}
	if( ch == '{' ) {
		return readDict( is );
	}
	return readName( is );
}

/*
int main( void )
{
	Obj *o = readFile( cin );
	o->printOn( cout );
	delete o;
	return 0;
}
*/
