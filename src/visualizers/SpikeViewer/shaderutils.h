#ifndef SHADER_UTILS_H_
#define SHADER_UTILS_H_

#include <iostream>
#include <fstream>
#include <string>

inline bool loadShaderSource(const std::string& filename, std::string& out) {

	std::ifstream file;
    file.open(filename.c_str());
    if (!file) {
	       return false;
    }

	std::stringstream stream;

	stream << file.rdbuf();
	file.close();

	out = stream.str();

	return true;
}

#endif
