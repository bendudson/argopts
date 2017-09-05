#include "argopts.hxx"

#include <iostream>

int main(int argc, char **argv) {
  ArgOpts args = { {'h', "help", "print help message"},
                   {'v', "verbose", "print more"} };
  
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
      // Check there's another argument after
      if (opt.index == argc-1) {
        std::cerr << "Missing argument to file option\nUsage: -f <file>\n";
        return 1;
      }
      std::string filename(argv[opt.index+1]);
      std::cout << "Using file: '" << filename << "'\n";
      break;
    }
    }
  }
  
  return 0;
}
 
 
