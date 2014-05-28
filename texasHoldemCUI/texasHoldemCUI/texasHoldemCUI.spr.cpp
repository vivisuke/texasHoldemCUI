﻿◎ テーブル幅を最大コミュニティカード数に合わせる
◎ ディーラマーク表示
◎ BB, SB マーク表示
◎ チップを正しく表示
◎ pot 表示
◎ 各プレイヤーのベット額表示
◎ 乱数生成器を mt19937 に変更
◎ カード表示：白背景に黒 or 赤で描画
◎ 伏せているカードはシアンで表示
◎ メニューをデータ化
◎ 左右キーでメニュー選択可能に
◎ とりあえず Check/Call で先に進めるようにする
◎ コールが揃ったら、次のターンに進む
◎ コミュニティカードを正しく表示
◎ 問題：コール時に持ちチップが減っていない
◎ 問題：精算時にコンピュータの手札がオープンされない
◎ 精算時：役種別表示
◎ 人間の手番でないときは、メニューを消した方がよい
◎ 結果表示
◎ １ゲーム終了後の精算処理
◎ 次のゲームに進んだ時、ディーラが進まない
◎ 問題：２ゲーム目が勝手に進行してしまう
◎ 問題：ディーラーマークが消えない
◎ 問題：精算後はベット値表示を消した方がよい
◎ フォールド処理
◎ 問題：どちらかがフォールドした場合はCOMの手札をオープンしない
◎ レイズ初期値：BB/2BB
◎ 上下キーでレイズ額を設定可能に
◎ [AllIn] メニュー追加
◎ 問題：メニュー表示状態で q を押したらアサーション発生
◎ レイズ額を上げた場合は、[Raise] を選択状態に
◎ レイズ処理
◎ 人間がレイズし、COMがコールしたはずなのに、COM のチップ、ベット表示が変化しない
◎ チップが無くなった時の終了処理
● 精算時に音声再生？
◎ _getch() は getChar() でのみコールするよう修正
◎ 問題：Fold したのに、人間の方のチップが増えてしまう
◎ 問題：Fold していないのに、COMの手札がオープンされない場合がある
◎ 問題：役表示：以前のテキストが残っている場合がある
◎ 問題：次のゲームになっても役表示が残っている
◎ AllIn メニューを選択した場合は、金額を全チップ額に設定
◎ デフォルトのレイズ額：チェックの場合は0、コールの場合は必要額にする
◎ SB の場合は、SB だけレイズ可能にする。その後は最小ベット額の倍数
◎ 問題：大量にレイズしたら、COM, MAN のチップが増えてしまった？
● D：役にしたカードがどれかわかるように表示（コミュの上下に*表示？）
● 問題？：相手のチップ額より多い額をベットできてしまう
● 問題：チップが足りないのにコールできてしまう
◎ どちらかがオールイン（chip == 0）の場合は、自動的に先にすすめるようにする
◎ 問題：オールインしても、それがベットに反映されない
◎ 問題：スプリットの時、各プレイヤーのチップが不正になった
● 問題：人間の方がチップが多い時に、オールインすると固まる