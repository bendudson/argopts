//
// ArgOpts command-line option parser
// ==================================
//
// MIT licence (OSI)
// -----------------
//
// Copyright (c) 2017 Ben Dudson <benjamin.dudson@york.ac.uk>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <list>
#include <string>
#include <sstream>
#include <initializer_list>

namespace ArgOpts {

  struct Option;
  
  /// Stores values as strings, and allows conversion
  /// between types via string storage
  ///
  /// Any type can be assigned to a StringStore
  /// provided that it can be streamed to a std::stringstream
  /// ie implements the "<<" operator
  ///
  /// A StringStore can be assigned to any type which
  /// can be streamed from a std::stringstream
  /// ie implements the ">>" operator
  ///
  class StringStore {
  public:
    StringStore() {}
    StringStore(std::string &value) : value(value) {}
    StringStore(const char *value) : value(value) {}

    /// Constructor from any type T
    /// Tries to stream the input to a std::stringstream
    /// and then saves the resulting string.
    template <typename T> StringStore(const T &inputvalue) {
      std::stringstream ss;
      ss << inputvalue;
      value = ss.str();
    }

    /// Cast operator, which allows this class to be
    /// assigned to type T
    ///
    /// Example
    /// -------
    ///
    /// StringStore s = "3.1415";
    /// double val = s;
    ///
    ///
    template <typename T> operator T() { return get<T>(); }

    /// Get the value as a specified type
    ///
    /// Example
    /// -------
    ///
    /// StringStore s = "3.1415";
    /// double val = s.get<int>();
    ///
    /// This sets val to 3, since the get() function
    /// returns an integer
    template <typename T> T get() {
      if (value.length() == 0) {
        throw "missing value";
      }

      T t;
      std::stringstream ss(value);
      ss >> t;
      if (ss.fail()) {
        throw "Failed to parse";
      }
      return t;
    }

  private:
    std::string value; ///< The internal data store
    Option *option;    ///< The option for this argument. May be nullptr
  };

  /// Trivial case of assignment to string
  template <> StringStore::operator std::string() {
    if (value.length() == 0) {
      throw "missing value";
    }
    return value;
  }

  /// Structure representing a command-line option
  ///
  ///
  struct Option {
    Option(char shortopt, std::string longopt, std::string help,
           int index = -1)
      : shortopt(shortopt), longopt(longopt), help(help), index(index) {}
    
    char shortopt;       ///< A single character short option
    std::string longopt; ///< A string used for the long option
    std::string help;    ///< A help string
    int index;           ///< The index into argv where the option appears
    StringStore arg;     ///< The argument following the option
  };
  
  /// Command-line argument options parser
  /// A simple parser for C++11
  ///
  /// Matches short options like "-h", "-v" and long options
  /// like "--help", "--verbose". Short options can be combined
  /// so "-hvv" is equivalent to "-h -v -v"
  ///
  /// Parsing stops when '--' is found
  ///
  /// Example 1
  /// ---------
  ///
  /// Simple use to parse arguments
  ///
  /// for (auto &opt : ArgOpts::Parser().parse(argc, argv)) {
  ///   if ((opt.shortopt == 'h') ||
  ///       (opt.longopt == "help")) {
  ///     std::cout << "Usage:\n" << argv[0] << " [options]\n";
  ///     std::cout << "Options:\n";
  ///     std::cout << "-h, --help		print help message\n";
  ///     std::cout << "-v, --verbose	print more\n";
  ///
  ///   } else if ((opt.shortopt == 'v') ||
  ///              (opt.longopt == "verbose")) {
  ///     std::cout << "Verbose\n";
  ///
  ///   }
  /// }
  ///
  /// Example 2
  /// ---------
  ///
  /// Here matches short and long options and handles
  /// printing of the option help
  ///
  /// ArgOpts::Parser args = { {'h', "help", "print help message"},
  ///                          {'v', "verbose", "print more"} };
  ///
  /// for (auto &opt: args.parse(argc, argv)) {
  ///   switch (opt.shortopt) {
  ///    case 'v': {
  ///       std::cout << "Verbose\n";
  ///       break;
  ///     }
  ///    case 'h': { // Print help message
  ///      std::cout << "Usage:\n" << argv[0] << " [options]\n";
  ///      std::cout << "Options:\n"
  ///                << args.printOptions() << "\n";
  ///      return 0;
  ///    }
  ///   }
  /// }
  class Parser {
  public:

    /// Empty constructor
    /// Options to be matched can be added after construction
    /// by calling the add() method
    ///
    /// Example
    /// -------
    ///
    /// Parser args; // empty constructor
    /// args.add('h', "help", "print help message");
    ///
    Parser() {}

    /// Constructor with list of options
    Parser(std::initializer_list<Option> options) : options(options) {}

    /// Add a command-line option to match
    ///
    /// Inputs
    /// ------
    ///
    /// @param[in] shortopt    A single character short name. Set to 0 for no
    /// short name
    /// @param[in] longopt     A longer name for the same option. Set to an empty
    /// string for no long name
    /// @param[in] help        A short help message, briefly describing the option
    ///
    void add(char shortopt, const std::string &longopt, const std::string &help) {
      options.push_back({shortopt, longopt, help});
    }

    /// Returns a formatted string, listing the known options
    std::string printOptions() {
      std::string result;

      for (auto &it : options) {
        if (it.shortopt != 0) {
          // Has a short option character
          result += "-" + std::string(1, it.shortopt);
          if (it.longopt.length() != 0) {
            // Both short and long option, so separate
            result += ", ";
          }
        }
        if (it.longopt.length() != 0) {
          // Has a long option name
          result += "--" + it.longopt;
        }
        result += "\t\t" + it.help + "\n";
      }
      return result;
    }

    using options_list = std::list<Option>;

    /// Looks for options in the given arguments,
    /// as passed to main(argc, argv)
    ///
    /// Inputs
    /// ------
    ///
    /// @param[in] argc  The number of arguments, including the command
    /// @param[in] argv  C array of strings, of length argc
    ///
    /// Returns
    /// -------
    ///
    /// A list of Option objects, in the order in which they
    /// appear in the arguments
    ///
    options_list parse(int argc, char **argv) {
      options_list options_found; // The returned list

      // Loop through argv, skipping index 0
      // since this is usually the command
      for (int i = 1; i < argc; i++) {

        if (argv[i][0] != '-') {
          // Skip anything without a '-' at the start
          continue;
        }
        if (argv[i][1] == 0) {
          // Just a '-'; ignore
          continue;
        }
        if (argv[i][1] == '-') {
          // Starts with '--'
          if (argv[i][2] == 0) {
            // Stop on a '--'
            break;
          }
          // A long option
          std::string found = std::string(&argv[i][2]);
          bool matched = false;
          for (auto &it : options) {
            if (it.longopt == found) {
              // Found this option
              Option option = it;
              option.index = i;
              options_found.push_back(option);
              matched = true;
              break;
            }
          }
          if (!matched) {
            // If not found, create a new option
            // Here the short option is set to zero
            options_found.push_back({0, found, "", i});
          }

          if (i != argc - 1) {
            // inputs still remaining. At this point we don't know if an argument
            // is expected for this option so use the next argv value
            options_found.back().arg = std::string(argv[i + 1]);
          }
        } else {
          // A short option. This consists of a single '-'
          // followed by one or more characters.
          // Each of these characters is a separate option

          int index = 1; // The character index, skipping '-'
          while (char c = argv[i][index]) {
            bool matched = false;
            for (auto &it : options) {
              if (it.shortopt == c) {
                // Found this option
                Option option = it;
                option.index = i;
                options_found.push_back(option);
                matched = true;
                break;
              }
            }
            if (!matched) {
              // If not found, create a new option
              // Here the long option is empty
              options_found.push_back({c, "", "", i});
            }

            if (i != argc - 1) {
              // inputs still remaining. At this point we don't know if an
              // argument
              // is expected for this option so use the next argv value
              options_found.back().arg = std::string(argv[i + 1]);
            }

            index++; // Next character
          }
        }
      }
      return options_found;
    }

  private:
    std::list<Option> options; ///< The options known about from construction or add() calls
  };

} // namespace ArgOpts;
