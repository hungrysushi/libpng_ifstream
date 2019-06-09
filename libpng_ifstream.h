#pragma once

#include <png.h>
#include <iostream>
#include <fstream>
#include <string>

void ReadData(png_structp png_ptr, png_bytep data, png_size_t length);
char* LoadPng(const std::string& filename, int& width, int& height, int& channels);
char* Error(const std::string& message);

