// object.cpp

#include "object.hpp"

namespace realware
{
	const cTypeInfo* cObject::GetType() const
	{
		if (!_type)
			_type = new cTypeInfo(typeid(*this).name());

		return _type;
	}

	const std::string& cObject::GetTypeName() const
	{
		return GetType()->GetName();
	}
}