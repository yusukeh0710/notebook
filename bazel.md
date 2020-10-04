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
cc_library (
    name = "lib",
    srcs = ["myclass.cpp"],
    hdrs = ["myclass.h"],
)

cc_binary (
    name = "bin",
    srcs = ["main.cpp"],
    deps = [
        ":myclass",
    ],
)
```

```
# Specify files using wildcard.
cc_library (
    name = "lib2",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*,h"]),
)

# Speciy files using wildcard recursively.
cc_library (
    name = "lib3",
    srcs = glob(["**/*.cpp"]),
    hdrs = glob(["**/*.h"]),
)
```



# Workspace Rules

- Use local static library (.a) or shared library (.so)
  [WORKSPACE]

  ```
  new_local_repository (
      name = "static_library_rule",
      path = "<directory path>",
      build_file = "BUILD.static_library",
  )
  
  new_local_repository (
      name = "shared_library_rule",
      path = "<directory path>",
      build_file = "BUILD.shared_library",
  )
  ```

  [BUILD.static_library]

  ```
  cc_library (
      name = "static_library"
      srcs = glob(["*.a"]),
      hdrs = glob(["*.h"]),
      includes = ["./"],
      visibility = ["//visibility:public"],
  )
  ```

  [BUILD.shared_library]

  ```
  cc_library (
      name = "shared_library"
      srcs = glob(["*.so"]),
      hdrs = glob(["*.h"]),
      includes = ["./"],
      visibility = ["//visibility:public"],
  )
  ```

  [src/BUILD]

  ```
  cc_binary (
      name = "bin",
      srcs = ["main.cpp"],
      # hdrs = [],
      deps = [
          "@static_library_rule:static_library",
          "@shared_library_rule:shared_library",
      ]
  )
  ```



- Download archived file via http and build/link it
  [WORKSPACE]

  ```
  load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
  new_http_archive (
      name = "http_archive_rule"
      urls = "<archive file url>",
      sha256 = "<sha256 of the archive>"
      build_file = "BUILD.http_archive"
  )
```
  
