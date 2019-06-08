## Install

Read her:

https://docs.bazel.build/versions/master/install-ubuntu.html



## Usage

#### Examples of Directory Configuration

```
Example1
    xxx -+- WORKSPACE
         +- src --+
                  + -- BUILD
                  + -- main.cpp
                  + -- myclass.h
                  + -- myclass.cpp

Example2
    xxx -+- WORKSPACE
         +- src --+
                  + -- moduleA -- + -- BUILD
                  |               + -- moduleA.h
                  |               + -- moduleA.cpp
                  |
                  + -- moduleB    + -- BUILD                                   ..
                       ....
```



#### Command

@src directory or deeper directory

```
 $ bazel //...  -> build all files
 $ bazel //moduleA/... -> build files in directory moduleA
 $ bazel //moduleA:xxx -> build files related app xxx in directory moduleA
```



#### Output

```
xxx -+- WORKSPACE
    -+- src ...
    -+- bazel-bin -- src -- OUTPUT files
    ....
```



## BUILD file Syntax

#### template (for above example1)

```
cc_library(
    name = "myclass",
    srcs = ["myclass.cpp"],
    hdrs = ["myclass.h"],
)

cc_binary(
    name = "my-test",
    srcs = ["main.cpp"],
    deps = [
        ":myclass",
    ],
)
```