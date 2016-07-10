#include "Resource.h"

using namespace Glade;

IResource::IResource(const std::string& name) : name(name), isValid(false) {}

IResource::~IResource() {}

// Auto Generate a string-version of an integer ID to identify resources
std::string IResource::GenerateName(char* const format)
{
	static int nameCounter = 1;
	char buffer[256];
	sprintf_s(buffer, format, nameCounter++);
	return std::string(buffer);
}

const std::string& IResource::GetName() const { return name; }
bool IResource::GetIsValid() const { return isValid; }