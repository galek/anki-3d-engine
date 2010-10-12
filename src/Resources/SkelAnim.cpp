#include "SkelAnim.h"
#include "Parser.h"


//======================================================================================================================
// load                                                                                                                =
//======================================================================================================================
void SkelAnim::load(const char* filename)
{
	Scanner scanner(filename);
	const Scanner::Token* token;

	// keyframes
	token = &scanner.getNextToken();
	if(token->getCode() != Scanner::TC_NUMBER || token->getDataType() != Scanner::DT_INT)
	{
		throw PARSER_EXCEPTION_EXPECTED("integer");
	}
	keyframes.resize(token->getValue().getInt());

	Parser::parseArrOfNumbers(scanner, false, false, keyframes.size(), &keyframes[0]);

	// bones num
	token = &scanner.getNextToken();
	if(token->getCode() != Scanner::TC_NUMBER || token->getDataType() != Scanner::DT_INT)
	{
		throw PARSER_EXCEPTION_EXPECTED("integer");
	}
	bones.resize(token->getValue().getInt());

	// poses
	for(uint i=0; i<bones.size(); i++)
	{
		// has anim?
		token = &scanner.getNextToken();
		if(token->getCode() != Scanner::TC_NUMBER || token->getDataType() != Scanner::DT_INT)
		{
			throw PARSER_EXCEPTION_EXPECTED("integer");
		}

		// it has
		if(token->getValue().getInt() == 1)
		{
			bones[i].keyframes.resize(keyframes.size());

			for(uint j=0; j<keyframes.size(); ++j)
			{
				// parse the quat
				float tmp[4];
				Parser::parseArrOfNumbers(scanner, false, true, 4, &tmp[0]);
				bones[i].keyframes[j].rotation = Quat(tmp[1], tmp[2], tmp[3], tmp[0]);

				// parse the vec3
				Parser::parseArrOfNumbers(scanner, false, true, 3, &bones[i].keyframes[j].translation[0]);
			}
		}
	} // end for all bones


	framesNum = keyframes[keyframes.size()-1] + 1;
}
