#pragma once
#ifndef GLADE_RESOURCE_H
#define GLADE_RESOURCE_H
#include "../Utils/SmartPointer/WeakPointer.h"
#include "../Utils/Assert.h"
#include <string>
#include <unordered_map>

namespace Glade {
class IResource
{
protected:
	IResource(const std::string& name);
	virtual ~IResource();

	static std::string GenerateName(char* const format);

public:
	const std::string& GetName() const;
	virtual bool GetIsValid() const;

private:
	std::string name;
	
protected:
	bool isValid;
};

template <typename T>
class Resource : public IResource
{
protected:
	Resource(const std::string& name) : IResource(name) { }
	virtual ~Resource() { UnregisterResource(GetName()); }
	
	static void RegisterResource(const SmartPointer<Resource>& ptr)
	{
		Assert(ptr);
		const auto name = ptr->GetName();
		AssertMsg(!resources.count(name), "Resource with name \"" + name + " already exists");
		resources.emplace(name, ptr);
	}
	static void UnregisterResource(const std::string& name)
	{
		auto find = resources.find(name);
		if(find != resources.end())
			resources.erase(find);
	}

public:
	static SmartPointer<T> FindResourceByName(const std::string& name)
	{
		auto find = resources.find(name);
		if(find == resources.end())
			return nullptr;
		return find->second.Lock();
	}

	static bool CanFindResourceByName(const std::string& name)
	{
		auto find = resources.find(name);
		if(find == resources.end())
			return false;
		return true;
	}

private:
	static std::unordered_map<std::string, WeakPointer<Resource>> resources;
};

template <typename T>
std::unordered_map<std::string, WeakPointer<Resource<T>>> Resource<T>::resources; 
}	// namespace
#endif	// GLADE_RESOURCE_H

