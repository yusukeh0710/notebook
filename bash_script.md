# Bashスクリプト入門

## シェルスクリプトとは？

シェルに読み込ませて実行するコマンドと、ループや条件文などの制御構文とを書き並べたプログラムファイルをシェルスクリプトと呼ぶ。シェルにはsh, bash, csh, zshなど様々な種類があるが、本稿ではbashを使ったスクリプトについて説明する。

以下に、bashスクリプトの例とその実行手順を示す。シェルスクリプトでは上から順にコマンドが実行され、その結果を表示したりやファイルへ出力したりする。以下の例では、まず最初に```echo``` コマンドで文字列を出力し、続いて```ls``` と```pwd``` が実行される。

[bashスクリプトの例：sample.sh]

```bash
#!/bin/bash
echo "This is sample."
ls
pwd
```

[シェルスクリプトの実行手順]

```
chmod +x sample.sh   # スクリプトの実行権限を付与
./sample.sh          # スクリプトを実行
```



## 基本構文

#### シェバン (shebang)

bashスクリプトでは```#!/bin/bash``` を先頭行に記述する。シェバンと呼ばれるもので、「スクリプトをどの処理系で実行するか」を意味する。例えば、```#!/bin/bash``` ならbashで処理することを示し、```#!/usr/bin/python3``` ならpython3で処理することを示している。

#### コメント

コメントを書くときには「＃」を使用する。以下の２つの書き方が可能である。

1. 行頭に「#」を書いて１行全部をコメントとする
2. 行の途中に「＃」を書いて＃以降をコメントとする

```
# １行全部を使ってコメントを記述
ls

ls  # 途中からコメントを記述

# 複数行でコメントを書く場合は、
# このように各行の先頭に「＃」を書く
```

※本稿では説明のため日本語でコメントを書くが、文字化けの原因となりうるため、英語でコメントを書くことを推奨する

#### 終了ステータス 

「exit 数値」でスクリプトの終了ステータスを返すことができ、「$?」でその終了ステータスを参照することができる。
「0」は成功を意味し、それ以外の数値は何かしらの失敗が生じたことを示す。コマンド実行時の出力ではなく、実行成否を示すステータスである点に注意。
※c言語やJavaのbool変数(true=1, false=0)と真逆の割付になっているので要注意

[success.sh]

```
#!/bin/bash
echo "Success"
exit 0
```

[failure.sh]

```
#!/bin/bash
echo "Failure"
exit 22
```

[main.sh]

```
#!/bin/bash
./success.sh
echo $?        # 成功を示す「０」を出力

./failure.sh
echo $?　　　　 # 失敗を示す「２２」を出力

./failure.sh
echo "dummy"
echo $?        # failure.shではなく 「echo "dummy"」の終了ステータス0を出力
```

[実行手順]

```
chmod +x success.sh failure.sh main.sh
./main.sh
```

#### 変数

bashスクリプトでは、スクリプト中で定義した```シェル変数```、Linux環境内で定義された```環境変数```、bashで予約されている```特殊変数```を変数として使用できる。

###### 代入と参照

- 代入： ```変数名=値```　※「=」の前後にスペースをいれるとエラーするので注意
- 参照：```${変数名}```
- 環境変数として定義：```export 変数名=値```(```var=value; export ${var}```と同値)
- 読み取り専用の変数として定義：```readonly 変数名=値``` (```var=value; readonly ${var}```と同値)

※環境変数として定義すると、他のスクリプトでその変数を使用することができる。ただし、バグの温床となりやすいため基本非推奨で、後述のコマンドライン引数を使用して受け渡しする方がよい。

(実装例)

[main.sh]

```
#!/bin/bash
var1="aaaa"　　　　　　　#シェル変数
export var2="bbbb"     #環境変数
readonly var3="RO"     #読み取り専用

echo "${var1} @main.sh"
echo "${var2} @main.sh"
echo "${var3} @main.sh"
./local.sh
```

[local.sh]

```
#!/bin/bash
echo "${var1} @local.sh"   #local.shで宣言されてない変数なので空文字が表示される
echo "${var2} @local.sh"   #var2は環境変数なので、こっちは表示される
```

[実行手順]

```
chmod +x main.sh local.sh
./main.sh
```

[補足]

変数の参照は以下のように、{}をつけなくても可能だが、意図せぬバグの原因となりうるため、２文字以上の変数は{}を付けることを推奨する。\$iや\$jのように1文字の変数はつけなくても大丈夫(..だと思う)

```
#!/bin/bash
var="abcd"

# どっちでも参照はできるが、${var}と${va}のように類似の変数がいるときに意図せぬ動きとなるのを避けるため、
# ２文字以上は{}をつけた方が無難
# 例えば、 echo "${va}r" (=変数+'r')を意図して echo "$var"と書いたのに、${var}の方の値が使用される
echo "${var} $var"

# 1文字変数は面倒であれば{}を付けなくても構わないが、上記の意図せぬ誤用は注意すること
i=3
echo "$i"
```



###### 特殊変数

- ```$0```: 実行したスクリプトファイルのパス
- ```$1, $2, ..```: スクリプト実行時の引数
- ```$@```: 全ての引数
- ```$#```:引数の個数
- ```$?```: 直前のコマンドの終了ステータス

(例)

[main.sh]

```
#!/bin/bash
echo $0
echo $1
echo $2
echo $@
echo $#
```

[実行手順]

```
chmod +x main.sh
./main.sh a b c
⇒ $0: ./main.sh, $1: a, $2: b, $@: a b c, $#: 4
```



###### コマンド置換

バッククォート「\`」や「$()」を使用し、コマンドの出力を変数へ代入したり、別のコマンドへの入力にしたりすることができる。
前者はシンプルに書けて、後者はコマンドのネストが可能なので、「実行コマンドは１つ⇒バッククォートで挟む、複数のコマンドをネスト⇒```$()```で挟む」と使い分けるとよい。

```
変数=`コマンド` ※バッククォートでコマンドを挟む
変数=$(コマンド)
```

(例)

```
x=`pwd`
y=$(basename $(pwd))
z=$(basename $0)

echo $x   # 現在いるディレクトリのパスを出力
echo $y   # 現在いるディレクトリの名前を出力
echo $z　 # 実行したスクリプトのファイル名を出力

# 別のコマンドの入力として使用
basename `pwd`
basename $(pwd)
```

#### コマンドの改行

コマンドが長文で途中で改行したい場合は```\ (バックスラッシュ もしくは　￥マーク)```を行末につけることで複数行に分けて記述することができる。ただし、変数を使って記述するなど、書き方自体に改善が必要なケースが多く、まずはそうした短縮化をする方が望ましい。(長いコマンドはバグ埋め込みの原因となりやすいので)

```
echo "my_command " \
     "-H 'application/json' -H 'HTML5.0'" \
     "http://mypage.super_long_url_web_page.html"
     
# 例えば、こんな感じに変数を使ってシンプルにできる
option="-H 'application/json' -H 'HTML5.0'"
url="http://mypage.super_long_url_web_page.html"
echo "my_command ${option} ${url}"
```



## 入出力

Linuxでは下図に示すように、標準入力(#0, stdin)を介してキーボードやファイル等からの入力を受けとり、標準出力(#1, stdout)/標準エラー出力(#2, stderr)を介して端末コンソールやファイル等へ実行結果を出力する。

<img src="figure/stdin_stdout_stderr.png" width="700" />

#### リダイレクト

```<```や```>```を使用してデータの入力先や出力先をデフォルト以外に変更することができ、これをリダイレクトと呼ぶ。

- ```コマンド<ファイル``` ファイルの中身をコマンドの入力として使用
- ```コマンド>ファイル``` 標準出力をファイルへ保存
- ```コマンド2>ファイル```標準エラー出力をファイルへ保存
- ```コマンド &>ファイル``` 標準出力と標準エラー出力の両方をファイルへ保存
- ```コマンド>>ファイル``` 標準出力をファイルへ追記 (>が上書きに対して、>>が追記)
-  ```コマンド 2>&1```標準エラー出力を標準出力へリダイレクト
- ```コマンド 2>/dev/null``` 標準エラー出力を空ファイル(```/dev/null```)へ捨てる (=標準出力だけ表示)
  ```コマンド >/dev/null``` 標準出力を捨てる(=標準エラー出力だけ表示)

(例)

```
pwd >stdout.txt 2>stderr.txt    # stdout.txtには現ディレクトリパス、stderr.txtには空文字が保存される
pwd -x >stdout.txt 2>stderr.txt # -xというオプションはなくエラーとなり
                                # stdout.txtには空文字、stderr.txtにはエラーメッセージが保存される
pwd >/dev/null     # 標準出力を捨てて標準エラーだけ出力 (=何も表示されない)
pwd -x 2>/dev/null # 標準エラー出力を捨てて標準出力だけ出力 (同上)

pwd -x >stdout.txt 2>&1 # エラーメッセージを標準出力へリダイレクトして、標準出力と一緒にstdout.txtへ保存

ls > a.txt
ls >> a.txt  # ファイルリスト x 2 を保存
```

#### ヒアドキュメント

文書ファイルの生成など、複数行からなる出力を行う必要がある場合、ヒアドキュメントというリダイレクト機能を使用することで簡易に実装することが可能である。
[書式]

```
コマンド << 終了キーワード(e.g. EOS(End of sentence), EOF(End of file))
  この部分に
  中身のテキストを記述する
終了キーワード
```

(例)

```
cat << EOS > file.txt
This is test.
This is test.
Is this test?
EOS
```

※上記例では、ヒアドキュメント内の文字列群が標準入力を介してcatコマンドへ入力され、catの出力がfile.txtへ書き込まれる

#### パイプライン (パイプ)

パイプ(```|```) を使うことで、コマンドの標準出力を次のコマンドの標準入力へ渡すことができる。これにより、複数のコマンドをパイプで区切って並べることで、コマンドの実行結果を次のコマンドへ渡して連続的に処理することができる。

(例)

```
ls -l /usr/ | grep bin
```



## 算術演算

```
a=5
b=4

echo "a+b = $((a+b))"
echo "a-b = $((a-b))"
echo "a*b = $((a*b))"
echo "a/b = $((a/b)) reminder $((a%b))"

((a++))   # a = a + 1 (インクリメント)
echo $a

((a--))   # a = a - 1 (デクリメント)
echo $a

((a+=2))  # a = a + 2
echo $a
```

## 制御構文

#### if

[書式]

```bash
# パターン1
if [ 条件式 ]; then
  # 条件式が成立するとき(Trueのとき)
fi

# パターン2
if [ 条件式 ]; then
  # 条件式が成立するとき(Trueのとき)
else
  # 条件式が不成立となるとき(Falseのとき)
fi

# パターン3
if [ 条件式A ]; then
  # 条件式Aが成立するとき
elif [ 条件式B ]; then
  # 条件式Bが成立するとき
else
  # それ以外
fi
```

※```[```と条件式の間にスペース１つ、条件式と```]```の間にスペースを1つ入れる必要あり

[数値比較]

- 数値1 -eq 数値２ (数値1$=$ 数値2 ?) ※eq: equal
- 数値1 -gt 数値2 (数値1 $>$ 数値2 ?)  ※gt: greater than
- 数値1 -ge 数値2 (数値1 $\geq$ 数値2 ?)  ※ge: greater than or equal
- 数値1 -lt 数値2 (数値1 $<$ 数値2 ?)  ※lt: less than
- 数値1 -le 数値2 (数値1 $\leq$ 数値2 ?)  ※le: less than or equal

(例)

```
i=5

if [ $i -eq 5 ]; then
  echo "i = 5"
else
  echo "i is not 5"
fi

if [ $i -gt 5 ]; then
  echo "i > 5"
else
  echo "i <= 5"
fi

if [ $i -ge 5 ]; then
  echo "i >= 5"
else
  echo "i < 5"
fi
```

[文字列比較]

- 文字列1 == 文字列2 (文字列1と文字列2は一致？)
- -n 文字列 (文字列は空文字ではない？)
- -z 文字列 (文字列は空文字？)

(例)

```
str="abcde"
str_empty=""

if [[ "${str}" == "abcde" ]]; then
  echo "str is abcde"
else
  echo "str is not abcde"
fi

if [ -n "${str}" ]; then
  echo "not empty"
fi

if [ -z "${str_empty}"]; then
  echo "empty"
fi

# 以下のようにダブルクォーテーションをつけず、変数が空文字だったときに文法エラーになるので
# if文で文字列変数を扱うときは必ずダブルクォーテーションで囲む
if [ ${str_empty} == "abcde" ]; then
  echo "aaaaa"
fi
```

[ファイル/ディレクトリの存在判定]

- -e PATH: 有効なファイルパス？
- -f PATH：ファイルは存在する？
- -d PATH：ディレクトリは存在する？
- -L PATH：リンクファイルは存在する？

(例)

```
home_dir=~/

if [ -e "${home_dir}" ]; then
  echo "Valid path"
fi

if [ -f "${home_dir}" ]; then
  echo "This is a file"
else
  echo "This is not a file"
fi

if [ -d "${home_dir}" ]; then
  echo "This is a directory"
fi
```

[否定文]

```
if [ ! 条件式 ]; then
  # 条件式が不成立となるとき(Falseのとき)
fi
```

(例)

```
i=5

if [ ! $i -gt 6 ]; then
  echo "i < 6"
fi

str="abcde"
if [ ! -z "${str}"]; then
  echo "str is not empty"
fi

home_dir=~/
if [ ! -f "${home_dir}" ]; then
  echo "home_dir is not file"
fi
```

[AND/OR]

AND条件

```
# パターン1
if [ 条件式1 ] && [ 条件式2 ]; then
  # do something
fi

# パターン2
if [[ 条件式1 && 条件式2 ]]; then
  # do something
fi
```

OR条件

```
# パターン1
if [ 条件式1 ] || [ 条件式2 ]; then
  # do something
fi

# パターン2
if [[ 条件式1 || 条件式2 ]]; then
  # do something
fi
```

[その他]

- 終了ステータスを使った条件分岐

  ```
  ls /usr/ -l | grep bin
  if [ $? -eq 0 ]; then
    echo "bin directory is found."
  fi
  
  ls /usr/ -l | grep xxx
  if [ $? -ne 0 ]; then
    echo "xxx directory is not found."
  fi
  ```

#### case (switch文)

[書式]

```
case 値 in
  パターン)
    処理
    ;;
  パターン)
    処理
    ;;
  *)
    処理
    ;;
esac
```

(例)

```
err_code=1

case ${err_code} in
  0)
    echo "success"
    ;;
  1)
    echo "invalid input"
    ;;
  2)
    echo "out range input"
    ;;
  *)
    echo "unknown error"
    ;;
esac
```

#### for

[書式1: ```for ((初期値; 終了条件; 更新処理))```] 

````
# i=0で開始して、ループの度に値を1増加させて、5となったらループ終了
for ((i=0; i<5; i++)); do
  echo $i
done

# ループのたびに値を2増加させて、0->2->4->6と変化し、6となったタイミングでi>5となり終了
for ((i=0; i<5; i+=2)); do
  echo $i
done
````

[書式2: ```for 変数 in 値のリスト``` ]

```
# リストの値を順に出力
for i in 1 2 3 7 9; do
  echo $i
done
```

#### while

```
while 条件式; do
  処理
done
```

(例)

```
i=0
while [ $i -gt 4 ]; do
  echo $i
  ((i++))
done
```

## 配列 

[書式]

- 初期化: ```配列変数名=(値1 値2 ...)```
- 参照: ```${配列変数名[添字]}```
- 全要素を参照: ```${配列変数名[@]}```
- 代入: ```配列変数名[添字]=値```

(例)

```
array=("a" "b" "c" "d" "e")
echo ${array[@]}
# 添字で参照するループを書くならこう
for ((i=0; i<5; i++)); do
  echo "$i, ${array[$i]}"
done

# 順に要素に参照したいだけなら、これでもOK
for item in ${array[@]}; do
  echo ${item}
done

array[3]="z"
echo ${array[@]}
```

(例2)

```
# コマンドの実行結果も同じようにリストへ格納可能
file_list=(`ls`)
echo ${file_list[@]}

# 区切り文字を空白以外にする場合は、環境変数IFSに値を代入
IFS_BK=${IFS}
IFS='/'

dirnames=(`pwd`)
echo ${dirnames[@]}

IFS=${IFS_BK} # 処理完了したら、忘れずIFSの値を元に戻す
```



## 関数

ひとまとまりの処理群を関数として定義することができる。高度な処理になるほど、コード量が多く複雑となり、可読性が低下していくことになる。その場合、ある程度の処理の塊をそれぞれ関数として定義することで可読性を改善し、メンテナンス性を向上・バグの埋め込む可能性を低下させることにつながる。

[書式]

```
関数名() {
  処理内容
  return 戻り値
}
```

- 引数：　スクリプトの実行時引数と同様 \$1, \$2, ... で取得
- 戻り値(=終了ステータス): return + 数値 で返却

(例)

```
myfunc() {
  echo $1
  echo $2
  return 3
}

myfunc "a" "b"
echo "return: $?"

myfunc "$@"  # $@を使ってスクリプトの引数全部を渡すことも可能
```

(例2) 文字列を生成する関数を実装し、その文字列を取得するときは、コマンド置換を利用する

```
myfunc() {
  name=$1
  echo "My name is ${name}."
}

greeting=`myfunc "Taro"`
echo ${greeting}
```



## その他の便利機能

#### パラメータファイル、ライブラリファイルのインポート

```source```コマンドを使用することで、「１．指定ファイル内のシェル変数＋関数をインポート、２．指定ファイルの実行」を行う。
複数のスクリプトで共通して使用するパラメータや関数がある場合は、共通部分を１つのファイルに集約してsourceで取り込むような実装が可能である。

[util.sh]

```
readonly PARAM1=3  # 意図せぬ変数更新を避ける意味で、ライブラリ中のパラメータはreadonlyを付けておくと無難
readonly PARAM2=5

library_func() {
  echo "Library output $*"
}

# このようにsourceで指定すると関数化してない部分は自動的に実行されてしまうので、
# globalなreadonly変数と関数以外はライブラリファイルには書かないようにした方がよい。
echo "This is unexpected call @ util.sh"
```

[main.sh]

```
#!/bin/bash
source util.sh  # import util.sh

echo ${PARAM1}, ${PARAM2}
library_func test test hoge
```



#### コマンドライン引数の解析

スクリプトの実行時引数が必須引数のみなら、以下のように実装できる。

```
arg1=$1
arg2=$2
if [ -z "${arg1}" ] || [ -z "${arg2}" ]; then
  echo "Usage: $0 ARG1 ARG2"
  exit 1
fi

# Main process using arg1/arg2
...
```

一方、オプション引数がある場合、引数の解析を行い、オプションに応じて処理を変更する実装が必要となる。
この場合、```getopts```を使うと簡単に実装することができる。

[書式]

```
while getopts オプション文字列 変数; do
  case ${変数} in
     ...
     *)
       # 該当なしのオプションが入力された場合
       ;;
  esac
done
shift $((OPTIND - 1))  # オプション部分を削除
```

- オプション文字列:
  - a ⇒ ```-a``` というオプション
  - a: ⇒ ```-a 値```というオプション
  - (例) ```abc:d:``` = ```[-a] [-b] [-c value_c] [-d value_d]``` と同値
- 引数で指定した値の取得:```${OPTARG}``` を使用する

(例) main.sh

```
#!/bin/bash
use_a=false
use_b=false
value_c="empty"
value_d="empty"

while getopts "abc:d:" OPT; do
  case ${OPT} in
    a) use_a=true;;
    b) use_b=true;;
    c) value_c=${OPTARG};;
    d) value_d=${OPTARG};;
    *) 
      echo "Invalid Option: ${OPT}"
      exit 1
      ;;
  esac
done
shift $((OPTIND - 1))

echo "option args: ${use_a} ${use_b} ${value_c} ${value_d}"
echo "potisional args: $@"
```

(実行例)

```
chmod +x main.sh
./main.sh -b -c xx arg1 arg2
```

#### 文字列の処理

###### 正規表現を使ったパターンマッチ

特定パターンの文字列を検索するときに、正規表現を使用して検索することができる。ルールは以下のとおりである。

- ```[]``` : 指定文字のどれか1つと一致
  - [abc] : aかbかcと一致
  - [a-d] : a,b,c,dと一致
- ```.```(ピリオド): 任意の1文字
- ```?```: 0回 / 1回の繰り返し
- ```*```:0回以上の繰り返し
- ```+```:1回以上の繰り返し
- ```^```:行の先頭に一致
- ```$```:行の末尾に一致

[if文での正規表現を使った判定]

```
if [[ 変数 =~ 正規表現 ]]; then
  # do something
fi
```

※通常の条件文と異なり```[``` ```]```をそれぞれ2つ使用する点に注意

(例)

```
a="test"
b="123test"
c="test123"
d="Test"

# a-z のみで構成された文字列ならTrue
if [[ "$a" =~ ^[a-z]+$ ]]; then
  echo "var_a is consisted with only alphabets."
fi

# 4文字の文字列ならTrue
if [[ "$a" =~ ^....$ ]]; then
  echo "var_a is xxxx string."
fi

# a-z以外の文字列が含まれるならTrue
if [[ ! "$b" =~ ^[a-z]+$ ]]; then
  echo "var_b is NOT consisted with only alphabets."
fi

# 数字列から始まるならTrue
if [[ "$b" =~ ^[1-9]+.*$ ]]; then
  echo "var_b starts with digits."
fi

# 数字列で終わるならTrue
if [[ "$c" =~ .*[1-9]+$ ]]; then
  echo "var_c ends with digits."
fi

# 小文字・大文字問わずで判定するなら、こう書く
if [[ "$d" =~ ^[a-zA-Z]+$ ]]; then
  echo "var_d consists with only alphabets."
fi
```

###### 文字列操作

- 切り出し：```${変数名:開始位置:長さ}```
- 置換: 
  - ```${変数名/パターン/置換後の文字列}``` 最初に一致した部分を置換
  - ```${変数名//パターン/置換後の文字列}``` 一致した全文字列を置換
- 削除:
  - ```${変数名#パターン}``` １文字目から最短のパターン一致した分を削除
  - ```${変数名##パターン}``` 　1文字目から最長のパターン一致した分を削除 ※拡張子の切り出しに便利
  - ```${変数名%パターン}``` 末尾から最短のパターン一致した分を削除
  - ```${変数名%%パターン}``` 末尾から最長のパターン一致した分を削除　※拡張子の削除に便利
- パターンに使用される正規表現(前述のifでの正規表現とルールがやや異なるので注意)
  - ```*``` 任意の文字列の繰り返し

(例)

```
str="abcAdAabc"
echo ${str:2:4}      # => cAdA
echo ${str/abc/xxx}  # => xxxAdAabc
echo ${str//abc/xxx} # => xxxAdAxxx

str="test.tar.gz"
echo ${str#*.}      # => tar.gz
echo ${str##*.}     # => gz
echo ${str%.*}      # => test.tar
echo ${str%%.*}     # => test

echo ${str#*tar}    # => .gz
echo ${str#tar*}    # => test.tar.gz
```



###### シングルクォート、ダブルクォート、エスケープ

- シングルクォートで囲った文字列：
  ```${}```の部分は変数展開されず、ただの文字列として処理される
- ダブルクォートで囲った文字列：
  ```${}```の部分は変数展開されて処理される
- エスケープ：
  bashでの予約記号を文字として表示したい場合、```\```を最初につけることで文字として処理される(=エスケープする　という)

　(例)

```
var="hoge"
echo '${var}'  # => ${var}
echo "${var}"  # => hoge
echo "${var} \${var}" # => hoge ${var} (エスケープした後者は$という文字として処理される)
```



#### ユーザ入力の読み込み、ファイルの読み込み (readコマンド)

```read```コマンドを使用して、キーボードからの入力を取得しインタラクティブに処理を行ったり、ファイルを１行ずつ処理したりすることができる。

[キーボード入力の処理]

```
read input
echo ${input}

while [ "${input}" != "end" ]; do
  read -p "input> " input
  echo "next!"
done

read -p "This operation is dangerous. Are you sure? (y/n): " input
while [ "${input}" != "y" ] && [ "${input}" != "n" ] ; do
  read -p "(y/n): " input
done
```

[ファイルの処理]
※標準入力をファイルにリダイレクトすることで、readへ１行ずつ入力

```
cat << EOF >test.txt
aaaa
bbbb
ccc
EOF

i=0
while read line; do
  ((i++))
  echo "$i: ${line}"
done <test.txt
```

#### 正規表現を使用したファイル検索処理

文字列操作での正規表現ルールを活用してファイルの検索が可能。再帰的な検索には向かないので、その場合は素直に```find```コマンドを使用する方がよい。

```
# /usr以下の全ディレクトリとファイルを列挙
for directory in /usr/*; do
  echo ${directory}
done

echo "----"

# /usr以下のsから始まる全ディレクトリとファイルを列挙
for directory in /usr/s*; do
  echo ${directory}
done

echo "----"
# *.confファイルを再帰的に列挙。再帰的な検索はfindの方が便利
for file in `find /etc/ -name "*.conf"`; do
  echo ${file}
done
```



## Appendix

#### 便利なLinuxコマンド

###### grep : 特定の文字を含む行を抽出

[用途例]

コマンドの実行結果のうち、必要な情報を抽出

```
ls /usr | grep bin
```

ls 対象ファイルの中から、必要な情報を抽出

```
grep kern /var/log/syslog
```

[書式]

```
grep [オプション] 検索パターン　ファイル名
```

オプション：

- -E: 正規表現を使用する
- -w: 単語全体で一致するものだけを抽出する
- -s: エラーメッセージを表示しない
- -i: 大文字/小文字を区別しない

オプション(ファイル指定時に使用するオプション)

- -n: 行番号も表示する
- -B 行数: 一致した行の前の行も指定行数分だけ表示する
- -A 行数: 一致した行の後の行も指定行数分だけ表示する
- -r: ディレクトリを指定した場合に、再帰的に検索する



###### awk / cut : 指定列の抽出

コマンド実行結果から、特定の列を抽出する

```
 ls -l /usr/ | awk '{print $1,$2,$NF}'  # 1列目、2列目、最後の列を抽出
 ls -l /usr/ | cut -d ' ' -f 1-4        # スペース区切りで、1列目から4列目を抽出
```



###### sed : 文字列の置換・編集

コマンドの実行結果を編集

```
コマンド | sed スクリプトコマンド
```

ファイルを読み込み編集して出力

```
sed -e スクリプトコマンド ファイル
```

ファイルを読み込み編集して中身を上書き

```
sed -i スクリプトコマンド　ファイル
```



スクリプトコマンド:

- 置換:
  s/パターン/置換後の文字列/     (最初に一致した文字列だけ置換)
  s/パターン/置換後の文字列/g   (一致した文字列全部を置換)

  ```
  ls /usr/ | sed 's/in/xx/g'    # 'in'を'xx'に置換
  ls /usr/ | sed 's/^bin$/xx/g' # 行頭b, 末尾nのbinという文字列を置換
  ```

- 削除

  ```
  sed -e '4d' file      # 4行目を削除
  sed -e '3,5d' file    # 3-5行目を削除
  sed -e '/aaa/,/bbb/d' # 'aaa'を含む行から'bbb'を含む行まで削除
  ```

- 指定行だけ抽出
  ```
  sed -n '1,4p' file  # 1-4行目だけ表示
  sed -n '/aaa/,/bbb/p' # 'aaa'を含む行から'bbb'を含む行だけ表示
  ```



###### find: 指定ファイルの検索

[書式]

```
find [オプション] 検索パス　検索式
```

(例)

```
find /usr -name "*bin"  # /usr以下のbinを含むディレクトリ、ファイルを検索
```

```
# コマンド置換を利用して、for-loopに組み込むのも可能
for f in `find /usr -name "*bin"; do
  ls -l $f
done
```



###### basename / dirname : ファイル名・ディレクトリ名の抽出

[書式]

```
basename パス名  # ファイル名を抽出
dirname パス名 # ディレクトリ名を抽出
```

(例)

```
filename=$(basename $0)
echo "Usage: ${filename} xx xx"  # こんな感じでファイル名だけ抽出が可能

exec_path=$(dirname $0)
${exec_path}/something_command.sh  # こんな感じで同階層のファイル指定するのに使える
```



###### curl : HTTPリクエストの送信(githubやslackへの通知など)

[HTTPメソッド]

- GET: リソースの取得
  - e.g. webページの取得、チャットのトーク取得(slack/teams)、レビュー情報の取得(Gitthub)
- POST: リソースの作成
  - e.g. メッセージの投稿(slack/teams)、CIでの評価結果の投稿(Github)
- PUT:  リソースの更新
- DELETE: リソースの削除

[書式]

- GET

```
curl [-H ヘッダ情報] http://xxx:port  # port番号を指定しない場合はhttpなら80, httpsなら443となる
```

(例)

```
curl -H "Content-Type: application/json" https://httpbin.org/get                       # 戻り値の形式をjsonで指定
curl -H "Content-Type: application/json" https://httpbin.org/get?arg1=hoge\&arg2=fuga  # 引数にarg1/arg2を指定
```

- POST

```
curl -X POST [-H ヘッダ情報] http://xxx:port -d '{リソースの情報}'
```

※-d '{"a": "xx", "b":"yy"}' は、 -d "a=xx" -d "b=yy" と書いてもＯＫ

(例)

```
curl -X POST -H "Content-Type: application/json" https://httpbin.org/post \
  -d '{"name": "test", "desc": "this is test"}

curl -X POST -H "Content-Type: application/json" https://httpbin.org/post \
  -d "name=test" -d "desc=this is test"
```

- PUT

```
curl -X PUT [-H ヘッダ情報] http://xxx:port -d '{リソースの情報}'
```

- DELETE

```
curl -X DELETE[-H ヘッダ情報] http://xxx:port 
```



[ユーザ認証]

```
curl -u user:password http://xxx:port
curl -X POST -u user:password http://xxx:port -d '{xxx:xxx}'
```

(例)

```
curl -u "testuser:testpassword" https://httpbin.org/basic-auth/testuser/testpassword
```



[使用例： (Slack) Incoming webhookを使ったメッセージ投稿 (非推奨)]

1. https://api.slack.com/messaging/webhooks か https://zenn.dev/hotaka_noda/articles/4a6f0ccee73a18 
   を参考にwebhookの登録を行う

2. 登録後、slackのページからwebhook URLを取得する

3. ```
   curl -X POST -H 'Content-type: application/json' \
    https://hooks.slack.com/services/{2.で取得したURL} \
    -d '{"text":"投稿する文字列"}'
   ```



[使用例: (Slack) Appを使ったメッセージ投稿(推奨)]

1. https://api.slack.com/authentication/basics か https://zenn.dev/kou_pg_0131/articles/slack-api-post-message
   を参考に通知用のbotを作成する

2. 作成後、User/Bot tokenを取得する (xoxb-xxx のようなフォーマットの文字列)

3. ```
   curl -X POST 'https://slack.com/api/chat.postMessage' \
     -d "token=取得したtoken" \
     -d "channel=#チャンネル名" \
     -d "text=投稿する文字列"
   ```

https://api.slack.com/methods

[使用例: (Teams) Incoming webhookを使ったメッセージ投稿]

1. https://learn.microsoft.com/ja-jp/microsoftteams/platform/webhooks-and-connectors/how-to/add-incoming-webhook
   を参考にwebhookの登録を行う

2. 登録後、表示されたページからwebhook URLを取得する

3. ```
   curl -H 'Content-Type: application/json' -d '{"text": "投稿する文字列"}' "2.で取得したURL"
   ```



###### ping / nc (対象ホストへの疎通確認)

[ping]

```
ping IPアドレス  # 指定IPアドレスへICMPを送り生存確認を実行
ping -c 回数 IPアドレス #指定回数だけICMPを送信
```

[nc (=netcat)]

```
nc -v -z IPアドレス ポート番号 # 指定IPアドレスの指定ポートの疎通確認を行う
# e.g.
# nc -v -z 192.168.1.1 22 (192.168.1.1のssh portの疎通確認を行う)
```



#### 代表的な環境変数

- ```$HOME```: ホームディレクトリ
- ```$HOSTNAME``` :ホスト名
- ```$USER```: ユーザ名
- ```$GROUPS```:グループID
- ```$PATH```: 実行パス
  - このパス以下の実行ファイルは絶対パスを省略して指定可能
    e.g. lsコマンドは```/bin/ls```と /bin以下にいるので、lsと打つだけで実行できる
- ```$PWD```: カレントディレクトリ

※その他に設定済みの環境変数は```printenv```で表示することができる
