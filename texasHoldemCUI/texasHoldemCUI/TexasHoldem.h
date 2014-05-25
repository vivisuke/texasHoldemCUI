#pragma once

#include <vector>
#include "Deck.h"

//----------------------------------------------------------------------
struct Player
{
public:
	Player(const std::string &name, int chip, short computer = 0)
		: m_name(name)
		, m_chip(chip)
		, m_AI(computer)
	{}
	Player(const Player &x)
		: m_name(x.m_name)
		, m_chip(x.m_chip)
		, m_AI(x.m_AI)
	{}

public:
	short		m_AI;		//	AI type, 0 は人間
	int		m_chip;
	std::string	m_name;
};
//----------------------------------------------------------------------
typedef std::pair<Card, Card> HoleCards;
class TexasHoldem
{
public:
	enum {
		PRE_FLOP = 0,
		FLOP,
		TURN,
		RIVER,
		
		UNKNOWN = 0,
		FOLD,
		CHECKED,
		CALLED,
		RAISED,
	};
public:
	TexasHoldem();
	~TexasHoldem() {};

public:
	void	getHoleCards(int, Card &, Card &) const;
	void	print(const char * = 0) const;
	void	printPlayerChips() const;		//	プレイヤーのチップのみ表示
	void	printProb() const;
	int	nPlayer() const { return m_nPlayer; }
	int	turn() const { return m_turn; }		//	ターン、0:プリフロップ、1:フロップ、2:ターン, 3:リバー
	int	dealerIX() const { return m_dealerIX; }
	int	SBIX() const;
	int	BBIX() const;
	int	call() const { return m_call; }		//	コールするのに必要なトータルチップ額
	int	chip(int ix) const { return m_player[ix].m_chip; }
	int	bet(int ix) const;
	int	pot() const { return m_pot; }
	bool	allCalled() const;

public:
	void	initialize();					//	テーブルの初期化
	void	initForGame();			//	１ゲーム（ホールカード配布、フロップ、ターン、リバーまで）のための初期化
	int	addPlayer(const Player &);		//	プレイヤーを追加、プレイヤーIDを返す
	void	setDealer();				//	ディーラをランダムに決める
	void	dealHoleCards();			//	ホールカードを配る
	void	dealFlop();					//	コミュニティカード3枚を配る
	void	dealTurn();					//	コミュニティカード4枚目を配る
	void	dealRiver();				//	コミュニティカード5枚目を配る
	void	showDown();				//	手札を晒し、勝敗を決め、チップを精算
	void	game();						//	1ゲームの処理
	void	forwardDealer();			//	デーラーをひとつ進める
	int	action(int);					//	AI
	void	setBet(int ix, int b);		//	ベット/コールを行う
	
private:
	Deck	m_deck;			//	デッキ（一組のカード）
	int	m_nPlayer;		//	テーブルの人数
	int	m_initBB;			//	BB 初期値
	int	m_BB;				//	big blind (sould be even)
	int	m_dealerIX;		//	ディーラインデックス
	int	m_pot;				//	ポット（m_bets の全合計）
	int	m_call;				//	コールするためのトータルチップ額
	int	m_turn;				//	ターン、0:プリフロップ、1:フロップ、2:ターン, 3:リバー
	bool	m_showDowned;
	std::vector<Player>	m_player;		//	各プレイヤー
	std::vector<bool>		m_folded;		//	FOLD済み
	std::vector<HoleCards>	m_holeCards;		//	各プレイヤーのホールカード
	std::vector<Card>		m_communityCards;	//	コミュニティカード
	std::vector<std::vector<int>>	m_bets;			//	各プレイヤーのターンごとのベット額
};
