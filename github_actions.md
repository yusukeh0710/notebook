# GitHub Actions

## 1. Try to build CI/CD with GitHub Actions

昨今のソフトウェア開発では、開発から本番環境へのデリバリーまでのサイクルを加速し、試行錯誤や価値提供のスピードをあげることが求められている。CI/CDは、継続的なソフトウェア検証とデリバリーを通じて、この課題を解決する手法として注目されている。

Continuous Integration (CI) では、ビルドとテストを自動化して、ソフトウェア検証を継続的に行うことで、問題の早期発見を実現する。Continuous Delivery (CD) ではリリースとデプロイを自動化して、ヒューマンエラーを防止するとともに、リリース時の工数を低減する。これらの自動化により、ソフトウェア品質を保ちながら、安定して高頻度にリリースすることが可能となる。

GitHubでは、GitHub Actionsと呼ばれるCI/CDサービスが提供されている。GitHub Actionsでは、ワークフローの定義から実行、結果の確認まで、GitHubのUI上で一元的に管理できる。本稿ではこのGitHub Actionsについて解説する。



#### ワークフローを定義する

Github Actionsでは、ワークフローという単位で処理を定義する。ワークフローはコードのビルド、テスト、デプロイなどの一連の処理を自動化する仕組みである。YAMLファイルで記述し、リポジトリの`github/workflows`に配置することで、Github Actionsで実行可能になる。

また、ワークフロー内で実行する最小の処理単位をアクションと呼ぶ。再利用可能な形で実装されており、公開されたアクションを利用することもでき、独自に作成したアクションをリポジトリ内に配置して使用することもできる。

例えば、リポジトリ内では、以下のような構成でファイルを配置する：

```
REPO_ROOT
  └── .github
        ├── workflows
        |   ├──  ...
        |   └──  ワークフローファイル
        |
        ├── actions
        |   ├──  ...
        |   └──  独自に作成したアクション
        |
        └── scripts
            ├──  ...
            └──  その他のツール、スクリプト
```



以下は、ワークフローのサンプルコードである。これを`.github/workflows/hello.yml`として動作確認のため保存する。

```yaml
name: Sample CI

on:
  push:
    branches: [ "main" ]
  workflow_dispatch:

jobs:
  hello:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Hello step
        run: echo "Hello, GitHub Actions!"
```



#### ワークフローを実行する ([参考ページ](https://docs.github.com/ja/actions/managing-workflow-runs-and-deployments/managing-workflow-runs/manually-running-a-workflow#running-a-workflow))

1. ワークフローを配置したリポジトリのGitHubページに移動する
2. リポジトリ名の下にある「Actions」をクリックする
3. 左側のサイドバーで、実行するワークフローを選択する
4. 「Run workflow」ボタンをクリックする



#### 実行結果を確認する

ワークフローを実行すると、実行一覧に結果が追加される。
例えば、以下の情報を確認できる：

- ワークフロー全体の成功/失敗状態
- 各ジョブやステップの実行状況
- 実行ログ
- 実行時間

- 生成された成果物



## 2. Workflow Components

ワークフローファイルは以下で構成される。

* ワークフロー： 1つのYAMLファイルで定義され、指定されたイベントをトリガーにジョブを実行する
* イベント：ワークフローのトリガー
* ジョブ：ワークフローの実行単位、jobs以下に複数定義可能
* ランナー：ジョブの実行環境
* ステップ：ワークフローにおける処理の最小単位、steps以下に複数定義



前述のワークフローファイルにあてはめると以下のようになる：

```yaml
name: Sample CI  # ワークフローの名前。Github Actions上に表示される。

on:　# イベント
  push:              # 発火条件1：main branchに変更がpushされたとき
    branches: [ "main" ]
  workflow_dispatch: # 発火条件2：手動実行されたとき

jobs:
  hello:  # ジョブ1： ジョブ名は"hello"
    runs-on: ubuntu-latest  # ランナー: ジョブ1はubuntu上で実行

    steps:
      - uses: actions/checkout@v4  # ステップ1: ソースコードをチェックアウト 
      - name: Hello step           # ステップ2: 文字列を出力、ステップ名は "Hello step"
        run: echo "Hello, GitHub Actions!"
```



#### イベント

主要なイベントとして、「手動実行」「定期実行」「Push/Pullrequestイベント」について解説する。
それ以外のイベントについては[イベント一覧](https://docs.github.com/ja/actions/writing-workflows/choosing-when-your-workflow-runs/events-that-trigger-workflows)を参照のこと。

###### <u>手動実行: workflow_dispatch</u>

リポジトリのページからワークフローを手動実行するためのイベントである。
入力パラメータは `inputs` キーで指定し、ワークフロー内で `${{ inputs.xxx }}` の形式で参照する。

```yaml
name: Manual Run
on:
  workflow_dispatch:
    inputs:
      message:
        required: true
      log-level:
        type: choice
        default: 'warning'
        options:
          - info
          - warning
          - debug
      environment:
        description: 'Environment to run tests'
        type: environment

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - run: echo "${{ inputs.message }}, Log level ${{ inputs.log-level }}"
```



入力パラメータには使用可能なプロパティは以下の通りである：

* type: データ型（boolean, number, string, choice, environment）
  * choice: 選択式入力、選択肢はoptionsで指定
  * [environment](https://zenn.dev/kitoketa/articles/f98a082e233c29): 登録済みの`Environments`から選択、`Environments`については[後述](#Environments)

* default: デフォルト入力値
* required: 指定が必須かどうか
* description: パラメータの説明文



###### <u>定期実行: schedule</u>

定期的にワークフローを実行するためのイベントである。実行タイミングはcron式で指定する。

```yaml
on:
  schedule:
    - cron: '30 0,12 * * 1'  # Run 0:30 and 12:30 on every Monday
    - cron: '30 6,18 * * 3'  # Run 6:30 and 18:30 on every Wednesday
```



###### <u>Push / Pullrequestイベント</u>

コード変更のイベントが生じたときにワークフローを実行する。
`push`はリポジトリへのプッシュ、`pull_request`はプルリクエストの作成や更新時に発火する。

```yaml
on:
  push:
    branches: [ main ]  # When code is pushed into the main branch 
  pull_request:
    branches: [ main ]  # When PR is created/updated for the main branch
```



#### ランナー

* [GitHub-hosted Runner](https://docs.github.com/ja/actions/using-github-hosted-runners/using-github-hosted-runners/about-github-hosted-runners)：
  GitHubの提供するマネージドな実行環境。特別な要件がなければ、こちらの利用を推奨する。
* [Self-hosted Runner](https://docs.github.com/ja/actions/hosting-your-own-runners/managing-self-hosted-runners/about-self-hosted-runners)：
  利用者が独自に用意・運用する実行環境。自身で用意するのでOSやスペックなどを自由に決められる。
* Container Runner:
  Docker containerをRunnerとして使用する。

```yaml
jobs:
  container-job:
    runs-on: self-hosted  # Containerをデプロイするホスト
    container:
      image: ubuntu:22.04
      volumes:
        - .....
      options: --cpus 1
    steps:
      .....
```



#### ステップ

* 「シェルコマンド」による定義：
  ```run```キーで指定し、通常のシェルスクリプトのように記述する。例えば：

  ```yaml
  - run : |
      echo "test"
      ls -l
  ```

* 「アクション」による定義例：
  ```uses```キーでアクション名を指定し、```with```キーで引数を指定する。例えば、

  ```yaml
  - uses: actions/checkout@v4
    with: ref
      ref: main
  ```



## 3. Workflow Syntax

#### 式と関数

* リテラル
  * null
  * boolean: `true` / `false`
  * number: 数字
  * string: 文字列
* 演算子
  * `!`, `&&`,`||`
  * `==`,`!=`,`>=`,`<=`,`>`,`<`
  * `()`,`[]`,`.`
* 関数
  * 文字列比較：contains(`VAR`, `keyword`), startsWith(`VAR`, `keyword`), endsWith(`VAR`, `keyword`)
  * 文字列生成：`format()`,`join()`
  * JSON操作：`toJSON(VAR)`,`fromJSON(VAR)`
  * ハッシュ生成：`hashFiles()`
* フィルタ
  * オブジェクトフィルタ： `${{github.event.*.html_url}}`
  * Glob
    * ```*```: 任意の文字列にマッチ（スラッシュを除く）
    * ```**```:任意の文字列にマッチ（スラッシュを含む）
    * `?`: 0文字または1文字（例：`test?.txt` → `test.txt`, `test1.txt`）
    * `+`: 任意の1文字（例：`test+.txt` → `test1.txt`）
    * `[]`: 括弧内の任意の1文字（例：`test[A-Z].txt` → `testX.txt`）
    * `!`: マッチしたパターンを除外（例：`!Readme.md`）



#### コンテクスト

コンテキストは、ワークフローの実行、変数、ランナーの環境、ジョブ、ステップなどの情報にアクセスする方法である。
各コンテキストはプロパティを含むオブジェクトであり、`${{  github.actor }}`の形式で参照する。

例えば、以下のようなコンテクストが存在する：

* githubコンテクスト：ワークフローの情報やイベントの情報を提供する ([プロパティ一覧はこちら](https://docs.github.com/ja/actions/writing-workflows/choosing-what-your-workflow-does/accessing-contextual-information-about-workflow-runs#github-context))

  * github.actor: ワークフローの実行ユーザ

  * github.run_id: 実行ID

  * github.event: トリガーとなったイベント

  * github.repository: リポジトリ名

  * github.head_ref: プルリクエストのソースブランチ


* runnerコンテクスト：実行環境の情報を提供する ([プロパティ一覧はこちら](https://docs.github.com/ja/actions/writing-workflows/choosing-what-your-workflow-does/accessing-contextual-information-about-workflow-runs#runner-context))

  * runner.name: ランナーの名前

  * runner.os: ランナーのOS


また、デバッグの手助けとして、以下のように書き、設定されたコンテクストを確認することができる：

```yaml
steps:
  - name: 'Debug github context'
    run: echo '${{ toJSON(github) }}'
  - name: 'Debug runner context'
    run: echo '${{ toJSON(runner) }}'
```



#### 環境変数

環境変数は`env`キーを使用して設定する。ワークフロー全体、ステップ内に限定など、任意のスコープに適用することができる。
環境変数の参照は、`${MY_ENV_VAR}}`、もしくは、コンテキストを介して`${{ env.MY_ENV_VAR }}`と記述する。なお、GitHub Actionsが提供するデフォルトの環境変数については[公式ドキュメント](https://docs.github.com/ja/actions/writing-workflows/choosing-what-your-workflow-does/store-information-in-variables#default-environment-variables)を参照のこと。

また、セキュリティ上の理由から、コンテキストの値をシェルコマンドで直接使用するのではなく、環境変数を介して渡すことが推奨される（中間環境変数）。

```yaml
on:
  workflow_dispatch:
  
env:
  # ここに書くと、ワークフロー内全域に指定した環境変数を適用
    
jobs:
  job:
    steps:
      - run: echo "This is test"
        env:
          # ここに書くと、このステップにだけ指定した環境変数を適用

      - run: echo "The actor is ${{ github.actor }}."  # インジェクションのリスクあり
      - run: echo "The actor is ${ACTOR}"              # インジェクションを回避〇
        env:
          ACTOR: ${{ github.actor}}
```



#### Variables / Secrets

GitHub Actionsでは、複数のワークフローで共通して使用する値を`Variables`と`Secrets`として事前に登録できる。値の参照はコンテクストを介して取得し,`${{ var.VAR_NAME }}` や `${{ secrets.PASSWORD }}` のように記述する。
Varibles/Secretsは以下のように使い分ける：

- **Variables**：機密性が低い情報を格納。
- **Secrets**：機密性が高い情報を格納。Secretsに登録された情報は暗号化され、ログや設定画面でもマスクされる。



参考情報：

* [Variablesに情報を格納する](https://docs.github.com/ja/enterprise-cloud@latest/actions/writing-workflows/choosing-what-your-workflow-does/store-information-in-variables)
* [GitHub ActionsでのSecretsの使用](https://docs.github.com/ja/actions/security-for-github-actions/security-guides/using-secrets-in-github-actions)



#### Environments

`Environments`は、複数のデプロイ環境（本番環境、テスト環境など）を管理するための機能である。以下のような特徴がある：

* デプロイ先ごとに異なる変数(ホスト名やユーザ名など)やシークレット（パスワードなど）を管理する
* ```Environment```をデプロイ先の数だけ作成し、```Environment variable```sと```Environment secrets```をそれぞれに登録する
* ジョブの中で使用するときには、```environment: <environment-name>```と指定する



#### 条件分岐

`if`キーを使用することで、ジョブやステップの実行条件を制御できる。

<u>ステータスによる条件分岐</u>

* success(): 手前の処理が成功したらTrue
* failure(): 手前の処理が失敗したらTrue
* cancelled(): 手前の処理がキャンセルされたらTrue
* always(): 手前の処理の結果に関係なくTrue

(例)

```yaml
steps:
  - run: echo "This is test"
  - run: echo "Run if success"
    if: ${{ success() }}
```



<u>条件式による条件分岐</u>

(例)

```yaml
steps:
  - run: echo "This is run by manual run"
    if: ${{ github.event_name == 'workflow_dispatch' }}
  - run: echo "This is run by code push"
    if: ${{ github.event_name == 'push' }}
```



#### ステップ間のデータ共有

* GITHUB_OUTPUT: 変数としてデータを保持
* GITHUB_ENV: 環境変数としてデータ保持
  ※基本はGITHUB_OUTPUTを推奨、環境変数としてセットが必要なときだけGITHUB_ENVを使用する

```yaml
steps:
  - run: export SHARE="test"
  - run: echo "${SHARE}"  # ステップごとに変数がリセットされるため、何も表示されない

  - id: my_step
    run: echo "share=test" >> ${GITHUB_OUTPUT}
  - run: echo "${{ steps.my_step.outputs.share }}" # id:my_stepの出力した変数share が表示される

  - run: echo "SHARE=test" >> ${GITHUB_ENV}
  - run: echo "${SHARE}"                           # 環境変数として取り出し
```


もしくは、以下のように書くこともできる：

```yaml
steps:
  - id: my_step
    run: echo "share=test" | tee -a  ${GITHUB_OUTPUT}
  - run: echo "${{ steps.my_step.outputs.share }}"

  - run: echo "SHARE=test" | tee -a ${GITHUB_ENV}
  - run: echo "${SHARE}"
```



#### その他のテクニック

<u>実行タイトルの設定</u>

`run-name`キーを使用して、実行タイトルを動的に設定できる：

```yaml
run-name: Run by @${{ github.actor }}
```



<u>シェルの指定</u>

ステップごとに使用するシェルを指定できる：

```yaml
steps:
  - run: echo "This will be run w/ bash"
    shell: bash
```

例えば、bash, python, powershellなどが指定可能。
なお、shellキーの有無でBashの起動オプションが変わる点に注意：

* 省略時：```bash -e {0}```
* 記述時：```bash --noprofile --norc -eo pipefail {0}```

また、デフォルトシェルを指定するときは、以下のように指定する
（bashであっても、上記で説明したようにshell指定の有無で挙動が変わるので重要）

```yaml
defaults:
  run:
    shell: bash  # これで、各ステップのデフォルトが bash --noprofile --norc -eo pipefail {0}となる
```



<u>エラーハンドリング</u>

```continue-on-error```を指定すると、ステップがエラーになっても後続のステップを実行する：

```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - run: exit 1
        continue-on-error: true
      - run: echo "This will be run even if above step is failed."
```


<u>タイムアウト設定</u>

```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    timeout-minutes: 1  # Timeout: 1min
```



## 4. Designing Workflow

#### 複数ジョブの制御

* それぞれのジョブは、デフォルトでは並列実行される
* ジョブ間に依存関係があり、逐次実行する場合、```needs```キーを使用して実行順序を制御する

<img src="./figure/github_actions/design_workflow.png" alt="design_workflow" style="zoom:40%;" />

#### ジョブ間のデータ共有

* ジョブ間でデータを共有する場合、`outputs`を使用する
* 送り手：`GITHUB_OUTPUTS`に値を出力し、`outputs`にて格納する変数を指定する
* 受け手：`${{ needs.ジョブ名.outputs.変数名 }}`の形式でアクセスする

```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - id: build_action
        run: echo "share=success" >> ${GITHUB_OUTPUT}
    outputs:
      result: ${{ steps.build_action.outputs.share }}

  report:
    runs-on: ubuntu-latest
    needs: [ build ]
    steps:
      - run: echo "${{ needs.build.outputs.result }}"
```



#### Matrix

`strategy.matrix`は、複数の設定でジョブを実行する機能である。これを使用して、複数のOSや複数バージョンのアプリ、実行環境の組み合わせを一度にテストすることができる。

以下の例では、`os`と`python version`を指定しており、合計4パターンのジョブが実行される。

```yaml
jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest]
        python-version: [2.7, 3.8]
    runs-on: ${{ matrix.os }}
    steps:
      - run: echo "${{ matrix.os }}-${{ matrix.python-version }}"
```



また、特定の組み合わせのジョブを実行する場合には、`includes`を使用する。
以下の例では、 `ubuntu x python2.7`と`windows x python3.8`のみ実行される。

```yaml
jobs:
  build:
    strategy:
      matrix:
        include:
          - os: ubuntu-latest
            python-version: 2.7
          - os: windows-latest
            python-version: 3.8
    runs-on: ${{ matrix.os }}
    steps:
      - run: echo "${{ matrix.os }}-${{ matrix.python-version }}"
```



#### キャッシュ

```actoins/cache```は、ダウンロードや生成物などの再利用可能なデータをキャッシュする機能である。キャッシュを使用することで、ビルド時間を短縮し、ワークフローの実行効率を向上させることができる。説明のため、以下に使用例を示す：

```yaml
- uses: actions/cache@v4
  with:
    path: ${{ github.workspace }}/cache
    key: key-${{ runner.os }}-${{ github.sha }}
    restore-keys: key-${{ runner.os }}
```



<u>キャッシュの動作</u>

* `path`で指定したファイルやディレクトリがキャッシュの対象となる
* `key`に完全一致したら、そのキャッシュが指定パスに展開される
* 一致するキャッシュがない場合で、`restore-keys`に前方一致するキャッシュが存在すればそれが展開される
* `actions/cache`では保存と復元を両方担当するが、以下のサブアクションにて片方だけ実行も可能である：
  * `action/cache/save`: キャッシュの保存だけ
  * `action/cache/restore`: キャッシュの復元だけ


<u>キャッシュの削除</u>

* 7日以上アクセスされていないキャッシュは自動削除
* 合計サイズは各リポジトリ10GBまで
* リポジトリの「Actions」ページからキャッシュの確認と削除が可能



<u>サンプルコード</u>

```yaml
... (前略) ...

jobs:
  build:
    runs-on: ubuntu-latest
    env:
      CACHE_PATH: ${{ github.workspace }}/cache
    steps:
      - name: Cache test file
        id: cache-file
        uses: actions/cache@v4
        with:
          path: ${{ env.CACHE_PATH }}
          key: key-${{ runner.os }}-${{ github.sha }}
          restore-keys: key-${{ runner.os }}

      - name: Create test file
        if: steps.cache-file.outputs.cache-hit != 'true'
        run: |
          mkdir -p ${CACHE_PATH}
          echo "This is a test file" > ${CACHE_PATH}/test.txt

      - name: Read cache
        run: cat ${CACHE_PATH}/test.txt
```



#### アーティファクト

Artifactは、ジョブ間でファイルを共有するための機能である。`actions/upload-artifact`でアップロードを行い、`actions/download-artifact`でダウンロードを行う。また、実行完了後も一定期間保存され、ワークフローの実行ページから生成物の確認、ダウンロード、削除ができる。

```yaml
- uses: actions/upload-artifact@v4
  with:
    name: キーワード
    path: アップロードするファイルやディレクトリのパス
    retention-days: 保存期間
```

```yaml
- uses: actions/download-artifact@v4
  with:
    name: キーワード
    path: ダウンロードするファイルやディレクトリのパス
```


<u>サンプルコード</u>

```yaml
... (前略) ...

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - run: echo "This is test" > test.txt
      - uses: actions/upload-artifact@v4
        with:
          name: keyword
          path: test.txt
          retention-days: 1
      - uses: actions/download-artifact@v4
        with:
          name: keyword
          path: download
      - run: cat download/test.txt
```



#### レポーティング

###### <u>デバッグログの有効化</u>

* ワークフローの再実行時に「Enable debug logging」にチェックをつける

* SecretsまたはVariableへ特定のキーと値をセットする：

  * ステップデバッグログ：`ACTIONS_STEP_DEBUG: True`

  * ランナー診断ログ：`ACTIONS_RUNNER_DEBUG: True`



###### デバッグログの出力

ワークフローコマンド `::debug::`を使用して、デバッグログを出力できる。
```yaml
run: echo "::debug::This is debug message."
```



###### <u>ログのグループ化</u>

ワークフローコマンド`::group::<group-name>`と`::endgroup::`を使用してログ出力をグループ化できる。囲まれた部分の出力はグループ化されて、折りたたまれて表示される。

グループ化は、環境変数や実行環境の情報など、デバッグにそなえて多くの出力をするときに有効である。例えば、以下では環境変数の一覧を折りたたんで表示しており、必要なときだけ展開して確認することができる。

```yaml
steps:
  - name: ENV
    run: |
      echo "::group::Env dump"
      printenv
      echo "::endgroup"
```

<img src='./figure/github_actions/groupenv.png' style="zoom:70%;" />

###### <u>アノテーション</u>

ワークフローコマンド`::notice::`, `::warning::`, `::error::`を使用して、重要度に応じたメッセージを出力できる。また、ワークフローの実行ページのサマリー部分にも表示され、重要なメッセージを強調して残すことができる。

```yaml
steps:
  - run: echo "::notice::Notice message"
  - run: echo "::warning::Warning message"
  - run: echo "::error::Error message"
```

<img src='./figure/github_actions/annotation.png' style="zoom:55%;" />

###### <u>ジョブサマリー</u>

`${GITHUB_STEP_SUMMARY}` へマークダウン形式を出力すると、整形されて、ワークフロー実行ページ上にサマリー表示が生成される。ジョブの実行結果の要約やテスト結果の可視化の方法として有用である。

```yaml
steps:
  - run: |
      echo "## Sameple job summary" >> ${GITHUB_STEP_SUMMARY}
      echo " * First item" >> ${GITHUB_STEP_SUMMARY}
      echo " * Second item" >> ${GITHUB_STEP_SUMMARY}
```



###### <u>チャット通知</u>

slack / teams



## 5. Action Module

アクションはモジュール化の重要な手段である。アクションを使用し、複雑な処理を再利用可能なコンポーネントとして実装できる。

アクションのロケーションには、公開リポジトリで管理されるリモートアクションと、同一リポジトリ内で管理されるローカルアクションの２種類が存在する。また、アクションの実装方法は以下に示す３つの方法が存在する：

* Composite Action：既存のアクションやシェルコマンドを組み合わせて実装
* JavaScript Action：Node.jsを使用して実装
* Docker Container Action：Dockerコンテナとして実装



#### リモートアクション / ローカルアクション

リモートアクションは、既に登場した```actions/checkout@v4```などを指す。文字列とURLは連動し、`{owner}/{repository}@{tag}`で指定して使用する。例えば、actions/checkout@v4は```https://github.com/actions/checkout```のv4を指し、```docker/build-push-action@v5```は```https://github.com/docker/build-push-action```のv5を指す。

ローカルアクションでは、アクションを格納したディレクトリパスを指定して使用する。アクションは`.github/actions`ディレクトリ配下に配置し、以下のような構造で実装する：

```
ROOT_REPO
└── .github
　   └── actions
    　   ├── action-name-A/   # アクション名のサブディレクトリ
    　   │    ├── action.yml  # アクションの定義ファイル
    　   │    └── ...         # 必要に応じて、その他のファイル
    　   |   ....
    　   │
    　   └── action-name-Z/
    　       ├── action.yml
    　       └── ...
```



#### Composite Action

Composite Actionはワークフローファイル同様に、複数のシェルやアクションを組み合わせて作成する。メタデータ構文と呼ばれる構文で書かれており、`input`、`output`、`runs`とその他のキーで構成される。ワークフローファイルと異なり、shellの指定を省略できない点に注意されたい。以下に、サンプルコードを示す：

[.github/actions/sample/action.yml]

```yaml
name: Sample Action
description: |
  This is a sample action module.

inputs:  # アクションの入力
  message:
    description: Print text
outputs: # アクションの出力
  time:
    value: ${{ steps.get-date.outputs.time }}

runs:　# `using: composite`から始めて、手続きを記載
  using: composite
  steps:
    - id: get-date
      shell: bash
      run: |
        echo "Hello ${{ inputs.message }}."
        echo "time=$(date)" >> ${GITHUB_OUTPUT}
```

[ワークフローファイル]

```yaml
steps:
  - uses: actions/checkout@v4       # action.ymlを参照するため、checkoutは必須
  - id: sample
    uses: ./.github/actions/sample/ # 使用するアクションのサブディレクトリを指定
    with:
      message: Test                 # ここに入力引数を指定
  - run: echo "${{ steps.sample.outputs.time }}"  # 出力の受け取りはstep間のデータ共有と同様、idで参照
```



#### Javascript Action

Javacript Actionでは、Node.jsを使用してアクションを実装する。入出力はJavscript libraryである`@actoins/core`を介して行う。
基本的なディレクトリ構成と、サンプルコードを以下に示す。

```
<基本形>
.github/actions/javascript-example/
  ├── action.yml    # アクションの定義ファイル
  ├── index.js      # メインのソースコード
  └── package.json  # 依存関係の定義
```

[action.yml]

```yaml
name: Sample Action
description: |
  This is a sample action module.

inputs:
  message:
    description: Print text
outputs:
  time:
    description: Current Time

runs:
  using: node20
  main: index.js
```

[index.js]

```javascript
const core = require('@actions/core');

try {
  const message = core.getInput('message');
  console.log(`Hello ${message}.`);
  const time = (new Date()).toTimeString();
  core.setOutput("time", time);
} catch (error) {
  core.setFailed(error.message);
}
```

[package.json]

```json
{
 "name": "sample-action",
 "version": "1.0.0",
 "description": "Sample Action",
 "main": "index.js",
 "dependencies": {
   "@actions/core": "^1.10.1"
 }
}
```

[ワークフローファイル]

```yaml
steps:
  - uses: actions/checkout@v4
  - uses: actions/setup-node@v4
  - run: npm install
    working-directory: ./.github/actions/sample
  - uses: ./.github/actions/sample/
    id: sample
    with:
      message: Test
  - run: echo "${{ steps.sample.outputs.time }}"
```



#### Docker Container Action

Docker Container Actionでは、Dockerコンテナとしてアクションを実装する。特定のランタイムや環境、複雑な依存関係がある場合に適している。入力はシェルの実行引数を介して行い、出力は`${GITHUB_OUTPUT}`を介して行う。
基本的なディレクトリ構成と、サンプルコードを以下に示す。

```
.github/actions/docker-example/
  ├── action.yml
  ├── Dockerfile
  └── entrypoint.sh
```

[action.yml]

```yaml
name: Sample Action
description: |
  This is a sample action module.

inputs:
  message:
    description: Print text
outputs:
  time:
    description: Current Time

runs:
  using: docker
  image: Dockerfile
  args:
    - ${{ inputs.message }}
```

[Dockerfile]

```yaml
FROM alpine:3.19
COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]

```

[entrypoint.sh]

```bash
#!/bin/sh -l
echo "Message: $1"
echo "time=$(date)" >> ${GITHUB_OUTPUT}
```

[ワークフローファイル]

```yaml
steps:
  - uses: actions/checkout@v4
  - uses: ./.github/actions/sample/
    id: sample
    with:
      message: Test
  - run: echo "${{ steps.sample.outputs.time }}"
```



## 6. Workflow Collaboration

#### ワークフローのの排他制御と自動キャンセル

`concurrency`を使用することで、同時実行するワークフローの数を制御できる。同じ`concurrency`グループに属するワークフローは、一度に1つしか実行されない。

```yaml
name: Exclusive workflow
on: pull_request
concurrency:
  group: ${{ github.workflow }}-${{ github.ref }}  # 同じPull Requestからのワークフローの同時起動を防止する
jpbs:
  ....
```



さらに、`cancel-in-progress: true`を設定することで、同じグループの古い実行を自動的にキャンセルできる。これは、最新のコミットに対する処理のみを実行したい場合に有用である。

```yaml
name: Auto Cancel
on: pull_request
concurrency:
  group: ${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true
jpbs:
  ....
```



#### Reusable Workflows

アクションは小さな処理をカプセル化する。一方、Reusable Workflowsはワークフローを丸ごとカプセル化する。
もし、あるワークフローを別のワークフローの中に組み込み再利用するようなケースでは、Reusable Workflowsが効果を発揮する。

[Callee]

```yaml
name: Reusable Workflows
on:
  workflow_call:
    inputs:  # 入力パラメータの定義はworkflow_dispatchと同様
      message:
    secrets: # inputs以外にも自由なキーワードで引数設定が可能
      token:
    outputs: # 出力値の定義
      result:
jobs:
   ....
```

[Caller]

```yaml
name: Caller
on: ...
jobs:
  call:
    uses: ./.github/workflows/reusable-workflows.yml
    with:
      message: "Test"
```



#### repository-dispatchイベント

Repository Dispatchは、外部から手動でワークフローをトリガーするための機能である。REST APIを使用して特定のイベントを発行し、それに応じてワークフローを実行することができる。

`types`で受け付けるイベントタイプを指定できる

`client_payload`でイベントとともにデータを送信できる

認証にはGitHubのパーソナルアクセストークンが必要。もしくはGithubApps。

[Callee workflow]

```yaml
name: Handle Repository Dispatch
on:
  repository_dispatch:
    types: [build, deploy]

jobs:
  process:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Handle build event
        if: github.event.action == 'build'
        run: echo "Build triggered with ${{ github.event.client_payload.version }}"
      - name: Handle deploy event
        if: github.event.action == 'deploy'
        run: echo "Deploy triggered to ${{ github.event.client_payload.environment }}"
```

[パーソナルトークンを使用して実行する場合]

```bash
curl -X POST \
  -H "Authorization: token ${GITHUB_TOKEN}" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/${OWNER}/${REPO}/dispatches \
  -d '{"event_type": "build", "client_payload": {"version": "v1.0.0"}}'
```

[GitHub Appsを経由して実行する場合]

```bash
curl -X POST \
  -H "Authorization: Bearer ${GITHUB_APP_TOKEN}" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/${OWNER}/${REPO}/dispatches \
  -d '{"event_type": "build", "client_payload": {"version": "1.0.0"}}'
```



## 7. Package Release

#### リリースのトリガとなるイベント

タグやリリースの作成をトリガーとしてワークフローを実行し、パッケージのリリースを自動化できる。

###### <u>tagイベント</u>

```yaml
on:
  push:
    tags:
      - 'v[0-9]+.[0-9]+.[0-9]+'  # e.g. v1.2.3
```

<u>releaseイベント</u>

```yaml
on:
  release:
    types: [ リリースイベントタイプ ]
```



[前段ワークフロー]
※ `PyGithub`を使ってpythonで書いた場合：

```python
from github import Github

token = xxxx
g = Github(token)
repo = g.get_repo(repo_name)
release = repo.create_git_release(
    tag=tag_name,
    name=release_name,
    message=body,
    draft=draft,            # (optional) default: False
    prerelease=prerelease,  # (optional) default: False
)
```

| draft  /  prerelease | リリースイベント    |
| -------------------- | ------------------- |
| False  /  False      | published, released |
| False / True         | preleased, released |
| True / False         | created             |
| True / True          | created             |



#### リリースの自動生成

タグをプッシュした際に、GitHub CLIを使用して前回のリリースからの変更をまとめたリリースノートを自動生成する。

```yaml
name: Create Release
on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0  # 全履歴を取得
      
      - name: Create Release Note
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        run: |
          # タグ名を取得
          TAG_NAME=${GITHUB_REF#refs/tags/}
          
          # リリースを作成（--generate-notesオプションで自動生成）
          gh release create $TAG_NAME \
            --title "Release ${TAG_NAME}" \
            --generate-notes
```

このワークフローは：

1. タグのプッシュをトリガーとする
2. リポジトリの全履歴を取得
3. GitHub CLIを使用してリリースノートを自動生成
4. 指定したタグ名でリリースを作成

`--generate-notes`オプションにより、前回のリリースからのコミット履歴を基に、プルリクエストやコミットメッセージを含むリリースノートが自動的に生成される。



## Other Tips

<u>手元でワークフローファイルを検証する</u>

actionlint

https://github.com/rhysd/actionlint/releases



<u>複数のイベントを待ち受けるときは、先に入力パラメータを整理する</u>

```test-pool: ${{ inputs.test-pool || 'all' }}```



<u>入力パラメータはサマリに表示しておく</u>



<u>printenv推奨</u>
