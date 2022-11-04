# Boost::ProgramOptions

### Setup / Compile

```
$ sudo apt install -y libboost-all-dev
```

```
$ g++ xxx.cpp -lboost_program_options
```

### Getting Started

```c++
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc,  char** argv) {
  int v1, v2;
  po::options_description options("Option");
  options.add_options()
    ("help,h", "Print help")
    ("v1", po::value<int>(&v1), "arg1")
    ("v2", po::value<int>(&v2), "arg2")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, options), vm);
  try {
    po::notify(vm);
  } catch (const boost::program_options::error_with_option_name& e) {
    std::cout << e.what() << std::endl;
  }

  if (vm.count("help")) {
    std::cout << options << std::endl;
    return 1;
  }
}
```

- po::store(): parse 'argc' / 'argv' and set 'vm' to the result
- po::notify(): set 'v1'/'v2' to stored values in 'vm'



### Argument variation

- Default value

  ```c++
  options.add_options()
    ("v1", po::value<int>(&v1)->default_value(5), "arg1");
  ```

- Required

  ```c++
  options.add_options()
    ("v1", po::value<int>(&v1)->required(), "arg1");
  ```

- Positional argument :
  positional_options_description & add(const char * name, int max_count)

  ```
  po::positional_options_description p;
  p.add("v1", 1);
  p.add("v2", 2);
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(options).positional(p).run(), vm);
  po::notify(vm);
  ```

### Config 

- File format

  ```
  [section]
  v1 = 1
  v2 = 2
  string = True
  # comment
  ```

- Code

  ```
  std::ifstream ifs("test.conf");
  po::variables_map vm;
  po::store(po::parse_config_file(ifs, options), vm);
  po::notify(vm);
  ```