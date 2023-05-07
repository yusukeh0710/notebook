# STL tips

## Range operations

#### max_element / min_element

* Return a iterator for max/min element in the STL container

```c++
auto it = std::max_element(v.begin(), v.end());
cout << *it << "(index: " << std::distance(v.begin(), it) << ")" << endl;
```



#### accumulate

* Return sum of the STL container
* accumulate(begin, end, offset) = sum(begin to end) + offset

```c++
cout << std::accumulate(v.begin(), v.end(), offset_value) << endl;
```



#### count / count_if

* Count the items satisfying the condition

```c++
cout << std::count(v.begin(), v.end(), value) << endl;
cout << std::count_if(v.begin(), v.end(), [](int x) { return /* if condition */; }) << endl;

e.g. 
cout << std::count_if(v.begin(), v.end(), [](int x) { return (v%2)==0; }) << endl;
```



#### all_of / any_of

* Return true/false if all/any of values in the STL container satisfy the condition

```c++
cout << std::all_of(v.begin(), v.end(), [](int x) { return /* if condition */ }) << endl;
cout << std::any_of(v.begin(), v.end(), [](int x) { return /* if condition */ }) << endl;
```



#### fill

* Update all items in the STL container to the specified value

```
std::fill(v.begin(), v.end(), value);
```



#### rotate

* Rotate items in the STL container by ```std::swap```

```
std::rotate(v.begin(), v.begin()+index, v.end());
-> {v[index], v[index+1], .., v[0], v[1], ..., v[index-1]}
```



#### transform

* Do some transformation for the STL container

```
STL dst(src.size());
std::transform(src.begin(), src.end(), dst.begin(), [](value) { return /* transformation */ });

e.g.
std::transform(src.begin(), src.end(), dst.begin(), [](int v) { return v * 2; });
```

