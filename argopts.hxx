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
#include <typeinfo>
#include <functional>
#include <stdexcept>
#include <memory> // For unique_ptr
#include <cctype> // for isdigit
#include <locale> // for wstring_convert
#include <codecvt> 

#include <iostream>

#ifdef __GNUG__ // gnu C++ compiler
  #include <cxxabi.h>
#endif

namespace ArgOpts {
#ifdef __GNUG__ // gnu C++ compiler

  
  /// This code from http://www.cplusplus.com/forum/beginner/175177/
  std::string demangle( const char* mangled_name ) {
    std::size_t len = 0 ;
    int status = 0 ;
    std::unique_ptr< char, decltype(&std::free) > ptr(
                                                      __cxxabiv1::__cxa_demangle( mangled_name, nullptr, &len, &status ),
                                                      &std::free ) ;
    return ptr.get() ;
  }
  
#else
  
  std::string demangle( const char* name ) { return name ; }
  
#endif // _GNUG_
  
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
    using ErrorHandler = std::function<void(const std::string&, const std::string&)>;
    
    StringStore(ErrorHandler handler = {}) : handler(std::move(handler)) {}
    StringStore(const std::string &value, ErrorHandler handler = {}) : value(value), handler(std::move(handler)) {}
    StringStore(const char *value, ErrorHandler handler = {}) : value(value), handler(std::move(handler)) {}

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
    /// double val = s.get<double>();
    ///
    template <typename T> T get() {
      if (value.length() == 0) {
        handleError(demangle(typeid(T).name()));
      }

      T t;
      std::stringstream ss(value);
      ss >> t;
      
      // Check if the parse failed
      if (ss.fail()) {
        handleError(demangle(typeid(T).name()));
      }
      // Check if there are characters remaining
      std::string remainder;
      std::getline(ss, remainder);
      for (const char &ch : remainder) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
          // Meaningful character not parsed
          handleError(demangle(typeid(T).name()));
        }
      }
      
      return t;
    }

  private:
    std::string value; ///< The internal data store
    ErrorHandler handler;

    /// This always throws an exception. The user-supplied
    /// handler handler may throw, but if not then std::invalid_argument is thrown.
    void handleError(const std::string &type_name) {
      if (handler) {
        handler(value, type_name);
      }
      throw std::invalid_argument("could not convert '" + value + "' to " + type_name);
    }
  };

  template<> std::string StringStore::get<std::string>() {
    if (value.length() == 0) {
      handleError("string");
    }
    return value;
  }
  
  /// Structure representing a command-line option
  ///
  ///
  struct Option {
    Option(char32_t shortopt, const std::string &longopt, const std::string &help, int index = -1)
      : shortopt(shortopt), longopt(longopt), help(help), index(index) {}
    
    char32_t shortopt;       ///< A single character short option
    std::string longopt; ///< A string used for the long option
    std::string help;    ///< A help string
    int index;           ///< The index into argv where the option appears
    StringStore arg;     ///< The argument following the option

    /// Returns a text containing the command-line option and help message
    /// No newline at the end
    std::string usage() const {
      std::string result;

      if (shortopt != 0) {
        // Has a short option character
        result += "-" + std::string(1, shortopt);
        if (longopt.length() != 0) {
          // Both short and long option, so separate
          result += ", ";
        }
      }
      if (longopt.length() != 0) {
        // Has a long option name
        result += "--" + longopt;
      }
      if (help.length() != 0) {
        result += "\t\t" + help;
      }
      return result;
    }
  };

  /// A functor which is called by StringStore
  /// when an error occurs. By storing a reference
  /// to the Option object, a useful error message
  /// can be put into an exception.
  struct OptionErrorHandler {
    OptionErrorHandler(const Option &option) : option(option) {}
    
    void operator()(const std::string &value, const std::string &type_name) {
      if (value.length() == 0) {
        // Missing value
        std::string message = "Missing argument, expected type " + type_name + "\n"
          "usage: "+ option.usage() + "\n";
        
        throw std::invalid_argument(message);
      }

      // Incorrect type
      std::string message = "Invalid argument: expected type " + type_name +
        " but got '"+ value + "'\n"
        "usage: "+ option.usage() + "\n";
      
      throw std::invalid_argument(message);
    }
  private:
    const Option &option;
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
    void add(char32_t shortopt, const std::string &longopt, const std::string &help) {
      options.push_back({shortopt, longopt, help});
    }

    /// Returns a formatted string, listing the known options
    std::string printOptions() {
      std::string result;

      for (auto &it : options) {
        result += it.usage() + "\n";
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
        if ( std::isdigit(static_cast<unsigned char>(argv[i][1])) ) {
          // A digit 0-9. Leading '-' is probably part of a number, so ignore
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

          std::string argvalue;  // The next entry in argv, empty if none
          if (i != argc - 1) {
            // inputs still remaining. At this point we don't know if an argument
            // is expected for this option so use the next argv value
            argvalue = std::string(argv[i + 1]);
          }
          // Create an error handler object which is called if there is a conversion error
          Option &option = options_found.back();
          option.arg = StringStore(argvalue, OptionErrorHandler(option));
        } else {
          // A short option. This consists of a single '-'
          // followed by one or more characters.
          // Each of these characters is a separate option

          // Convert from utf8 to utf32, skipping '-'
          std::u32string utf32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(&argv[i][1]);
          // Iterate over code units
          for(char32_t c : utf32) {

            bool matched = false; // matched known option
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

            std::string argvalue;
            if (i != argc - 1) {
              // inputs still remaining. At this point we don't know if an
              // argument
              // is expected for this option so use the next argv value
              argvalue = std::string(argv[i + 1]);
            }
            // Create an error handler object which is called if there is a conversion error
            Option &option = options_found.back();
            option.arg = StringStore(argvalue, OptionErrorHandler(option));
          }
        }
      }
      return options_found;
    }

  private:
    std::list<Option> options; ///< The options known about from construction or add() calls
  };

} // namespace ArgOpts;
