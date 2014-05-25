#include "consoleUtil.h"
#include "Deck.h"
#include "TexasHoldem.h"
#include <windows.h>
#include <conio.h>

using namespace std;

#define		TABLE_X		10
#define		TABLE_Y		(COM_Y+PLAYER_HT+1)
#define		TABLE_HT		3
#define		TABLE_WD		(2+2*5+1*4+2)
#define		PLAYER_HT	4
#define		PLAYER_WD	(1+8+1)		//	" COMPUTER ", "  HUMAN   "
#define		COM_X			(TABLE_X + (TABLE_WD - PLAYER_WD) / 2)
#define		COM_Y			2
#define		COM_BET_X	(COM_X + PLAYER_WD + 1)
#define		COM_BET_Y	(COM_Y + 1)
#define		MAN_X			COM_X
#define		MAN_Y			(TABLE_Y+TABLE_HT+1)
#define		MAN_BET_X	(MAN_X + PLAYER_WD + 1)
#define		MAN_BET_Y	(MAN_Y + 1)
#define		POT_X			(TABLE_X + TABLE_WD + 1)
#define		POT_Y			(TABLE_Y + 1)
#define		MENU_X		2
#define		MENU_Y		(MAN_Y+PLAYER_HT+1)
#define		N_MENU		3		//	メニュー選択肢数
#define		MENU_FOLD	0
#define		MENU_CC		1		//	Check/Call
#define		MENU_RAISE	2
#define		KEY_SPECIAL	0xe0
#define		KEY_LEFT		0x4b
#define		KEY_RIGHT	0x4d
#define		ACT_FOLD		1
#define		ACT_CC			2		//	Check/Call
#define		ACT_RAISE	3


TexasHoldem g_table;
int	g_comIX;
int	g_manIX;

int	g_menuIX;			//	選択されているメニューIX
const char *g_menu[] = {
	"Fold", "Check/Call", "Raise"
};

void draw_card(int x, int y, Card c)
{
	if( c.m_suit == Card::SPADES || c.m_suit == Card::CLUBS )
		setColor(COL_BLACK, COL_WHITE);
	else
		setColor(COL_RED, COL_WHITE);
	setCursorPos(x, y);
	coutW(c.toStringW());
}

void draw_com()
{
	setCursorPos(COM_X, COM_Y);
	setColor(COL_BLACK, COL_YELLOW);
	cout << " COMPUTER ";
	setColor(COL_GRAY, COL_BLACK);
	setCursorPos(COM_X, COM_Y+1);
	cout << "chip:" << g_table.chip(g_comIX);
	if( g_comIX == g_table.dealerIX() ) {
		setCursorPos(COM_X - 2, COM_Y);
		cout << "D";
	}
	if( g_comIX == g_table.BBIX() ) {
		setCursorPos(COM_X - 3, COM_Y + 1);
		cout << "BB";
	}
	if( g_comIX == g_table.SBIX() ) {
		setCursorPos(COM_X - 3, COM_Y + 1);
		cout << "SB";
	}
	setCursorPos(COM_BET_X, COM_BET_Y);
	cout << "bet:" << g_table.bet(g_comIX) << "      ";
	setColor(COL_BLACK, COL_CYAN);
	setCursorPos(COM_X, COM_Y+2);
	cout << "  ";
	setCursorPos(COM_X+3, COM_Y+2);
	cout << "  ";
}
void draw_human()
{
	setCursorPos(MAN_X, MAN_Y);
	setColor(COL_BLACK, COL_YELLOW);
	cout << "  HUMAN   ";
	setColor(COL_GRAY, COL_BLACK);
	setCursorPos(MAN_X, MAN_Y+1);
	cout << "chip:" << g_table.chip(g_manIX);
	setCursorPos(MAN_BET_X, MAN_BET_Y);
	cout << "bet:" << g_table.bet(g_manIX) << "      ";
	if( g_manIX == g_table.dealerIX() ) {
		setCursorPos(MAN_X - 2, MAN_Y);
		cout << "D";
	}
	if( g_manIX == g_table.BBIX() ) {
		setCursorPos(MAN_X - 3, MAN_Y + 1);
		cout << "BB";
	}
	if( g_manIX == g_table.SBIX() ) {
		setCursorPos(MAN_X - 3, MAN_Y + 1);
		cout << "SB";
	}
	Card c1, c2;
	g_table.getHoleCards(0, c1, c2);
	if( c1.m_suit == Card::SPADES || c1.m_suit == Card::CLUBS )
		setColor(COL_BLACK, COL_WHITE);
	else
		setColor(COL_RED, COL_WHITE);
	setCursorPos(MAN_X, MAN_Y+2);
	coutW(c1.toStringW());
	//cout << " ";
	if( c2.m_suit == Card::SPADES || c2.m_suit == Card::CLUBS )
		setColor(COL_BLACK, COL_WHITE);
	else
		setColor(COL_RED, COL_WHITE);
	setCursorPos(MAN_X+3, MAN_Y+2);
	coutW(c2.toStringW());
	//cout << "?? ??";
}
void draw_table()
{
	setColor(COL_WHITE, COL_DARK_GREEN);
	for(int k = 0; k < TABLE_HT; ++k) {
		setCursorPos(TABLE_X, k + TABLE_Y);
		for(int i = 0; i < TABLE_WD; ++i)
			cout << " ";
	}
	const std::vector<Card> &cc = g_table.communityCards();
	for (int i = 0; i < (int)cc.size(); ++i) {
		//setCursorPos(TABLE_X + 2 + i * 3, TABLE_Y + 1);
		draw_card(TABLE_X + 2 + i * 3, TABLE_Y + 1, cc[i]);
	}
	
	setColor(COL_GRAY, COL_BLACK);
	setCursorPos(POT_X, POT_Y);
	cout << "Pot:" << g_table.pot() << "        ";
}
void draw_menu()
{
	setCursorPos(MENU_X, MENU_Y);
	for (int i = 0; i < N_MENU; ++i) {
		if( i == g_menuIX )
			setColor(COL_BLACK, COL_WHITE);
		else
			setColor(COL_GRAY, COL_BLACK);
		cout << "[" << g_menu[i] << "]";
		setColor(COL_GRAY, COL_BLACK);
		cout << " ";
	}
	//setColor(COL_GRAY, COL_BLACK);
	//cout << "[Fold] [Check] [Call] [Raise] 0";
}
//	プリフロップ、フロップ、ターン、リバーの処理
//	全員コール：return true;
//	一人以外全員降りたら：return false;
bool turn()
{
	int pix = g_table.dealerIX() + 1;		//	現在の手番
	if( g_table.turn() == TexasHoldem::PRE_FLOP )
		pix += 2;
	for (;;) {
		draw_com();
		draw_human();
		draw_table();
		int act = 0;
		while( pix >= g_table.nPlayer() )
			pix -= g_table.nPlayer();
		if( pix == g_manIX ) {
			g_menuIX = MENU_CC;		//	Check/Call
			for (;;) {
				draw_menu();
				int ch = _getch();
				if( ch == 'Q' || ch == 'q' )
					return 0;
				if( ch == KEY_SPECIAL ) {		//	矢印キーなどの場合
					ch = _getch();
					if( ch == KEY_LEFT && g_menuIX != 0 )
						--g_menuIX;
					else if( ch == KEY_RIGHT && g_menuIX < N_MENU - 1)
						++g_menuIX;
				}
				if( ch == '\r' || ch == '\n' ) {	//	メニュー確定
					break;
				}
			}
			switch( g_menuIX ) {
				case MENU_FOLD:
					act = ACT_FOLD;
					break;
				case MENU_CC: {
					act = ACT_CC;
					break;
				}
				case MENU_RAISE:
					act = ACT_RAISE;
					break;
			}
		} else {
			Sleep(500);
			act = ACT_CC;
		}
		switch( act ) {
			case ACT_CC: {
				int b = g_table.call() - g_table.bet(pix);
				if( b != 0 ) {
					g_table.addBet(pix, b);
				}
				break;
			}
		}
		if( g_table.allCalled() )
			return true;
		++pix;
	}
}
int main()
{
	g_table.addPlayer(Player("YOU", 200));
	g_manIX = 0;
	g_table.addPlayer(Player("COM", 200, /*comp:*/true));
	g_comIX = 1;
	g_table.setDealer();
	g_table.dealHoleCards();
	setColor(COL_BLACK, COL_WHITE);
	setCursorPos(5, 0);
	cout << "*** Texas Hold'em Poker ***";
	for (;;) {
		if( turn() ) {
			//	まだ２人以上残っている場合
			g_table.dealFlop();
			if( turn() ) {
				//	まだ２人以上残っている場合
				g_table.dealTurn();
				if( turn() ) {
					//	まだ２人以上残っている場合
					g_table.dealTurn();
					turn();
				}
			}
		}
		//	精算処理
	}
	//getchar();
	return 0;
}
/*
◎ テーブル幅を最大コミュニティカード数に合わせる
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
*/
