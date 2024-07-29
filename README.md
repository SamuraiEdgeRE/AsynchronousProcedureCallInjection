# AsynchronousProcedureCallInjection

## プロジェクトの概要
このプロジェクトでは非同期プロシージャ呼び出しを使ったプロセスインジェクションツールを実装しました。  
プロセスインジェクションは特定のプロセスメモリ空間に外部からコードを書き込んで実行する手法です。  
これにより、ターゲットプロセスのメモリ内で任意のコードを実行することが可能となるため、ウイルス対策ソフトの検出回避、アプリの改造、WindowsAPI関数のフックなどの目的でよく使用されます。

## プロジェクトの目的
このツールの目的は、プロセスインジェクション技術を学び、その実装方法を理解することです。また、セキュリティ研究やマルウェア解析の教育用途にも利用されることを目指しています。

[インジェクター名] [プロセス名] [ファイルパス]

#### 例：```AsynchronousProcedureCall_ShellcodeInjection.exe Notepad.exe "C:\Code\TestShellcode\x64\Release\TestShellcode.bin"```
(メモ帳プロセスにTest.binをシェルコードインジェクション)

## AsynchronousProcedureCallInjectionインジェクションの詳細
