#ifndef BINARY_STREAM_H
#define BINARY_STREAM_H

#include <iostream>
#include "Exception.h"
#include "StdTypes.h"


/// Read from binary streams. You can read/write data as if it is an iostream but it also contains methods for
/// reading/writing binary data
class BinaryStream: public std::iostream
{
	public:
		/// The 2 available byte orders
		enum ByteOrder
		{
			BO_LITTLE_ENDIAN, ///< The default
			BO_BIG_ENDIAN
		};

		/// The one and only constructor
		/// @param sb An std::streambuf for in/out
		/// @param byteOrder The stream byte order
		BinaryStream(std::streambuf* sb, ByteOrder byteOrder = BO_LITTLE_ENDIAN);

		/// Read unsigned int
		uint readUint();

		/// Read float
		float readFloat();

		/// Read a string. It reads the size as an unsigned int and then it reads the characters
		std::string readString();

		/// @return The byte order of the current running platform
		static ByteOrder getMachineByteOrder();

	private:
		ByteOrder byteOrder;

		/// A little hack so we dont write duplicate code
		template<typename Type>
		Type read32bitNumber();
};


inline BinaryStream::BinaryStream(std::streambuf* sb, ByteOrder byteOrder_):
	std::iostream(sb),
	byteOrder(byteOrder_)
{}


#endif
