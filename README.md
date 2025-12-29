# yasl
Yet Another Stack Language

## 概要

**YASL (Yet Another Stack Language)** は多重逆順スタックパラダイムに基づいたインタプリタ言語です。元々は非IT専門家のユーザー向けに高レベルの会計および財務処理のために設計されました。

YASLの詳細な言語仕様や使い方については、[GitHub Wiki](https://github.com/GawinGowin/yasl/wiki)を参照してください。

## リポジトリ構造

```
yasl/
├── .devcontainer/       # Dev Container設定
│   └── devcontainer.json
├── .vscode/             # VS Code設定
├── example/             # C言語実装例
│   ├── Makefile
│   ├── display_b64.c
│   └── display_b64.out
├── img/                 # テスト用画像（Base64エンコード済み）
│   ├── img1.rgb.b64
│   ├── img2.rgb.b64
│   ├── img3.rgb.b64
│   ├── img4.rgb.b64
│   ├── img5.rgb.b64
│   ├── faulty1.rgb.b64
│   └── faulty2.rgb.b64
├── src/                 # YASLスクリプト（各Exercise）
│   ├── yasl_hw
│   ├── yasl_aff_param
│   ├── yasl_do
│   ├── yasl_repeat
│   ├── yasl_fact
│   ├── yasl_split
│   ├── yasl_interactive
│   └── display_b64
├── CLAUDE.md            # プロジェクトドキュメント
├── Dockerfile           # Dockerイメージ定義
└── README.md            # このファイル
```

## 開発環境のセットアップ

### devcontainerの利用（推奨）

このプロジェクトはDev Container環境で開発することを推奨しています。YASLインタプリタが事前にセットアップされており、すぐに開発を始められます。

**セットアップ手順：**

1. **VS Codeのインストール**
   [Visual Studio Code](https://code.visualstudio.com/)をインストールします。

2. **Dev Containers拡張機能のインストール**
   VS Codeの拡張機能マーケットプレイスから「Dev Containers」をインストールします。

3. **リポジトリのクローン**
   ```bash
   git clone https://github.com/GawinGowin/yasl.git
   cd yasl
   ```

4. **VS Codeでプロジェクトを開く**
   ```bash
   code .
   ```

5. **コンテナで再度開く**
   VS Codeが起動したら、左下の緑色のアイコンをクリックし、「Reopen in Container」を選択します。

6. **スクリプトの実行**
   コンテナ内でYASLスクリプトを実行できます：
   ```bash
   yasl src/yasl_hw
   # または
   chmod +x src/yasl_hw
   ./src/yasl_hw
   ```

### 手動セットアップ

YASLインタプリタを手動でインストールする場合は、42.fr CDNからダウンロードし、`/usr/local/bin/yasl`に配置してください。

詳細な手順については、[Dockerfile](./Dockerfile)を参考にしてください。

## Exercises

このリポジトリには、YASL言語の学習用に複数のExerciseスクリプトが含まれています。

- Exercise 0: yasl_hw
- Exercise 1: yasl_aff_param
- Exercise 2: yasl_do
- Exercise 3: yasl_repeat
- Exercise 4: yasl_fact
- Exercise 5: yasl_split
- Exercise 6: yasl_interactive

---

### Main Course: display_b64

標準入力からBase64エンコードされた24ビットRGB正方形画像を読み取り、ターミナル上で標準256色を使用して画像を表示します。

**使用方法：**
```bash
$ cat img/img1.rgb.b64 | ./src/display_b64
```

**スクリーンショット：**

<img width="1020" height="1079" alt="Screenshot from 2025-12-29 15-23-18" src="https://github.com/user-attachments/assets/c88e3c0a-ae00-4a27-89b5-a4eb994bd457" />

<img width="1342" height="2857" alt="Screenshot from 2025-12-29 15-25-54" src="https://github.com/user-attachments/assets/1e7ee321-ac50-474c-9f59-29f080f6d9c9" />

<img width="989" height="563" alt="Screenshot from 2025-12-29 15-27-28" src="https://github.com/user-attachments/assets/caaf976d-3e12-4eee-8772-b26e137ccab9" />

---

## 関連リンク

- [GitHub Wiki](https://github.com/GawinGowin/yasl/wiki) - YASL言語の詳細仕様
