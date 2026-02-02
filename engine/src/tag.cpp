// tag.cpp

#include "tag.hpp"

using namespace types;

namespace triton
{
	cTag::cTag()
	{
		FillZeros();
	}

	cTag::cTag(const std::string& text) : cTag()
	{
		CopyChars((const u8*)text.c_str(), text.size());
	}

	cTag::cTag(const u8* chars, usize charsByteSize) : cTag()
	{
		CopyChars(chars, charsByteSize);
	}

	bool cTag::operator==(const cTag& rhs)
	{
		if (rhs.GetByteSize() != _byteSize)
			return false;

		for (usize i = 0; i < _byteSize; i++)
		{
			if (rhs.GetData()[i] != _data[i])
				return false;
		}

		return true;
	}

	bool cTag::operator==(const std::string& rhs)
	{
		if (rhs.size() != _byteSize)
			return false;

		for (usize i = 0; i < _byteSize; i++)
		{
			if (rhs[i] != _data[i])
				return false;
		}

		return true;
	}

	void cTag::FillZeros()
	{
		memset(&_data[0], 0, kMaxTagByteSize);
	}

	void cTag::CopyChars(const u8* chars, usize charsByteSize)
	{
		if (chars == nullptr || charsByteSize == 0 || charsByteSize >= kMaxTagByteSize)
			return;

		_byteSize = charsByteSize;
		memcpy(&_data[0], &chars[0], _byteSize);
	}
}