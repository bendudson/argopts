ArgOpts
=======

A simple command line option parser for C++11. 

Matches short options like "-h", "-v" and long options like "--help", "--verbose".
Short options can be combined, so "-hvv" is equivalent to "-h -v -v".
Arguments not starting with '-' are ignored, and parsing stops when '--' is found.

All code in a single header file, released under MIT license.

Example
-------

```C++
  // Short form (single character), long form, and help message
  ArgOpts::Parser args = { {'h', "help", "print this help message"},
                           {'f', "file", "[FILE] file name"},
                           {'n', "number", "[NUMBER] An important integer"}};

  for (auto &opt : ArgOpts::Parser.parse(argc, argv)) {
    switch (opt.shortopt) {
    case 'h': {  // This matches both -h and --help
      std::cout << "Usage:\n" << argv[0] << " [options]\n";
      std::cout << "Options:\n" << args.printOptions() << "\n";
      return 0;
    }
    case 'f': {  // Matches -f and --file
      std::string filename = opt.arg;
      std::cout << "Using file: '" << filename << "'\n";
      break;
    }
    case 'n': {  // Matches -n and --number
      int num = opt.arg; // Expect an int as the next argument
      std::cout << "Got number: " << num << "\n";
      break;
    }
    default: {
      // Unrecognised option
    }
    }
  }
```

If an option is missing an error is printed e.g. a missing argument after "-n":

    terminate called after throwing an instance of 'std::invalid_argument'
    what():  Missing argument, expected type int
    usage: -n, --number		[NUMBER] An important integer

or if a value cannot be parsed correctly e.g. "-n something":

    terminate called after throwing an instance of 'std::invalid_argument'
    what():  Invalid argument: expected type int but got 'something'
    usage: -n, --number		[NUMBER] An important integer


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
