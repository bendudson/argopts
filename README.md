ArgOpts
=======

A simple command line option parser for C++11. 

Matches short options like "-h", "-v" and long options like "--help", "--verbose".
Short options can be combined, so "-hvv" is equivalent to "-h -v -v".
Arguments not starting with '-' are ignored, and parsing stops when '--' is found.

All code in a single header file, released under MIT license.

Examples
--------

Simple argument parser
~~~~~~~~~~~~~~~~~~~~~~

    
    for (auto &opt : ArgOpts().parse(argc, argv)) {
      if ((opt.shortopt == 'h') ||
          (opt.longopt == "help")) {
        std::cout << "Usage:\n" << argv[0] << " [options]\n";
        std::cout << "Options:\n";
        std::cout << "-h, --help		print help message\n";
        std::cout << "-v, --verbose	print more\n";
   
      } else if ((opt.shortopt == 'v') ||
                 (opt.longopt == "verbose")) {
        std::cout << "Verbose\n";
   
      }
    }

Group short and long options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here matches short and long options and handles printing of the option help

   
    ArgOpts args = { {'h', "help", "print help message"},
                     {'v', "verbose", "print more"} };
   
    for (auto &opt: args.parse(argc, argv)) {
      switch (opt.shortopt) {
       case 'v': {
          std::cout << "Verbose\n";
          break;
        }
       case 'h': { // Print help message
         std::cout << "Usage:\n" << argv[0] << " [options]\n";
         std::cout << "Options:\n"
                   << args.printOptions() << "\n";
         return 0;
       }
      }
    }


MIT licence (OSI)
-----------------

Copyright (c) 2017 Ben Dudson <benjamin.dudson@york.ac.uk>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
