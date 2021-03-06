#include "consoleUtil.h"
#include "Deck.h"
#include "TexasHoldem.h"
#include "poker.h"
#include <windows.h>
#include <conio.h>
#include <algorithm>    // std::find

using namespace std;

#define		CONS_WD		80
#define		CONS_HT		25
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
#define		N_MENU		4		//	メニュー選択肢数
#define		MENU_FOLD	0
#define		MENU_CC		1		//	Check/Call
#define		MENU_RAISE	2
#define		MENU_ALLIN	3
#define		HAND_HELP_X		(CONS_WD/2+8)
#define		HAND_HELP_Y		1
#define		VS_RAND_X			(CONS_WD/2+8)
#define		VS_RAND_Y			1
#define		KEY_SPECIAL	0xe0
#define		KEY_LEFT		0x4b
#define		KEY_RIGHT	0x4d
#define		KEY_UP			0x48
#define		KEY_DOWN	0x50
#define		ACT_FOLD		1
#define		ACT_CC			2		//	Check/Call
#define		ACT_RAISE	3


TexasHoldem g_table;
int	g_comIX;
int	g_manIX;
//int	g_raiseMin;		//	最小レイズ額
int	g_raise;				//	レイズ額
int	g_menuIX;			//	選択されているメニューIX
const char *g_menu[] = {
	"Fold", "Check/Call", "Raise", "AllIn", 0
};

namespace My {
	std::string to_string(int v)
	{
		if( !v ) return std::string("0");
		std::string str;
		while( v != 0 ) {
			str = std::string(1, v % 10 + '0') + str;
			v /= 10;
		}
		return str;
	}
	std::string to_percent_string(double v)
	{
		int i = (int)(v * 100);
		std::string str = to_string(i);
		str += '.';
		str += to_string((int)(v*1000) - i * 10);
		str += '%';
		return str;
	}
};
int getChar()
{
	int ch = _getch();
	if( ch == KEY_SPECIAL ) {		//	矢印キーなどの場合
		ch = _getch();
		if( ch == KEY_LEFT )
			return VK_LEFT;
		if( ch == KEY_RIGHT )
			return VK_RIGHT;
		if( ch == KEY_UP )
			return VK_UP;
		if( ch == KEY_DOWN )
			return VK_DOWN;
		return 0;
	} else
		return ch;
}

void draw_card(int x, int y, Card c)
{
	if( c.m_suit == Card::SPADES || c.m_suit == Card::CLUBS )
		setColor(COL_BLACK, COL_WHITE);
	else
		setColor(COL_RED, COL_WHITE);
	setCursorPos(x, y);
	coutW(c.toStringW());
}
void draw_hand_help()
{
	int x = HAND_HELP_X;
	int y = HAND_HELP_Y;
	draw_card(x+0*3, y, Card(Card::SPADES, Card::RANK_K));
	draw_card(x+1*3, y, Card(Card::SPADES, Card::RANK_Q));
	draw_card(x+2*3, y, Card(Card::SPADES, Card::RANK_J));
	draw_card(x+3*3, y, Card(Card::SPADES, Card::RANK_10));
	draw_card(x+4*3, y, Card(Card::SPADES, Card::RANK_9));
	setCursorPos(x+5*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << handName[STRAIGHT_FLUSH];
	++y;
	draw_card(x+0*3, y, Card(Card::HERTS, Card::RANK_5));
	draw_card(x+1*3, y, Card(Card::SPADES, Card::RANK_5));
	draw_card(x+2*3, y, Card(Card::CLUBS, Card::RANK_5));
	draw_card(x+3*3, y, Card(Card::DIAMONDS, Card::RANK_5));
	draw_card(x+4*3, y, Card(Card::DIAMONDS, Card::RANK_7));
	setCursorPos(x+5*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << handName[FOUR_OF_A_KIND];
	++y;
	draw_card(x+0*3, y, Card(Card::HERTS, Card::RANK_7));
	draw_card(x+1*3, y, Card(Card::SPADES, Card::RANK_7));
	draw_card(x+2*3, y, Card(Card::CLUBS, Card::RANK_7));
	draw_card(x+3*3, y, Card(Card::DIAMONDS, Card::RANK_3));
	draw_card(x+4*3, y, Card(Card::DIAMONDS, Card::RANK_3));
	setCursorPos(x+5*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << handName[FULL_HOUSE];
	++y;
	draw_card(x+0*3, y, Card(Card::CLUBS, Card::RANK_A));
	draw_card(x+1*3, y, Card(Card::CLUBS, Card::RANK_J));
	draw_card(x+2*3, y, Card(Card::CLUBS, Card::RANK_7));
	draw_card(x+3*3, y, Card(Card::CLUBS, Card::RANK_3));
	draw_card(x+4*3, y, Card(Card::CLUBS, Card::RANK_2));
	setCursorPos(x+5*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << handName[FLUSH];
	++y;
	draw_card(x+0*3, y, Card(Card::DIAMONDS, Card::RANK_10));
	draw_card(x+1*3, y, Card(Card::SPADES, Card::RANK_9));
	draw_card(x+2*3, y, Card(Card::DIAMONDS, Card::RANK_8));
	draw_card(x+3*3, y, Card(Card::HERTS, Card::RANK_7));
	draw_card(x+4*3, y, Card(Card::CLUBS, Card::RANK_6));
	setCursorPos(x+5*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << handName[STRAIGHT];
	++y;
	draw_card(x+0*3, y, Card(Card::HERTS, Card::RANK_7));
	draw_card(x+1*3, y, Card(Card::SPADES, Card::RANK_7));
	draw_card(x+2*3, y, Card(Card::CLUBS, Card::RANK_7));
	draw_card(x+3*3, y, Card(Card::CLUBS, Card::RANK_A));
	draw_card(x+4*3, y, Card(Card::DIAMONDS, Card::RANK_3));
	setCursorPos(x+5*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << handName[THREE_OF_A_KIND];
	++y;
	draw_card(x+0*3, y, Card(Card::HERTS, Card::RANK_A));
	draw_card(x+1*3, y, Card(Card::SPADES, Card::RANK_A));
	draw_card(x+2*3, y, Card(Card::CLUBS, Card::RANK_5));
	draw_card(x+3*3, y, Card(Card::SPADES, Card::RANK_5));
	draw_card(x+4*3, y, Card(Card::DIAMONDS, Card::RANK_Q));
	setCursorPos(x+5*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << handName[TWO_PAIR];
	++y;
	draw_card(x+0*3, y, Card(Card::CLUBS, Card::RANK_K));
	draw_card(x+1*3, y, Card(Card::DIAMONDS, Card::RANK_K));
	draw_card(x+2*3, y, Card(Card::CLUBS, Card::RANK_10));
	draw_card(x+3*3, y, Card(Card::HERTS, Card::RANK_5));
	draw_card(x+4*3, y, Card(Card::DIAMONDS, Card::RANK_2));
	setCursorPos(x+5*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << handName[ONE_PAIR];
	++y;
	draw_card(x+0*3, y, Card(Card::CLUBS, Card::RANK_K));
	draw_card(x+1*3, y, Card(Card::DIAMONDS, Card::RANK_J));
	draw_card(x+2*3, y, Card(Card::CLUBS, Card::RANK_10));
	draw_card(x+3*3, y, Card(Card::HERTS, Card::RANK_5));
	draw_card(x+4*3, y, Card(Card::DIAMONDS, Card::RANK_2));
	setCursorPos(x+5*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << handName[HIGH_CARD];
}
void clear_rightHalf()
{
	setColor(COL_GRAY, COL_BLACK);
	for (int y = 0; y < CONS_HT; ++y) {
		setCursorPos(CONS_WD/2, y);
		int WD = y < CONS_HT - 1 ? CONS_WD/2 : CONS_WD/2 - 1;
		for (int x = 0; x < WD; ++x) {
			cout << ' ';
		}
	}
}
//	各ターンでの vs ランダム勝率を表示
void draw_winSplit_vsRand(int x, int y, Card c1, Card c2)
{
	const std::vector<Card> &cc = g_table.communityCards();
	std::vector<Card> v;
	double ws = calcWinSplitProb(c1, c2, v);
	setCursorPos(x - 6, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << My::to_percent_string(ws);
	draw_card(x, y, c1);
	draw_card(x+3, y, c2);
	if( cc.size() < 3 ) return;
	++y;
	v.push_back(cc[0]);
	v.push_back(cc[1]);
	v.push_back(cc[2]);
	ws = calcWinSplitProb(c1, c2, v);
	setCursorPos(x - 6, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << My::to_percent_string(ws);
	draw_card(x, y, c1);
	draw_card(x+3, y, c2);
	for (int i = 0; i < (int)v.size(); ++i) {
		draw_card(x+3*(i+2), y, v[i]);
	}
	if( cc.size() < 4 ) return;
	++y;
	v.push_back(cc[3]);
	ws = calcWinSplitProb(c1, c2, v);
	setCursorPos(x - 6, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << My::to_percent_string(ws);
	draw_card(x, y, c1);
	draw_card(x+3, y, c2);
	for (int i = 0; i < (int)v.size(); ++i) {
		draw_card(x+3*(i+2), y, v[i]);
	}
	if( cc.size() < 5 ) return;
	++y;
	v.push_back(cc[4]);
	ws = calcWinSplitProb(c1, c2, v);
	setCursorPos(x - 6, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << My::to_percent_string(ws);
	draw_card(x, y, c1);
	draw_card(x+3, y, c2);
	for (int i = 0; i < (int)v.size(); ++i) {
		draw_card(x+3*(i+2), y, v[i]);
	}
}
void draw_winSplit_vsRand()
{
	clear_rightHalf();
	int x = VS_RAND_X;
	int y = VS_RAND_Y;
	Card c1, c2;
	g_table.getHoleCards(g_comIX, c1, c2);
	draw_winSplit_vsRand(x, y, c1, c2);
	g_table.getHoleCards(g_manIX, c1, c2);
	draw_winSplit_vsRand(x, y+5, c1, c2);
}
void draw_com(bool open = false);
void draw_com(bool open)
{
	setCursorPos(COM_X, COM_Y);
	setColor(COL_BLACK, COL_YELLOW);
	cout << " COMPUTER ";
	setColor(COL_GRAY, COL_BLACK);
	setCursorPos(COM_X, COM_Y+1);
	cout << "chip:" << g_table.chip(g_comIX) << "    ";
		setCursorPos(COM_X - 2, COM_Y);
	if( g_comIX == g_table.dealerIX() ) {
		cout << "D";
	} else
		cout << " ";
	if( g_comIX == g_table.BBIX() ) {
		setCursorPos(COM_X - 3, COM_Y + 1);
		cout << "BB";
	}
	if( g_comIX == g_table.SBIX() ) {
		setCursorPos(COM_X - 3, COM_Y + 1);
		cout << "SB";
	}
	setCursorPos(COM_BET_X, COM_BET_Y);
	if( g_table.pot() != 0 )
		cout << "bet:" << g_table.bet(g_comIX) << "      ";
	else
		cout << "          ";
	if( open ) {
		Card c1, c2;
		g_table.getHoleCards(g_comIX, c1, c2);
		draw_card(COM_X, COM_Y+2, c1);
		draw_card(COM_X+3, COM_Y+2, c2);
	} else {
		setColor(COL_BLACK, COL_CYAN);
		setCursorPos(COM_X, COM_Y+2);
		cout << "  ";
		setCursorPos(COM_X+3, COM_Y+2);
		cout << "  ";
	}
}
void draw_human()
{
	setCursorPos(MAN_X, MAN_Y);
	setColor(COL_BLACK, COL_YELLOW);
	cout << "  HUMAN   ";
	setColor(COL_GRAY, COL_BLACK);
	setCursorPos(MAN_X, MAN_Y+1);
	cout << "chip:" << g_table.chip(g_manIX) << "    ";
	setCursorPos(MAN_BET_X, MAN_BET_Y);
	if( g_table.pot() != 0 )
		cout << "bet:" << g_table.bet(g_manIX) << "      ";
	else
		cout << "          ";
	setCursorPos(MAN_X - 2, MAN_Y);
	if( g_manIX == g_table.dealerIX() ) {
		cout << "D";
	} else
		cout << " ";
	if( g_manIX == g_table.BBIX() ) {
		setCursorPos(MAN_X - 3, MAN_Y + 1);
		cout << "BB";
	}
	if( g_manIX == g_table.SBIX() ) {
		setCursorPos(MAN_X - 3, MAN_Y + 1);
		cout << "SB";
	}
	Card c1, c2;
	g_table.getHoleCards(g_manIX, c1, c2);
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
void show_message(const char *ptr, int dy = 0)
//void show_message(const char *ptr)
{
	setCursorPos(MENU_X, MENU_Y + dy);
	setColor(COL_GRAY, COL_BLACK);
	cout << ptr;
}
void clear_menu(int dy = 0)
{
	setCursorPos(MENU_X, MENU_Y+dy);
	setColor(COL_GRAY, COL_BLACK);
	for (int i = 0; i < CONS_WD/2; ++i) {
		cout << " ";
	}
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
	cout << g_raise << "    ";
}
void print_result(const uint odr[])
{
	setCursorPos(MENU_X, MENU_Y);
	setColor(COL_GRAY, COL_BLACK);
	if( odr[g_manIX] > odr[g_comIX] ) {
		cout << "+++ You Win +++";
		g_table.winner(g_manIX);
	} else if( odr[g_manIX] < odr[g_comIX] ) {
		cout << "--- Com Win ---";
		g_table.winner(g_comIX);
	} else {
		cout << "=== Split ===";
		std::vector<int> v;
		v.push_back(g_manIX);
		v.push_back(g_comIX);
		g_table.split(v);
	}
	cout << " (Push Enter Key)";
}
void show_com_act(const char *act)
{
	setColor(COL_GRAY, COL_BLACK);
	setCursorPos(COM_X, COM_Y + 3);
	cout << act << "            ";
}
//	プリフロップ、フロップ、ターン、リバーの処理
//	全員コール：return true;
//	一人以外全員降りたら：return false;
bool round()
{
	show_com_act("");
	if( g_table.isAllIn() ) {
		draw_com();
		draw_human();
		draw_table();
		Sleep(500);
		return true;
	}
	const int nPlayer = g_table.nPlayer();
	std::vector<bool> done(nPlayer, false);		//	全員がベット/コールしたかどうか
	int pix = g_table.dealerIX() + 1;		//	現在の手番
	if( g_table.round() == TexasHoldem::PRE_FLOP )
		pix += 2;
	//	最後のレイズ額（レイズ額は、これ以上）
	//	$2 に対して $5 レイズした場合、lastRaise は $3
	//	リレイズする場合は $5 + $3 = $8 以上となる
	int lastRaise = 0;
	for (;;) {
		draw_com();
		draw_human();
		draw_table();
		int act = 0;
		while( pix >= nPlayer )
			pix -= nPlayer;
		int raiseUnit = g_table.BB();		//	undone: 既にレイズされている場合はそれ以上
		if( g_table.round() >= TexasHoldem::TURN )
			raiseUnit *= 2;
		if( pix == g_manIX ) {
			g_raise = raiseUnit;
			g_menuIX = MENU_CC;		//	Check/Call
			int chip = g_table.chip(g_manIX);
			for (;;) {
				if( g_menuIX == MENU_CC )
					g_raise = g_table.call() - g_table.bet(pix);	//	コール必要額
				else if( g_menuIX == MENU_ALLIN )
					g_raise = chip;
				draw_menu();
				int ch = getChar();
				//if( ch == 'Q' || ch == 'q' )
				//	return 0;
				if( ch == VK_LEFT && g_menuIX != 0 )
						--g_menuIX;
				else if( ch == VK_RIGHT && g_menuIX < N_MENU - 1)
					++g_menuIX;
				else if( ch == VK_DOWN && g_raise != 0 ) {
#if	1
					if( (g_raise -= raiseUnit) < 0 )
						g_raise = 0;
					if( g_raise >= raiseUnit )
						g_menuIX = MENU_RAISE;
					else
						g_menuIX = MENU_CC;
#else
					if( g_raise % raiseUnit == 0 )
						g_raise -= raiseUnit;
					else
						g_raise -= g_raise % raiseUnit;
					if( g_raise != 0 )
						g_menuIX = MENU_RAISE;
					else
						g_menuIX = MENU_CC;
#endif
				} else if( ch == VK_UP && g_raise < chip ) {
#if	1
					if( g_raise == g_table.BB() / 2 )		//	SB の場合
						g_raise = g_table.BB() / 2;
					if( (g_raise += raiseUnit) > chip ) {
						g_raise = chip;
						g_menuIX = MENU_ALLIN;
					} else
						g_menuIX = MENU_RAISE;
#else
					if( g_raise < raiseUnit ) {		//	SB の場合
						//	最小額は SB + BB、以降は BB 単位
						if( (g_raise = raiseUnit) > chip ) {
							g_raise = chip;
							g_menuIX = MENU_ALLIN;
						} else
							g_menuIX = MENU_RAISE;
					} else if( (g_raise += raiseUnit) > chip ) {
						g_raise = chip;
						g_menuIX = MENU_ALLIN;
					} else
						g_menuIX = MENU_RAISE;
#endif
				} else if( ch == '\r' || ch == '\n' ) {	//	メニュー確定
					break;
				} else if( ch == 'p' || ch == 'P' ) {
					//	確率・スレッシュホールド表示
					Card c1, c2;
					g_table.getHoleCards(g_manIX, c1, c2);
					double ws = calcWinSplitProb(c1, c2, g_table.communityCards());
					show_message("WinSplit = ", 1);
					cout << ws*100 << "%";
					int call = g_table.call() - g_table.bet(pix);
					double th = calcThreshold(g_table.pot(), call);
					cout << ", threshold = " << th*100 << "%";
				}
			}
			clear_menu();
			clear_menu(1);
			switch( g_menuIX ) {
				case MENU_FOLD:
					act = ACT_FOLD;
					break;
				case MENU_CC: {
					act = ACT_CC;
					break;
				}
				case MENU_RAISE:
				case MENU_ALLIN:
					act = ACT_RAISE;
					break;
			}
		} else {
			//	コンピュータの手番
			Card c1, c2;
			g_table.getHoleCards(pix, c1, c2);
			double ws = calcWinSplitProb(c1, c2, g_table.communityCards());
			//show_message("WinSplit = ", 1);
			//cout << ws*100 << "%";
			int call = g_table.call() - g_table.bet(pix);
			double th = calcThreshold(g_table.pot(), call);
			//Sleep(500);
			if( ws < th ) {
				act = ACT_FOLD;
				show_com_act("fold");
			} else {
				int b = g_table.call() - g_table.bet(pix);		//	コール必要額
				if( !b && ws >= 0.55 ) {
					int t = (int)((ws - 0.50)/0.05);
					g_raise = min(t * raiseUnit, g_table.chip(pix));
					act = ACT_RAISE;
					std::string str("raise ");
					str += My::to_string(g_raise);
					show_com_act(str.c_str());
				} else {
					act = ACT_CC;
					if( !b )
						show_com_act("check");
					else
						show_com_act("call");
				}
			}
		}
		done[pix] = true;
		switch( act ) {
			case ACT_FOLD:
				g_table.fold(pix);
				return false;
			case ACT_CC: {
				int b = min(g_table.call() - g_table.bet(pix), g_table.chip(pix));
				if( b != 0 ) {
					g_table.addBet(pix, b);
				}
				break;
			}
			case ACT_RAISE:
				g_table.addBet(pix, g_raise);
				break;
		}
		draw_com();
		draw_human();
		if( (g_table.allCalled() /*|| g_table.isAllIn()*/)
			&& std::find(done.begin(), done.end(), false) == done.end() )
		{
			return true;
		}
		++pix;
	}
}
void game()
{
	g_table.setDealer();
	while ( g_table.chip(g_manIX) != 0 && g_table.chip(g_comIX) != 0 ) {
		g_table.dealHoleCards();
		if( round() ) {
			//	まだ２人以上残っている場合
			g_table.dealFlop();
			if( round() ) {
				//	まだ２人以上残っている場合
				g_table.dealTurn();
				if( round() ) {
					//	まだ２人以上残っている場合
					g_table.dealRiver();
					//if( !g_table.isAllIn() )
						round();
				}
			}
		}
		//draw_com(/*open:*/true);
		//	精算処理
		setCursorPos(MENU_X, MENU_Y);
		setColor(COL_GRAY, COL_BLACK);
		bool folded = true;
		if( g_table.folded(g_comIX) ) {
			cout << "+++ You Win +++ (Push Enter Key)";
			g_table.winner(g_manIX);
		} else if( g_table.folded(g_manIX) ) {
			cout << "--- Com Win --- (Push Enter Key)";
			g_table.winner(g_comIX);
		} else {
			folded = false;
			draw_com(/*open:*/true);
			uint odr[2];		//	undone: ３人以上対応
			uint hand[2];
			std::vector<Card> v;
			for (int i = 0; i < g_table.nPlayer(); ++i) {
				g_table.playersCard(i, v);
				hand[i] = checkHand(v, odr[i]);
			}
			setColor(COL_GRAY, COL_BLACK);
			setCursorPos(COM_X, COM_Y + 3);
			cout << handName[hand[g_comIX]] << "        ";
			setCursorPos(MAN_X, MAN_Y + 3);
			cout << handName[hand[g_manIX]] << "        ";
			print_result(odr);
		}
		//draw_com(/*open:*/!folded);		//	どちらかが降りた場合は手札をさらさない
		int ch = getChar();
		if( ch == 'p' )
			draw_winSplit_vsRand();
		clear_menu();
		draw_com(/*open:*/!folded);		//	どちらかが降りた場合は手札をさらさない
		draw_human();
		draw_table();
		show_message("Push Enter Key");
		getChar();
		//clear_winSplit();
		//	役名表示部分を消去
		setColor(COL_GRAY, COL_BLACK);
		setCursorPos(COM_X, COM_Y + 3);
		cout << "               ";
		setCursorPos(MAN_X, MAN_Y + 3);
		cout << "               ";
		clear_menu();
		g_table.forwardDealer();
	}
}
int main()
{
	setColor(COL_BLACK, COL_WHITE);
	setCursorPos(5, 0);
	cout << "*** Texas Hold'em Poker ***";
	draw_hand_help();
	g_table.addPlayer(Player("YOU", 200));
	g_manIX = 0;
	g_table.addPlayer(Player("COM", 200, /*comp:*/true));
	g_comIX = 1;
	for (;;) {
		game();
		if( g_table.chip(g_manIX) == 0 ) {
			show_message("You Busted.");
			g_table.setChip(g_manIX, 200);		//	次のゲーム用
		} else {
			show_message("Computer are Busted.");
			g_table.setChip(g_comIX, 200);		//	次のゲーム用
		}
		cout << " Try Again ? [y/n]";
		for (;;) {
			int ch = getChar();
			if( ch == 'n' || ch == 'N' )
				return 0;
			if( ch == 'y' || ch == 'Y' )
				break;
		}
		clear_menu();
	}
}
