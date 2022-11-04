# boost::bimap

### Getting Started

```c++
#include <iostream>
#include <string>

#include <boost/bimap.hpp>

int main() {
  // using MapTpe = std::map<int, std::string>;
  using BimapType = boost::bimap<int, std::string>;
  using BimapValue = BimapType::value_type;

  BimapType bm;
  bm.insert(BimapValue(1, "one"));
  bm.insert(BimapValue(2, "two"));
  bm.insert(BimapValue(3, "three"));

  // int -> string (left lookup)
  for (const int key : {1, 2, 3, 4, 5}) {
    if (bm.left.find(key) != bm.left.end()) {
      std::cout << bm.left.at(key) << ", ";
    } else {
      std::cout << "N/A, ";
    }
  }
  std::cout << std::endl;

  // string -> int (right lookup)
  for (const auto& key : {"one", "two", "three", "four", "five"}) {
    if (bm.right.find(key) != bm.right.end()) {
      std::cout << bm.right.at(key) << ", ";
    } else {
      std::cout << "N/A, ";
    }
  }
  std::cout << std::endl;
}
```



### Initialize

1. The way to use boost::assign
   ```
   #include <boost/assign.hpp>
   
   BimapType bm = boost::assign::list_of<BimapType::relation>
       ("one", 1)("two", 2)("three", 3);
   ```

   

2. The way to use value_type vector
   ```
   vector<BimapType::value_type> v{{"one", 1}, {"two", 2}, {"three", 3}};
   BimapType bm(v.begin(), v.end());
   ```




### Convert map to bimap

```
MapType m = {{1, "one"}, {2, "two"}, {3, "three"}};
BimapType bm;
std::transform(m.begin(), m.end(), std::inserter(bm.left, bm.left.end()),
    [](const auto& x){ return std::make_pair(x.first, x.second);});
```

