## Comand list
#### Run operation
- run
- break xx : create breakpoint
  - break main      : create breakpoint at main()
  - break file.c:30 : create breakpoint at line #30 in file.c
- next : next line
- step : next step (if next line is function, move there)
- delete : delete breakpoint

#### Print operation
- list : print previous/later lines
- print variable : print value stored in the variable (e.g. print i -> $x = 4)
-- print *(int [4]*)array : print 4-elements in array (array[0], array[1], .., array[3])
- whatis variable : print type of the value

## CGDB
vim-like interface GDB

| Command | Description |
----|---- 
| Escape | Go to source code |
| i | Go to gdb console |
| F5 | run |
| F6 | contnue |
| space | create breakpoint |
| F8 | next |
| F10 | step |

#### Open file mode
press 'o' to open other file

| Command | Description |
----|---- 
| q | return regular window |
| / | search from current cursor position |
| ? | reverse search |
| n | next forward search |
| N | next reverse seach |
