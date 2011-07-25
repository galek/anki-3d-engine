#include <cstring>
#include "Parser.h"
#include "Util/Exception.h"


namespace Parser {


//==============================================================================
// parseArrOfNumbers                                                           =
//==============================================================================
template <typename Type>
void parseArrOfNumbers(Scanner::Scanner& scanner, bool bracket, bool signs,
	uint size, Type* arr)
{
	const Scanner::Token* token;

	// first of all the left bracket
	if(bracket)
	{
		token = &scanner.getNextToken();
		if(token->getCode() != Scanner::TC_LBRACKET)
		{
			throw PARSER_EXCEPTION_EXPECTED("{");
		}
	}

	// loop to parse numbers
	for(uint i=0; i<size; i++)
	{
		token = &scanner.getNextToken();


		// check if there is a sign in front of a number
		bool sign = 0; // sign of the number. 0 for positive and 1 for negative
		if(signs)
		{
			if(token->getCode() == Scanner::TC_MINUS)
			{
				sign = 1;
				token = &scanner.getNextToken();
			}
			else if(token->getCode() == Scanner::TC_PLUS)
			{
				sign = 0;
				token = &scanner.getNextToken();
			}

			// check if not number
			if(token->getCode() != Scanner::TC_NUMBER)
			{
				throw PARSER_EXCEPTION_EXPECTED("number");
			}
		} // end if signs

		// put the number in the arr and do typecasting from int to float
		Type nmbr;

		if(token->getDataType() == Scanner::DT_FLOAT)
		{
			nmbr = static_cast<Type>(token->getValue().getFloat());
		}
		else
		{
			nmbr = static_cast<Type>(token->getValue().getInt());
		}

		arr[i] = (sign==0) ? nmbr : -nmbr;
	}

	// the last thing is the right bracket
	if(bracket)
	{
		token = &scanner.getNextToken();
		if(token->getCode() != Scanner::TC_RBRACKET)
		{
			throw PARSER_EXCEPTION_EXPECTED("}");
		}
	}
}


//==============================================================================
// parseNumber                                                                 =
//==============================================================================
template <typename Type>
void parseNumber(Scanner::Scanner& scanner, bool sign, Type& out)
{
	try
	{
		const Scanner::Token* token = &scanner.getNextToken();
		bool negative = false;

		// check the sign if any
		if(sign)
		{
			if(token->getCode() == Scanner::TC_PLUS)
			{
				negative = false;
				token = &scanner.getNextToken();
			}
			else if(token->getCode() == Scanner::TC_MINUS)
			{
				negative = true;
				token = &scanner.getNextToken();
			}
			else if(token->getCode() != Scanner::TC_NUMBER)
			{
				throw PARSER_EXCEPTION_EXPECTED("number or sign");
			}
		}

		// check the number
		if(token->getCode() != Scanner::TC_NUMBER)
		{
			throw PARSER_EXCEPTION_EXPECTED("number");
		}

		if(token->getDataType() == Scanner::DT_FLOAT)
		{
			double d = negative ? -token->getValue().getFloat() :
				token->getValue().getFloat();
			out = static_cast<Type>(d);
		}
		else
		{
			ulong l = negative ? -token->getValue().getInt() :
				token->getValue().getInt();
			out = static_cast<Type>(l);
		}
	}
	catch(std::exception& e)
	{
		throw EXCEPTION(e.what());
	}
}


//==============================================================================
// parseMathVector                                                             =
//==============================================================================
template <typename Type>
void parseMathVector(Scanner::Scanner& scanner, Type& out)
{
	try
	{
		const Scanner::Token* token = &scanner.getNextToken();
		uint elementsNum = sizeof(Type) / sizeof(float);

		// {
		if(token->getCode() != Scanner::TC_LBRACKET)
		{
			throw PARSER_EXCEPTION_EXPECTED("{");
		}

		// numbers
		for(uint i=0; i<elementsNum; i++)
		{
			double d;
			parseNumber(scanner, true, d);
			out[i] = d;
		}

		// }
		token = &scanner.getNextToken();
		if(token->getCode() != Scanner::TC_RBRACKET)
		{
			throw PARSER_EXCEPTION_EXPECTED("}");
		}
	}
	catch(std::exception& e)
	{
		throw EXCEPTION(e.what());
	}
}


//==============================================================================
// parseBool                                                                   =
//==============================================================================
inline bool parseBool(Scanner::Scanner& scanner)
{
	const char* errMsg = "identifier true or false";

	const Scanner::Token* token = &scanner.getNextToken();
	if(token->getCode() != Scanner::TC_IDENTIFIER)
	{
		throw PARSER_EXCEPTION_EXPECTED(errMsg);
	}

	if(!strcmp(token->getValue().getString(), "true"))
	{
		return true;
	}
	else if (!strcmp(token->getValue().getString(), "false"))
	{
		return false;
	}
	else
	{
		throw PARSER_EXCEPTION_EXPECTED(errMsg);
	}
}


//==============================================================================
// parseIdentifier                                                             =
//==============================================================================
inline std::string parseIdentifier(Scanner::Scanner& scanner,
	const char* expectedIdentifier)
{
	const Scanner::Token* token = &scanner.getNextToken();
	if(token->getCode() != Scanner::TC_IDENTIFIER)
	{
		if(expectedIdentifier == NULL)
		{
			throw PARSER_EXCEPTION_EXPECTED("identifier");
		}
		else
		{
			throw PARSER_EXCEPTION_EXPECTED("identifier " + expectedIdentifier);
		}
	}

	if(expectedIdentifier != NULL &&
		strcmp(token->getValue().getString(), expectedIdentifier))
	{
		throw PARSER_EXCEPTION_EXPECTED("identifier " + expectedIdentifier);
	}

	return token->getValue().getString();
}


//==============================================================================
// isIdentifier                                                                =
//==============================================================================
inline bool isIdentifier(const Scanner::Token& token, const char* str)
{
	return token.getCode() == Scanner::TC_IDENTIFIER &&
		!strcmp(token.getValue().getString(), str);
}


//==============================================================================
// parseString                                                                 =
//==============================================================================
inline std::string parseString(Scanner::Scanner& scanner)
{
	const Scanner::Token* token = &scanner.getNextToken();
	if(token->getCode() != Scanner::TC_STRING)
	{
		throw PARSER_EXCEPTION_EXPECTED("string");
	}
	return token->getValue().getString();
}


} // End namespace Parser
