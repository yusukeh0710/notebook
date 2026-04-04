# Chapter 1: サブモジュールの追加

git submodule add <URL or PATH> <path>

- 親リポジトリに gitlink（commitポインタ）が追加される  
- .gitmodules に URL と path が記録される  

---

# Chapter 2: サブモジュールの更新

## 2.1 登録コミットに合わせる（再現）

git submodule update

- 親リポジトリが指している commit に checkout  
- 再現性のための操作  

---

## 2.2 最新に追従する

git submodule update --remote

- 指定branchの最新commitに移動  
- mergeではなく 強制checkout（reset的挙動）  

---

## 2.3 追従ブランチを指定する

git config -f .gitmodules submodule.<path>.branch <branch>  
git add .gitmodules  
git commit -m "set submodule branch"

例：  
git config -f .gitmodules submodule.utils/mod_a.branch develop

- どのbranchを追うかを定義  
- 「pin止め」ではない  

---

## 2.4 特定コミットに固定（pin止め）

cd <submodule>  
git checkout <commit or tag>  

cd ..  
git add <submodule path>  
git commit -m "pin submodule"

- 再現性を確保するための固定  

---

# Chapter 3: サブモジュール込みでclone

git clone --recurse-submodules <repo>

または：

git clone <repo>  
git submodule update --init --recursive

- submoduleの内容も含めて完全再現  

---

# Chapter 4: サブモジュールで開発する

## 4.1 submoduleへ移動

cd <submodule path>

## 4.2 ブランチ作成（必須）

git switch -c feature/xxx

- detached HEADで作業しない  

## 4.3 修正・コミット

git add .  
git commit -m "update"

## 4.4 push

git push origin feature/xxx

- submoduleは独立repoなので必須  

---

# Chapter 5: 親リポジトリへ反映（最重要）

git submodule update --remote  
git add <submodule path>  
git commit -m "update submodule"

- submoduleのcommitポインタを更新  
- これをしないと他人に反映されない  

---

# Chapter 6: 本質理解

- submoduleは別リポジトリ  
- 親リポジトリはcommitポインタのみ管理  
- 実体は含まれない  

---

# Chapter 7: 運用パターン

- 固定（pin） → 安定・再現性重視  
- 追従（remote） → 開発効率重視  

---

# Chapter 8: 最終まとめ

submoduleとは：

外部リポジトリの特定commitを、親リポジトリでバージョン管理する仕組み# Chapter: サブモジュールの情報取得

## 1. サブモジュール一覧と現在のcommit

git submodule status

- 各submoduleの commit hash とパスを表示  
- 先頭記号の意味：  
  - " "（スペース）: 正常  
  - "-" : 未初期化  
  - "+" : 親repoと不一致  

---

## 2. 詳細情報を表示

git submodule summary

- 親repoが指しているcommitと、現在の差分を表示  

---

## 3. インデックス上のcommitを確認

git ls-files --stage

- submoduleは「160000」のモードで表示される  
- その横のhashが記録されているcommit  

---

## 4. submodule内部の状態確認

git -C <submodule path> status

- submodule内の変更状況を確認  

---

## 5. 現在のHEAD（commit）確認

git -C <submodule path> rev-parse HEAD

- 現在checkoutされているcommitを取得  

---

## 6. リモートとの関係確認

git -C <submodule path> branch -vv

- どのbranchにいるか  
- upstreamとの関係  

---

## 7. .gitmodulesの確認

cat .gitmodules

- submoduleのURL、path、branch設定を確認  

---

## まとめ

- 親repo視点 → git submodule status / ls-files  
- submodule内部 → git -C <path> ...  
- 設定情報 → .gitmodules  

👉 「ポインタ（親）」と「実体（submodule）」を分けて見るのが重要