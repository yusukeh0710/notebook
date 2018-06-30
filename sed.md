## 
## Substitute
- syntax

```sed -e "s/regular expression/replacement/flags"```

- example
<pre>
sed -e "s/abc/efg/"   # replace 'abc' with 'efg'
sed -e "s/^abc/efg/"  # replace the string 'abc' at the beginning of line
sed -e "s/abc\$/efg/" # replace the string 'abc' at the end of line
sed -e "s/^/abc/"     # add 'abc' at the beginning of line
sed -e "s/\$/abc/"    # add 'abc' at the end of line
sed -e "s/abc//"      # replace 'abc' with empty string (= delete 'abc')
</pre>

## Delete
- syntax

```sed -e "/cond/d"```

- example
<pre>
sed -e '1d' file      # delete 1st line
sed -e '1,4d' file    # delete 1st - 4th lines
sed -e '$d' file      # delete last line
sed -e '/abc/d' file  # delete the line containing string 'abc'
sed -e '/^abc/d' file # delete the line containing string 'abc' at the beginning of line
sed -e '/^$/d' file   # delete empty line
sed -e '/^#/d' file   # delete comment line in bash script and so on
</pre>

- other solution
-- delete last line
 $ head -n -1 file
-- delete last 4 lines
 $ head -n -4 file
