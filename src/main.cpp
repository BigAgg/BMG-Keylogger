/*
*	MIT License
*
*	Copyright (c) 2025 BigAgg
*
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:

*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.

*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	SOFTWARE.
*/

#ifndef VERSION
#define VERSION "0.0.0"
#endif

#include "engine.h"
#include "logger.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char *argv[]) {
  int exitCode = EXIT_SUCCESS;
  try {
    bool logging = false;
    for (int x = 0; x < argc; x++) {
      std::string arg = argv[x];
      if (arg == "-l" || arg == "--logging")
        logging = true;
    }
    engine::setup(logging);
    engine::run();
    engine::saveSettings();
  } catch (const std::exception &e) {
    logger::log("ERROR", e.what());
    std::ifstream src("run.log", std::ios::binary);
    std::ofstream dest("crash.log", std::ios::binary);
    dest << src.rdbuf();
    exitCode = EXIT_FAILURE;
  }
  if(engine::isRunning())
    engine::shutdown();
  return exitCode;
}
