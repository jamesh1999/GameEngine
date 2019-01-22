#ifndef __DESCRIPTOR_PARSER_INCLUDED__
#define __DESCRIPTOR_PARSER_INCLUDED__

#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>

class DescriptorParser
{
public:

	enum Type { Atom, List, Object };

	// Determine the type of a given section in a descriptor
	static Type GetType(const std::string& descriptor)
	{
		switch (descriptor[0])
		{
		case '[':
			return List;
		case '{':
			return Object;
		}
		return Atom;
	}

	// Descriptor list -> vector of parts
	static std::vector<std::string> ParseList(const std::string& descriptor)
	{
		std::vector<std::string> parts;

		int depth = 0;
		size_t start = 0;
		size_t pos = descriptor.find_first_of(";[]{}\\", 1);

		while (pos != descriptor.size() - 1)
		{
			switch (descriptor[pos])
			{
			case '[':
			case '{':
				++depth;
				break;
			case ']':
			case '}':
				--depth;
				break;
			case '\\':
				++pos;
				break;
			case ';':
				if (depth != 0) break;
				parts.push_back(descriptor.substr(start, pos - start));
				start = pos + 1;
			}

			pos = descriptor.find_first_of(";[]{}\\", pos + 1);
		}
		parts.push_back(descriptor.substr(start, pos - start));

		return parts;
	}

	// Descriptor object -> part KVPs
	static std::unordered_map<std::string, std::string> ParseObject(const std::string& descriptor)
	{
		std::vector<std::string> tokens;

		int depth = 0;
		size_t start = 0;
		size_t pos = descriptor.find_first_of(";:[]{}\\", 1);

		while (pos != descriptor.size() - 1)
		{
			switch (descriptor[pos])
			{
			case '[':
			case '{':
				++depth;
				break;
			case ']':
			case '}':
				--depth;
				break;
			case '\\':
				++pos;
				break;
			case ':':
			case ';':
				if (depth != 0) break;
				tokens.push_back(descriptor.substr(start, pos - start));
				start = pos + 1;
			}

			pos = descriptor.find_first_of(";:[]{}\\", pos + 1);
		}
		tokens.push_back(descriptor.substr(start, pos - start));

		std::unordered_map<std::string, std::string> kvps;
		for (int i = 0; i < tokens.size(); i += 2)
			kvps[tokens[i]] = tokens[i + 1];

		return kvps;
	}

	static std::vector<std::string> GetParts(const std::string& descriptor)
	{
		std::vector<std::string> parts;

		int depth = 0;
		size_t start = 0;
		size_t pos = descriptor.find_first_of(";[]");

		while (pos != -1)
		{
			

			switch (descriptor[pos])
			{
			case '[':
				++depth;
				break;
			case ']':
				--depth;
				break;
			case ';':
				if (depth != 0) break;
				parts.push_back(descriptor.substr(start, pos - start));
				start = pos + 1;
			}

			pos = descriptor.find_first_of(";[]", pos + 1);
		}
		parts.push_back(descriptor.substr(start, descriptor.size() - start));

		for (std::string& part : parts)
		{
			if (part[0] != '[') continue;
			part = part.substr(1, part.size() - 2);
		}

		return parts;
	}

	static std::string GetExtension(std::string filename)
	{
		size_t idx = filename.find_last_of('.');
		std::string extension = filename.substr(idx + 1, filename.size() - idx - 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		return extension;
	}

};

#endif