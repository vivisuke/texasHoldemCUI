#include "consoleUtil.h"
#include "Deck.h"
#include "TexasHoldem.h"

using namespace std;

#define		TABLE_X		10
#define		TABLE_Y		(COM_Y+PLAYER_HT+1)
#define		TABLE_HT		3
#define		TABLE_WD		(2+2*5+1*4+2)
#define		PLAYER_HT	4
#define		PLAYER_WD	(1+8+1)		//	" COMPUTER ", "  HUMAN   "
#define		COM_X			(TABLE_X + (TABLE_WD - PLAYER_WD) / 2)
#define		COM_Y			2
#define		MAN_X			COM_X
#define		MAN_Y			(TABLE_Y+TABLE_HT+1)
#define		MENU_X		2
#define		MENU_Y		(MAN_Y+PLAYER_HT+1)

TexasHoldem g_table;
int	g_comIX;
int	g_manIX;

void draw_com()
{
	setCursorPos(COM_X, COM_Y);
	setColor(COL_BLACK, COL_YELLOW);
	cout << " COMPUTER  ";
	setColor(COL_GRAY, COL_BLACK);
	setCursorPos(COM_X, COM_Y+1);
	cout << "chip:200";
	setCursorPos(COM_X, COM_Y+2);
	cout << "?? ??";
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
}
void draw_human()
{
	setCursorPos(MAN_X, MAN_Y);
	setColor(COL_BLACK, COL_YELLOW);
	cout << "  HUMAN   ";
	setColor(COL_GRAY, COL_BLACK);
	setCursorPos(MAN_X, MAN_Y+1);
	cout << "chip:200";
	setCursorPos(MAN_X, MAN_Y+2);
	Card c1, c2;
	g_table.getHoleCards(0, c1, c2);
	coutW(c1.toStringW());
	cout << " ";
	coutW(c2.toStringW());
	//cout << "?? ??";
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
}
void draw_table()
{
	setColor(COL_WHITE, COL_DARK_GREEN);
	for(int k = 0; k < TABLE_HT; ++k) {
		setCursorPos(TABLE_X, k + TABLE_Y);
		for(int i = 0; i < TABLE_WD; ++i)
			cout << " ";
	}
	setCursorPos(TABLE_X + 2, TABLE_Y + 1);
	cout << "?? ?? ?? ?? ??";
}
void draw_menu()
{
	setCursorPos(MENU_X, MENU_Y);
	setColor(COL_GRAY, COL_BLACK);
	cout << "[Fold] [Check] [Call] [Raise] 0";
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
	draw_com();
	draw_human();
	draw_table();
	draw_menu();
	getchar();
	return 0;
}
/*
◎ テーブル幅を最大コミュニティカード数に合わせる
◎ ディーラマーク表示
◎ BB, SB マーク表示
*/
