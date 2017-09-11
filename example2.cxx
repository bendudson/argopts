#include "argopts.hxx"

#include <iostream>

int main(int argc, char **argv) {
  ArgOpts::Parser args = { {'h', "help", "print help message"},
                           {'v', "verbose", "print more"},
                           {'f', "file", "[FILE] file name"},
                           {'n', "number", "Some input integer"}};
  
  auto options = args.parse(argc, argv);
  // first check for help
  for (auto &opt: options) {
    if (opt.shortopt == 'h') {
      std::cout << "Usage:\n" << argv[0] << " [options]\n";
      std::cout << "Options:\n" << args.printOptions() << "\n";
      return 0;
    }
  }
  // Check other options
  for (auto &opt: options) {
    switch (opt.shortopt) {
    case 'v': {
      std::cout << "Verbose\n";
      break;
    }
    case 'f': {
      // Option to set a file name
      std::string filename = opt.arg;
      std::cout << "Using file: '" << filename << "'\n";
      break;
    }
    case 'n': {
      int num = opt.arg; // Expect an int as the next argument
      std::cout << "Got number: " << num << "\n";
    }
    }
  }
  
  return 0;
}
 
 
