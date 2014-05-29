#include "consoleUtil.h"
#include "Deck.h"
#include "TexasHoldem.h"
#include "poker.h"
#include <windows.h>
#include <conio.h>
#include <algorithm>    // std::find
#include <assert.h>

using namespace std;

/*
������ *** TexasHold'em Poker *** ������������������
���@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@��
���@�@�@�@�@�����������������������������@�@�@�@�@�� 
���@�@�@�@�@��COMPUTER-2����COMPUTER-3���@�@�@�@�@�� 
���@�@�@�@�@��chip:200�@����chip:200�@���@�@�@�@�@�� 
���@�@�@�@�@��bet:0�@�@ ����bet:0�@�@ ���@�@�@�@�@�� 
���@�@�@�@�@��[] []�@�@ ����[] []�@�@ ���@�@�@�@�@�� 
���@�@�@�@�@��call�@�@�@����call�@�@�@���@�@�@�@�@�� 
���@�@�@�@�@�����������������������������@�@�@�@�@�� 
����������������������������������������������������
����COMPUTER-1����River �@�@�@�@�@����COMPUTER-4����
����chip:200�@�����@�@�@�@�@�@�@�@����chip:200�@����
����bet:0�@�@ ���� [] [] [] [] [] ����bet:0�@�@ ����
����[] []�@�@ �����@�@�@�@�@�@�@�@����[] []�@�@ ����
����call�@�@�@����pot:80�@�@�@�@�@����call�@�@�@����
����������������������������������������������������
���@�@�@�@�@�@�@�@ ���������������@�@�@�@�@�@�@�@ ��
���@�@�@�@�@�@�@�@ ��HUMAN�@�@ ���@�@�@�@�@�@�@�@ ��
���@�@�@�@�@�@�@�@ ��chip:200�@���@�@�@�@�@�@�@�@ ��
���@�@�@�@�@�@�@�@ ��bet:0�@�@ ���@�@�@�@�@�@�@�@ ��
���@�@�@�@�@�@�@�@ ��[] []�@�@ ���@�@�@�@�@�@�@�@ ��
���@�@�@�@�@�@�@�@ ��call�@�@�@���@�@�@�@�@�@�@�@ ��
���@�@�@�@�@�@�@�@ ���������������@�@�@�@�@�@�@�@ ��
���@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@��
���@�yFold�z�yCheck/Call�z�yRaise�z�yAllIn�z100�@ ��
���@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@��
����������������������������������������������������
*/

#define		N_PLAYER		5
#define		CONS_WD		80
#define		CONS_HT		25
#define		TABLE_X		11
#define		TABLE_Y		(COM_Y+PLAYER_HT+1)
#define		TABLE_HT		5
#define		TABLE_WD		(2+2*5+1*4+2)
#define		PLAYER_HT	5
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
#define		N_MENU		4		//	���j���[�I������
#define		MENU_FOLD	0
#define		MENU_CC		1		//	Check/Call
#define		MENU_RAISE	2
#define		MENU_ALLIN	3
#define		HAND_HELP_X		(CONS_WD/2+8)
#define		HAND_HELP_Y		1
#define		VS_RAND_X			(CONS_WD/2+8+4)
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
//int	g_raiseMin;		//	�ŏ����C�Y�z
int	g_raise;				//	���C�Y�z
int	g_menuIX;			//	�I������Ă��郁�j���[IX
const char *g_menu[] = {
	"Fold", "Check/Call", "Raise", "AllIn", 0
};
const char *shortHandName[] = {
	"highCard",
	"onePair",
	"twoPair",
	"3OfAKind",
	"straight",
	"flush",
	"fullHouse",
	"4OfAKind",
	"strtFlush",
};
//typedef std::pair<int, int> CurPos;
struct CurPos {
	int	m_x;
	int m_y;
};
const CurPos playerPos[] = {
	{16, 14},
	{1, 8},
	{11, 2},
	{21, 2},
	{31, 8},
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
	std::string to_string(int v, int width)		//	width < 0 �̏ꍇ�͍��l��
	{
		std::string str = to_string(v);
		if( width > 0 && width - str.size() > 0 )
			str = std::string(width - str.size(), ' ') + str;
		else if( width < 0 && -width - str.size() > 0 )
			str += std::string(-width - str.size(), ' ');
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
	if( ch == KEY_SPECIAL ) {		//	���L�[�Ȃǂ̏ꍇ
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
//	�e�^�[���ł� vs �����_��������\��
void draw_winSplit_vsRand(int ix, int x, int y, Card c1, Card c2)
{
	const std::vector<Card> &cc = g_table.communityCards();
	std::vector<Card> v;
	double ws = calcWinSplitProb(c1, c2, v, N_PLAYER);
	setCursorPos(x - 6, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << My::to_percent_string(ws);
	draw_card(x, y, c1);
	draw_card(x+3, y, c2);
	setCursorPos(x + 7*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << g_table.bet(ix, 0) << "   ";
	if( cc.size() < 3 ) return;
	++y;
	v.push_back(cc[0]);
	v.push_back(cc[1]);
	v.push_back(cc[2]);
	ws = calcWinSplitProb(c1, c2, v, N_PLAYER);
	setCursorPos(x - 6, y);
	cout << My::to_percent_string(ws);
	draw_card(x, y, c1);
	draw_card(x+3, y, c2);
	for (int i = 0; i < (int)v.size(); ++i) {
		draw_card(x+3*(i+2), y, v[i]);
	}
	setCursorPos(x + 7*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << g_table.bet(ix, 1) << "   ";
	if( cc.size() < 4 ) return;
	++y;
	v.push_back(cc[3]);
	ws = calcWinSplitProb(c1, c2, v, N_PLAYER);
	setCursorPos(x - 6, y);
	//setColor(COL_GRAY, COL_BLACK);
	cout << My::to_percent_string(ws);
	draw_card(x, y, c1);
	draw_card(x+3, y, c2);
	for (int i = 0; i < (int)v.size(); ++i) {
		draw_card(x+3*(i+2), y, v[i]);
	}
	setCursorPos(x + 7*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << g_table.bet(ix, 2) << "   ";
	if( cc.size() < 5 ) return;
	++y;
	v.push_back(cc[4]);
	ws = calcWinSplitProb(c1, c2, v, N_PLAYER);
	setCursorPos(x - 6, y);
	//setColor(COL_GRAY, COL_BLACK);
	cout << My::to_percent_string(ws);
	draw_card(x, y, c1);
	draw_card(x+3, y, c2);
	for (int i = 0; i < (int)v.size(); ++i) {
		draw_card(x+3*(i+2), y, v[i]);
	}
	setCursorPos(x + 7*3, y);
	setColor(COL_GRAY, COL_BLACK);
	cout << g_table.bet(ix, 3) << "   ";
}
void draw_winSplit_vsRand()
{
	clear_rightHalf();
	int x = VS_RAND_X;
	int y = VS_RAND_Y;
	Card c1, c2;
#if	1
	for (int i = 0; i < N_PLAYER; ++i, y+=5) {
		g_table.getHoleCards(i, c1, c2);
		draw_winSplit_vsRand(i, x, y, c1, c2);
	}
#else
	g_table.getHoleCards(g_comIX, c1, c2);
	draw_winSplit_vsRand(x, y, c1, c2);
	g_table.getHoleCards(g_manIX, c1, c2);
	draw_winSplit_vsRand(x, y+5, c1, c2);
#endif
}
void show_act(int x, int y, const char *act)
{
	setColor(COL_GRAY, COL_BLACK);
	setCursorPos(x, y + 4);
	std::string str(act);
	if( str.size() < 10 )
		str += std::string(10 - str.size(), ' ');
	cout << str;
}
void draw_player(int ix, int x, int y,
							bool open = false,		//	�J�[�h�I�[�v��
							int nCard = 2)			//	�J�[�h�\������
{
	setCursorPos(x, y);
	setColor(COL_BLACK, g_table.folded(ix) ? COL_LIGHT_GRAY : COL_YELLOW);
	cout << " " << g_table.player(ix).m_name << " ";
	setColor(COL_GRAY, COL_BLACK);
	if( ix == g_table.dealerIX() )
		cout << " D ";
	else if( ix == g_table.BBIX() )
		cout << " BB";
	else if( ix == g_table.SBIX() )
		cout << " SB";
	else
		cout << "   ";
	setCursorPos(x, y+1);
	cout << "chip:" << My::to_string(g_table.chip(ix), 4);
	assert( g_table.chip(ix) >= 0 );
	setCursorPos(x, y + 2);
	if( g_table.pot() != 0 )
		cout << "bet:" << My::to_string(g_table.bet(ix), 5);
	else
		cout << "          ";
	//if( !nCard ) {
		setColor(COL_BLACK, COL_BLACK);
		setCursorPos(x, y+3);
		cout << "  ";
		setCursorPos(x+3, y+3);
		cout << "  ";
	//}
	if( open ) {
		Card c1, c2;
		g_table.getHoleCards(ix, c1, c2);
		if( nCard >= 1 )
			draw_card(x, y+3, c1);
		if( nCard >= 2 )
			draw_card(x+3, y+3, c2);
	} else {
		setColor(COL_BLACK, COL_CYAN);
		if( nCard >= 1 ) {
			setCursorPos(x, y+3);
			cout << "  ";
		}
		if( nCard >= 2 ) {
			setCursorPos(x+3, y+3);
			cout << "  ";
		}
	}
	//if( g_table.folded(ix) )
	//	show_act(x, y, "folded");
}
void draw_com(bool open = false)
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
		draw_card(TABLE_X + 2 + i * 3, TABLE_Y + 2, cc[i]);
	}
	
	setColor(COL_GRAY, COL_DARK_GREEN);
	setCursorPos(TABLE_X+1, TABLE_Y);
	switch( g_table.round() ) {
		case 0:	cout << "Pre-Flop:";	break;
		case 1:	cout << "Flop:";	break;
		case 2:	cout << "Turn:";	break;
		case 3:	cout << "River:";	break;
	}
	setCursorPos(TABLE_X+1, TABLE_Y + TABLE_HT - 1);
	cout << "Pot:" << g_table.pot();
	//setColor(COL_GRAY, COL_DARK_GREEN);
	cout << "        ";
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
int human_act(int raiseUnit)
{
	int pix = g_manIX;
	g_raise = raiseUnit;
	g_menuIX = MENU_CC;		//	Check/Call
	int chip = g_table.chip(g_manIX);
	for (;;) {
		if( g_menuIX == MENU_CC )
			g_raise = g_table.call() - g_table.bet(pix);	//	�R�[���K�v�z
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
			if( g_raise == g_table.BB() / 2 )		//	SB �̏ꍇ
				g_raise = g_table.BB() / 2;
			if( (g_raise += raiseUnit) > chip ) {
				g_raise = chip;
				g_menuIX = MENU_ALLIN;
			} else
				g_menuIX = MENU_RAISE;
#else
			if( g_raise < raiseUnit ) {		//	SB �̏ꍇ
				//	�ŏ��z�� SB + BB�A�ȍ~�� BB �P��
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
		} else if( ch == '\r' || ch == '\n' ) {	//	���j���[�m��
			break;
		} else if( ch == 'p' || ch == 'P' ) {
			//	�m���E�X���b�V���z�[���h�\��
			Card c1, c2;
			g_table.getHoleCards(g_manIX, c1, c2);
			double ws = calcWinSplitProb(c1, c2, g_table.communityCards(), N_PLAYER);
			show_message("WinSplit = ", 1);
			cout << ws*100 << "%";
			int call = g_table.call() - g_table.bet(pix);
			double th = calcThreshold(g_table.pot(), call);
			cout << ", threshold = " << th*100 << "%";
		}
	}
	clear_menu();
	clear_menu(1);	//	���j���[�̎��̍s������
	int act = 0;
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
	//done[pix] = true;
	return act;
}
//	�v���t���b�v�A�t���b�v�A�^�[���A���o�[�̏���
//	�S���R�[���Freturn true;
//	��l�ȊO�S���~�肽��Freturn false;
bool round()
{
	for (int ix = 0; ix < g_table.nPlayer(); ++ix) {
		show_act(playerPos[ix].m_x, playerPos[ix].m_y, "");
	}
	//show_act(COM_X, COM_Y, "");
	if( g_table.isAllIn() ) {
		for (int ix = 0; ix < g_table.nPlayer(); ++ix) {
			draw_player(ix, playerPos[ix].m_x, playerPos[ix].m_y, /*open:*/ix == g_manIX);
		}
		draw_table();
		Sleep(500);
		return true;
	}
	const int nPlayer = g_table.nPlayer();
	std::vector<bool> done(nPlayer, false);		//	�S�����x�b�g/�R�[���������ǂ���
	int pix = g_table.dealerIX() + 1;		//	���݂̎��
	if( g_table.round() == TexasHoldem::PRE_FLOP )
		pix += 2;
	//	�Ō�̃��C�Y�z�i���C�Y�z�́A����ȏ�j
	//	$2 �ɑ΂��� $5 ���C�Y�����ꍇ�AlastRaise �� $3
	//	�����C�Y����ꍇ�� $5 + $3 = $8 �ȏ�ƂȂ�
	int lastRaiseIX = -1;
	int lastRaise = 0;
	int raiseCnt = 0;		//	���C�Y��
	for (;;) {
		for (int ix = 0; ix < g_table.nPlayer(); ++ix) {
			draw_player(ix, playerPos[ix].m_x, playerPos[ix].m_y, /*open:*/ix == g_manIX);
			//draw_player(ix, playerPos[ix].m_x, playerPos[ix].m_y, /*open:*/true);		//	for Debug
		}
		draw_table();
		int act = 0;
		while( pix >= nPlayer )
			pix -= nPlayer;
		int px = playerPos[pix].m_x;		//	��Ԃ̃v�����[�̕\���ʒu
		int py = playerPos[pix].m_y;
		int raiseUnit = g_table.BB();		//	undone: ���Ƀ��C�Y����Ă���ꍇ�͂���ȏ�
		if( g_table.round() >= TexasHoldem::TURN )
			raiseUnit *= 2;
		if( g_table.isFolded(pix) ) {
			done[pix] = true;
			++pix;
			continue;
		}
		if( pix == g_manIX ) {		//	�l�Ԃ̎��
			//act = ACT_CC;		//	for Test
			act = human_act(raiseUnit);
		} else {			//	�R���s���[�^�̎��
			Card c1, c2;
			g_table.getHoleCards(pix, c1, c2);
			double ws = calcWinSplitProb(c1, c2, g_table.communityCards(), N_PLAYER);
			//show_message("WinSplit = ", 1);
			//cout << ws*100 << "%";
			int call = g_table.call() - g_table.bet(pix);
			double th = calcThreshold(g_table.pot(), call);
			if( !g_table.round() && th <= 0.5 )		//	�v���t���b�v�ł͂��܂�t�H�[���h���Ȃ�
				th /= 2;
			//Sleep(500);
			if( ws < th ) {
				act = ACT_FOLD;
				show_act(px, py, "fold");
			} else {
				int b = g_table.call() - g_table.bet(pix);		//	�R�[���K�v�z
				if( g_table.chip(pix) > b && ws >= 0.60 + 0.05 * raiseCnt ) {
					int t = (int)((ws - 0.50)/0.05);
					g_raise = min(t * raiseUnit, g_table.chip(pix) - b);
					act = ACT_RAISE;
					std::string str("raise ");
					str += My::to_string(g_raise);
					show_act(px, py, str.c_str());
					g_raise += b;
				} else {
					act = ACT_CC;
					if( !b )
						show_act(px, py, "check");
					else
						show_act(px, py, "call");
				}
			}
		}
		done[pix] = true;
		switch( act ) {
			case ACT_FOLD:
				g_table.fold(pix);
				if( g_table.nNotFoldPlayer() == 1 ) {
					draw_player(pix, playerPos[pix].m_x, playerPos[pix].m_y, pix == g_manIX);
					return false;
				}
				break;
			case ACT_CC: {
				int b = min(g_table.call() - g_table.bet(pix), g_table.chip(pix));
				if( b != 0 ) {
					g_table.addBet(pix, g_table.round(), b);
				}
				break;
			}
			case ACT_RAISE:
				++raiseCnt;
				g_table.addBet(pix, g_table.round(), g_raise);
				break;
		}
		draw_player(pix, playerPos[pix].m_x, playerPos[pix].m_y, pix == g_manIX);
		if( (g_table.allCalled() /*|| g_table.isAllIn()*/)
			&& std::find(done.begin(), done.end(), false) == done.end() )
		{
			return true;
		}
		++pix;
	}
}
void animate_dealHoleCards()
{
	draw_table();
	for (int i = 0; i <= N_PLAYER * 2; ++i) {
		for (int ix = 0; ix < g_table.nPlayer(); ++ix) {
			int n = 0;
			if( ix < i ) ++n;
			if( ix + N_PLAYER < i ) ++n;
			draw_player(ix, playerPos[ix].m_x, playerPos[ix].m_y, /*open:*/false, /*nCard:*/n);
		}
		Sleep(250);
	}
}
void animate_dealFlopCards()
{
	setColor(COL_BLACK, COL_CYAN);
	for (int i = 0; i < 3; ++i) {
		//draw_card(TABLE_X + 2 + i * 3, TABLE_Y + 2, cc[i]);
		setCursorPos(TABLE_X + 2 + i * 3, TABLE_Y + 2);
		cout << "  ";
		Sleep(300);
	}
}
void animate_dealTurnCards()
{
	setColor(COL_BLACK, COL_CYAN);
	setCursorPos(TABLE_X + 2 + 3 * 3, TABLE_Y + 2);
	cout << "  ";
	Sleep(300);
}
void animate_dealRiverCards()
{
	setColor(COL_BLACK, COL_CYAN);
	setCursorPos(TABLE_X + 2 + 4 * 3, TABLE_Y + 2);
	cout << "  ";
	Sleep(300);
}
int game()
{
	g_table.setDealer();
	//while ( g_table.chip(g_manIX) != 0 && g_table.chip(g_comIX) != 0 )
	for(;;) {
		for (int i = 0; i < g_table.nPlayer(); ++i) {
			if( !g_table.chip(i) )
				return i;
		}
		g_table.dealHoleCards();
		animate_dealHoleCards();
		if( round() ) {
			//	�܂��Q�l�ȏ�c���Ă���ꍇ
			g_table.dealFlop();
			animate_dealFlopCards();
			if( round() ) {
				//	�܂��Q�l�ȏ�c���Ă���ꍇ
				g_table.dealTurn();
				animate_dealTurnCards();
				if( round() ) {
					//	�܂��Q�l�ȏ�c���Ă���ꍇ
					g_table.dealRiver();
					animate_dealRiverCards();
					round();
				}
			}
		}
		//draw_com(/*open:*/true);
		//	���Z����
		setCursorPos(MENU_X, MENU_Y);
		setColor(COL_GRAY, COL_BLACK);
		bool folded = true;
#if	1
		int winIX;
		if( g_table.nNotFoldPlayer(winIX) == 1 ) {
			//	done: ��l�ȊO�S�����~�肽�ꍇ
		} else {
			vector<uint> odr(g_table.nPlayer());
			vector<uint> hand(g_table.nPlayer());
			std::vector<Card> v;
			uint mx = 0;
			for (int i = 0; i < g_table.nPlayer(); ++i) {
				draw_player(i, playerPos[i].m_x, playerPos[i].m_y, !g_table.folded(i));
				if( !g_table.folded(i) ) {
					g_table.playersCard(i, v);
					hand[i] = checkHand(v, odr[i]);
					show_act(playerPos[i].m_x, playerPos[i].m_y, shortHandName[hand[i]]);
					if( odr[i] > mx ) {
						mx = odr[i];
						winIX = i;
					}
				}
			}
		}
		draw_table();
		g_table.winner(winIX);
		setCursorPos(MENU_X, MENU_Y);
		setColor(COL_GRAY, COL_BLACK);
		if( winIX == g_manIX ) {
			cout << "+++ You win +++ (Push Enter Key)";
		} else {
			cout << "--- " << g_table.player(winIX).m_name << " win --- (Push Enter Key)";
		}
#else
		if( g_table.folded(g_comIX) ) {
			cout << "+++ You Win +++ (Push Enter Key)";
			g_table.winner(g_manIX);
		} else if( g_table.folded(g_manIX) ) {
			cout << "--- Com Win --- (Push Enter Key)";
			g_table.winner(g_comIX);
		} else {
			folded = false;
			draw_player(g_comIX, COM_X, COM_Y, /*open:*/true);
			//draw_com(/*open:*/true);
			uint odr[2];		//	undone: �R�l�ȏ�Ή�
			uint hand[2];
			std::vector<Card> v;
			for (int i = 0; i < g_table.nPlayer(); ++i) {
				g_table.playersCard(i, v);
				hand[i] = checkHand(v, odr[i]);
			}
#if	1
			show_act(COM_X, COM_Y, shortHandName[hand[g_comIX]]);
			show_act(MAN_X, MAN_Y, shortHandName[hand[g_manIX]]);
#else
			setColor(COL_GRAY, COL_BLACK);
			setCursorPos(COM_X, COM_Y + 3);
			cout << handName[hand[g_comIX]] << "        ";
			setCursorPos(MAN_X, MAN_Y + 3);
			cout << handName[hand[g_manIX]] << "        ";
#endif
			print_result(odr);
		}
#endif
		//draw_com(/*open:*/!folded);		//	�ǂ��炩���~�肽�ꍇ�͎�D�����炳�Ȃ�
		int ch = getChar();
		if( ch == 'p' )
			draw_winSplit_vsRand();
		clear_menu();
#if	1
		for (int i = 0; i < N_PLAYER; ++i) {
			draw_player(i, playerPos[i].m_x, playerPos[i].m_y, !g_table.folded(i));
		}
#else
		draw_player(g_comIX, COM_X, COM_Y, /*open:*/!folded);
		//draw_com(/*open:*/!folded);		//	�ǂ��炩���~�肽�ꍇ�͎�D�����炳�Ȃ�
		draw_human();
#endif
		draw_table();
		show_message("Push Enter Key");
		getChar();
		//clear_winSplit();
		//	�𖼕\������������
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
#if	0
	std::vector<Card> cm;
	cm.push_back(Card(Card::DIAMONDS, Card::RANK_K));
	cm.push_back(Card(Card::SPADES, Card::RANK_4));
	cm.push_back(Card(Card::CLUBS, Card::RANK_A));
	cm.push_back(Card(Card::DIAMONDS, Card::RANK_10));
	cm.push_back(Card(Card::HERTS, Card::RANK_Q));
	double ws = calcWinSplitProb(Card(Card::HERTS, Card::RANK_K),
												Card(Card::HERTS, Card::RANK_10),
												cm, 5);
	cout << ws;
#endif
	setColor(COL_BLACK, COL_WHITE);
	setCursorPos(5, 0);
	cout << "*** Texas Hold'em Poker ***";
	draw_hand_help();
	g_table.addPlayer(Player("YOU", 200));
	g_manIX = 0;
	g_table.addPlayer(Player("COM1", 200, /*comp:*/true));
	g_table.addPlayer(Player("COM2", 200, /*comp:*/true));
	g_table.addPlayer(Player("COM3", 200, /*comp:*/true));
	g_table.addPlayer(Player("COM4", 200, /*comp:*/true));
	g_comIX = 1;
	for (int g = 0;;++g) {
		int i = game();
		if( g_table.chip(g_manIX) == 0 ) {
			show_message("You Busted.");
		} else {
			show_message("Computer are Busted.");
		}
		for (int i = 0; i < g_table.nPlayer(); ++i) {
			if( !g_table.chip(i) )
				g_table.setChip(i, 200);		//	���̃Q�[���p
		}
		cout << " continue ? [y/n]";
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
