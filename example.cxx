#include "argopts.hxx"

#include <iostream>

int main(int argc, char **argv) {

  for (auto &opt : ArgOpts::Parser().parse(argc, argv)) {
    if ((opt.shortopt == 'h') ||
        (opt.longopt == "help")) {
      std::cout << "Usage:\n" << argv[0] << " [options]\n";
      std::cout << "Options:\n";
      std::cout << "-h, --help		print help message\n";
      std::cout << "-v, --verbose	print more\n";
      
    } else if ((opt.shortopt == 'v') ||
               (opt.longopt == "verbose")) {
      std::cout << "Verbose\n";
      
    } else if ((opt.shortopt == 'f') ||
               (opt.longopt == "file")) {
      // Option to set a file name
      // Check there's another argument after
      if (opt.index == argc-1) {
        std::cerr << "Missing argument to file option\nUsage: -f <file>\n";
        return 1;
      }
      std::string filename(argv[opt.index+1]);
      std::cout << "Using file: '" << filename << "'\n";
      
    } else {
      std::cout << "Unknown option\n";
    }
  }
  
  return 0;
}
 
 
