// object.hpp

#pragma once

#include <string>
#include <typeinfo>

namespace realware
{
	class cContext;

	class cTypeInfo
	{
	public:
		cTypeInfo(const std::string& name) : _name(name) {}
		~cTypeInfo() = default;

		inline const std::string& GetName() const { return _name; }

	private:
		std::string _name = "";
	};
	
	class cObject
	{
	public:
		explicit cObject() = default;
		virtual ~cObject() = default;

		virtual const cTypeInfo* GetType() const final;
		virtual const std::string& GetTypeName() const final;

	private:
		mutable const cTypeInfo* _type = nullptr;
	};

	class iObjectFactory
	{
	public:
		explicit iObjectFactory(cContext* context) : _context(context) {}
		~iObjectFactory() = default;

		virtual cObject* Create() = 0;
		const std::string& GetType() { return _type; }

	protected:
		cContext* _context = nullptr;
		std::string _type = "";
	};

	template <typename T>
	class cObjectFactory : public iObjectFactory
	{
	public:
		explicit cObjectFactory(cContext* context) : iObjectFactory(context) {}

		virtual T* Create() override final {}
	};
}