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
	virtual ~Resource() { UnregisterWeakResource(GetName()); UnregisterSmartResource(GetName()); }
	
	static void RegisterWeakResource(const SmartPointer<Resource>& ptr)
	{
		Assert(ptr);
		const auto name = ptr->GetName();
		AssertMsg(!weakResources.count(name), "Weak Resource with name \"" + name + " already exists");
		weakResources.emplace(name, ptr);
	}
	static void UnregisterWeakResource(const std::string& name)
	{
		if(weakResources.size() == 0) return;
		auto find = weakResources.find(name);
		if(find != weakResources.end())
			weakResources.erase(find);
	}

	static void RegisterSmartResource(const SmartPointer<Resource>& ptr)
	{
		Assert(ptr);
		const auto name = ptr->GetName();
		AssertMsg(!smartResources.count(name), "Smart Resource with name \"" + name + " already exists");
		smartResources.emplace(name, ptr);
	}
	static void UnregisterSmartResource(const std::string& name)
	{
		if(smartResources.size() == 0) return;
		auto find = smartResources.find(name);
		if(find != smartResources.end())
			smartResources.erase(find);
	}

public:
	static SmartPointer<T> FindResourceByName(const std::string& name)
	{
		auto find = smartResources.find(name);
		if(find != smartResources.end())
			return find->second;

		auto find2 = weakResources.find(name);
		if(find2 == weakResources.end())
			return nullptr;
		return find2->second.Lock();
	}

	static bool CanFindWeakResourceByName(const std::string& name)
	{
		auto find = weakResources.find(name);
		if(find == weakResources.end())
			return false;
		return true;
	}

	static bool CanFindSmartResourceByName(const std::string& name)
	{
		auto find = smartResources.find(name);
		if(find == smartResources.end())
			return false;
		return true;
	}
private:
	static std::unordered_map<std::string, WeakPointer<Resource>> weakResources;
	static std::unordered_map<std::string, SmartPointer<Resource>> smartResources;
};

template <typename T>
std::unordered_map<std::string, WeakPointer<Resource<T>>> Resource<T>::weakResources;

template <typename T>
std::unordered_map<std::string, SmartPointer<Resource<T>>> Resource<T>::smartResources;
}	// namespace
#endif	// GLADE_RESOURCE_H

