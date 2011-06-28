#ifndef SCANNER_H
#define SCANNER_H

#include <fstream>
#include "ScannerToken.h"


namespace Scanner {


/// C++ Tokenizer
///
/// The Scanner loads a file or an already loaded iostream and extracts the tokens. The script must be in C++ format.
/// The class does not make any kind of memory allocations so it can be fast.
class Scanner
{
	//==================================================================================================================
	// Public                                                                                                          =
	//==================================================================================================================
	public:
		/// Constructor #1
		/// @param newlinesAsWhitespace @see newlinesAsWhitespace
		Scanner(bool newlinesAsWhitespace = true);

		/// Constructor #2
		/// @see loadFile
		/// @param newlinesAsWhitespace @see newlinesAsWhitespace
		/// @exception Exception
		Scanner(const char* filename, bool newlinesAsWhitespace = true);

		/// Constructor #3
		/// @see loadIstream
		/// @param newlinesAsWhitespace @see newlinesAsWhitespace
		/// @exception Exception
		Scanner(std::istream& istream_, const char* scriptName_ = "unamed-istream", bool newlinesAsWhitespace = true);

		/// It only unloads the file if file is chosen
		~Scanner() {unload();}

		/// Load a file to extract tokens
		/// @param filename The filename of the file to read
		/// @exception Exception
		void loadFile(const char* filename);

		/// Load a STL istream to extract tokens
		/// @param istream_ The stream from where to read
		/// @param scriptName_ The name of the stream. For error reporting
		/// @exception Exception
		void loadIstream(std::istream& istream_, const char* scriptName_ = "unamed-istream");

		/// Extracts all tokens and prints them. Used for debugging
		void getAllPrintAll();

		/// Get the next token from the stream. Its virtual and you can override it
		/// @return The next Token
		/// @exception Exception
		virtual const Token& getNextToken();

		/// Accessor for the current token
		/// @return The current Token
		const Token& getCrntToken() const {return crntToken;}

		/// Get the name of the input stream
		const char* getScriptName() const {return scriptName;}

		/// Get the current line the Scanner is processing
		int getLineNumber() const {return lineNmbr;}

	//==================================================================================================================
	// Protected                                                                                                       =
	//==================================================================================================================
	protected:
		/// Every char in the Ascii table is binded with one characteristic code type. This helps the scanning
		enum AsciiFlag
		{
			AC_ERROR = 0,
			AC_EOF = 1,
			AC_LETTER = 2,
			AC_DIGIT = 4,
			AC_SPECIAL = 8,
			AC_WHITESPACE = 16,
			AC_QUOTE = 32,
			AC_DOUBLEQUOTE = 64,
			AC_ACCEPTABLE_IN_COMMENTS = 128 ///< Only accepted in comments
		};

		/// Reserved words like "int" "const" etc. Currently the reserved words list is being populated with dummy data
		struct ResWord
		{
			const char* string;
			TokenCode   code;
		};

		static char eofChar; ///< Special end of file character

		static AsciiFlag asciiLookupTable[]; ///< The array contains one AsciiFlag for every symbol of the ASCII table

		/// @name Reserved words
		/// Groups of ResWord grouped by the length of the ResWord::string
		/// @{
		static ResWord rw2[], rw3[], rw4[], rw5[], rw6[], rw7[];
		/// @}

		static ResWord* rwTable[]; ///< The array contains all the groups of ResWord

		Token crntToken; ///< The current token
		char  line[MAX_SCRIPT_LINE_LEN]; ///< In contains the current line's text
		char* pchar; ///< Points somewhere to @ref line
		int   lineNmbr; ///< The number of the current line

		/// Treat newlines as whitespace. If false means that the Scanner returns (among others) newline tokens
		bool newlinesAsWhitespace;

		/// Commented lines number
		/// Used to keep track of the newlines in multiline comments so we can then return the correct number of newlines
		/// in case of newlinesAsWhitespace is false
		int commentedLines;

		/// @name Input
		/// @{
		std::ifstream inFstream; ///< The file stream. Used if the @ref Scanner is initiated using @ref loadFile
		std::istream* inStream; ///< Points to either @ref inFstream or an external std::istream
		char scriptName[512]; ///< The name of the input stream. Mainly used for error messaging
		/// @}

		/// @name Checkers
		/// @{
		void checkWord();
		void checkComment();
		void checkNumber();
		void checkString();
		void checkChar();
		void checkSpecial();
		/// @}

		/// It reads a new line from the iostream and it points @ref pchar to the beginning of that line
		void getLine();

		/// Get the next char from the @ref line. If @ref line empty then get new line. It returns '\\0' if we are in the
		/// end of the line
		char getNextChar();

		/// Put the char that @ref getNextChar got back to the current line
		char putBackChar();

		/// Initializes the asciiLookupTable. It runs only once in the construction of the first Scanner @see Scanner()
		static void initAsciiMap();

		/// To save us from typing
		AsciiFlag lookupAscii(char ch_) {return asciiLookupTable[(int)ch_];}

		/// Common initialization code
		void init(bool newlinesAsWhitespace_);

		/// Unloads the file
		void unload();
}; // end class Scanner


} // end namespace


#endif
