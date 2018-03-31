## g++ usage
1 basic

```$ g++ -o out main.cc test.cc```

2 compile each module and compile with their modules integratedly

<pre>
$ g++ -c -o test test.cc
$ g++ -o out test main.cc
</pre>

3 compile modules in other directory

[directory configuration]
<pre>
+ -- app -- + -- main.cc
|
+ -- test -- + -- test.h
             + -- test.cc
</pre>
[operation]
<pre>
$ cd test
$ g++ -c -o test test.cc
$ cd ..
$ g++ -o main test/test app/main.cc -I./test
</pre>

4 compile with static library

[directory configuration]
<pre>
+ -- app -- + -- main.cc
|
+ -- test -- + -- test1.h
             + -- test2.h
             + -- test1.cc
             + -- test2.cc
</pre>
[operation]
<pre>
$ cd test
$ g++ -c test1.cc test2.cc
$ ar rcs libtest.a test1.o test2.o
$ cd ..
$ g++ -o main app/main.cc -I./test -L./test -ltest
</pre>
[Note]
- r: Insert or replace the specified object files into specified archive
- c: Create the archive, suppress warning message about 'creating archive file'
- s: Write an object-file index into the archive ( = ranib command ) 

5 compile with shared object
<pre>
$ cd test
$ g++ -shared -fPIC -o libtest.so test1.cc test2.cc
$ cd ../
$ g++ -o main app/main.cc -I./test -L./test -ltest
$ LD_LIBRARY_PATH=~/xx/test
$ ./main
</pre>

6 compile with shared object while inserting .so path into binary
<pre>
$ g++ -o main app/main.cc -I./test -L./test -ltest -Wl,-rpath=./test
$ ./main
  -> ok
$ mkdir aaa; mv main aaa/; cd aaa
$ ./main
  -> error while loading shared libraries: libtest.so: cannot open shared object file: No such file or directory
</pre>

other options
- -w: suppress all warning message
- -Wall: warn for all non-recommended descriptions
- -Werror: treat warning as error
- --std=c++11, --std=c++14, --std=c++17: enable c++11/14/17 feature
- O1, O2, O3: optimize for compiled object (non-optimized < O1 < O2 < O3)
