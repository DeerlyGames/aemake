#pragma once
#include "stdint.h"

uint64_t fileLastModified(const char* _path);

int fileExists(const char* _path);

int runPremake(int argc, const char** argv);

void readSettings();