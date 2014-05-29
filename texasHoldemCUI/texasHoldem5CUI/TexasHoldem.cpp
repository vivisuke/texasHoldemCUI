#include <iostream>
#include <numeric>		//	for accumulate
#include <algorithm>
#include <assert.h>
#include "TexasHoldem.h"
#include "poker.h"
#include "consoleUtil.h"

typedef unsigned int uint;

TexasHoldem::TexasHoldem()
{
	std::random_device rnd;     // 非決定的な乱数生成器
	//##m_mt.seed(rnd());		//	random_device の乱数でシードを初期化
	initialize();
}
//	テーブルの初期化
void TexasHoldem::initialize()
{
	m_nPlayer = 0;
	m_initBB = 2;
	m_dealerIX = 0;
	initForGame();
}
//	１ゲーム（ホールカード配布、フロップ、ターン、リバーまで）のための初期化
void TexasHoldem::initForGame()
{
	m_BB = m_initBB;
	m_pot = 0;
	m_round = 0;		//	0:ホールカード配布、1:フロップ、2:ターン、3:リバー
	m_showDowned = false;
	m_communityCards.clear();
	m_bets.clear();
	//m_deck.shuffle();
	m_holeCards.clear();		//	ホールカード配列をクリア
	//m_bets.clear();
	m_bets.resize(m_nPlayer);
	m_folded.clear();
	m_folded.resize(m_nPlayer, false);
	m_deck.init(/*shuffle*/true);
}
//	ディーラをランダムに決める
void TexasHoldem::setDealer()
{
	std::uniform_int_distribution<> randCard(0, m_nPlayer - 1);
	m_dealerIX = randCard(m_mt);
	//m_dealerIX = rand() % m_nPlayer;
}
void TexasHoldem::dealHoleCards()			//	ホールカードを配る
{
	initForGame();
	for (int i = 0; i < m_nPlayer; ++i) {
		HoleCards hc(m_deck.deal(), m_deck.deal());		//	カードを２枚配る
		m_holeCards.push_back(hc);					//	それをホールカード配列に追加
		m_bets[i].resize(4, /*value:*/0);		//	m_bets はラウンド毎のベット額
	}
	int ix = m_dealerIX + 1;
	if( ix >= m_nPlayer ) ix = 0;
	m_player[ix].m_chip -= m_BB / 2;				//	Small Blind
	m_bets[ix][0] = m_BB / 2;
	if( ++ix >= m_nPlayer ) ix = 0;
	m_player[ix].m_chip -= m_BB;		//	Big Blind
	m_bets[ix][0] = m_BB;
	m_pot = m_BB + m_BB / 2;
	m_call = m_BB;		//	現在のトータルコール額
}
void TexasHoldem::getHoleCards(int ix, Card &c1, Card &c2) const
{
	if( ix >= m_nPlayer ) {
		c1 = c2 = -1;
	} else {
		c1 = m_holeCards[ix].first;
		c2 = m_holeCards[ix].second;
	}
}
void TexasHoldem::dealFlop()					//	コミュニティカード3枚を配る
{
	m_round = FLOP;
	m_communityCards.push_back(m_deck.deal());
	m_communityCards.push_back(m_deck.deal());
	m_communityCards.push_back(m_deck.deal());
}
void TexasHoldem::dealTurn()					//	コミュニティカード4枚目を配る
{
	m_round = TURN;
	m_communityCards.push_back(m_deck.deal());
}
void TexasHoldem::dealRiver()				//	コミュニティカード5枚目を配る
{
	m_round = RIVER;
	m_communityCards.push_back(m_deck.deal());
}
void TexasHoldem::playersCard(int ix, std::vector<Card> &v)
{
	v.clear();
	v.push_back(m_holeCards[ix].first);
	v.push_back(m_holeCards[ix].second);
	for (int i = 0; i < (int)m_communityCards.size(); ++i) {
		v.push_back(m_communityCards[i]);
	}
}
//	フォールド
void TexasHoldem::fold(int ix)
{
	m_folded[ix] = true;
}
//	フォールドしていないプレイヤー人数を返す
int TexasHoldem::nNotFoldPlayer() const
{
	int cnt = 0;
	for (int i = 0; i < (int)m_nPlayer; ++i) {
		if( !m_folded[i] ) ++cnt;
	}
	return cnt;
}
//	フォールドしていないプレイヤー人数を返す
int TexasHoldem::nNotFoldPlayer(int &ix) const
{
	int cnt = 0;
	for (int i = 0; i < (int)m_nPlayer; ++i) {
		if( !m_folded[i] ) {
			ix = i;
			++cnt;
		}
	}
	return cnt;
}
//	多すぎるベットを元のプレイヤーに返す
void TexasHoldem::backBets()
{
	int cmn = INT_MAX;
	for (int i = 0; i < m_nPlayer; ++i) {
		if( !m_folded[i] && bet(i) < cmn )
			cmn = bet(i);
	}
	//int cmn = min(bet(0), bet(1));
	for (int i = 0; i < m_nPlayer; ++i) {
		if( !m_folded[i] ) {
			int d = bet(i) - cmn;
			m_player[i].m_chip += d;
			m_pot -= d;
		}
	}
}
void TexasHoldem::winner(int ix)			//	ix が買った場合の処理
{
	backBets();
	m_player[ix].m_chip += m_pot;
	m_pot = 0;
	//m_bets.clear();
}
void TexasHoldem::split(const std::vector<int> &v)		//	引き分けだった場合の処理
{
	backBets();
	//	undone: 余りがある場合の処理 for ３人以上対応
	for (int i = 0; i < (int)v.size(); ++i) {
		m_player[i].m_chip += m_pot/v.size();
	}
	m_pot = 0;
	//m_bets.clear();
}
void TexasHoldem::showDown()				//	手札を晒し、勝敗を決め、チップを精算
{
	std::cout << "\n";
	m_showDowned = true;
#if	1
	assert( m_nPlayer == 2 );
	uint odr1, odr2;
	std::vector<Card> v = m_communityCards;
	v.push_back(m_holeCards[0].first);
	v.push_back(m_holeCards[0].second);
	int h = checkHand(v, odr1);
	std::cout << m_player[0].m_name << ": " << handName[h] << "\n";
	v = m_communityCards;
	v.push_back(m_holeCards[1].first);
	v.push_back(m_holeCards[1].second);
	h = checkHand(v, odr2);
	std::cout << m_player[1].m_name << ": " << handName[h] << "\n";
	if( odr1 > odr2 ) {
		std::cout << m_player[0].m_name << " won.\n";
		m_player[0].m_chip += m_pot;
	} else if( odr1 < odr2 ) {
		std::cout << m_player[1].m_name << " won.\n";
		m_player[1].m_chip += m_pot;
	} else {
		std::cout << "split.\n";
		m_player[0].m_chip += m_pot / 2;
		m_player[1].m_chip += m_pot / 2;
	}
#else
	int winIX = -1;
	uint maxOdr = 0, odr;
	std::vector<Card> v;
	for (int i = 0; i < m_nPlayer; ++i) {
		v = m_communityCards;
		v.push_back(m_holeCards[i].first);
		v.push_back(m_holeCards[i].second);
		checkHand(v, odr);
		if( 
	}
#endif
	print();
}
//	AI
int TexasHoldem::action(int ix)
{
	double p = calcWinSplitProb(m_holeCards[ix].first, m_holeCards[ix].second, m_communityCards);
	int call = m_call - std::accumulate(m_bets[ix].begin(), m_bets[ix].end(), 0);
	double threshold = (double)call / (m_pot + call);
	std::cout << "p = " << p << ", threshold = " << threshold << "\n";
	if( p >= threshold ) {
		return CALLED;
	} else if( call == 0 )
		return CALLED;
	else
		return FOLD;
}
void TexasHoldem::game()						//	1ゲームの処理
{
	std::cout << "\nNew Game:\n";
	int nActPlayer = m_nPlayer;		//	アクティブプレイヤー数
	for (int t = 0; t < 4; ++t) {
		const char *mess;
		switch( t ) {
			case 0: dealHoleCards();	mess = "Pre-Flop:";	break;
			case 1: dealFlop();	mess = "Flop:";	break;
			case 2: dealTurn();	mess = "Turn:";	break;
			case 3: dealRiver();	mess = "River:";	break;
		}
		print(mess);
		int ix = m_dealerIX + 3;
		while( ix >= m_nPlayer ) ix -= m_nPlayer;
		int lastRaisedIX = -1;
		//	ホールカード配布後のベットラウンド
		//	コールが揃う or 一人以外がフォールドし、アクティブが１になるまでループ
		//	コールが揃う：lastRaisedIX にやってきて、toCall がゼロの場合
		//int lastRaisedIX = m_dealerIX + 3;		//	D SB BB の次が最後にレイズしたとみなす
		//while( lastRaisedIX >= m_nPlayer )
		//	lastRaisedIX -= m_nPlayer;
		for (;;) {
			int bets = std::accumulate(m_bets[ix].begin(), m_bets[ix].end(), 0);
			int toCall = m_call - bets;		//	コールに必要な額
			if( !toCall && ix == lastRaisedIX )		//	コールが揃った場合
				break;
			int act = UNKNOWN;
			int raise = 0;		//	レイズ金額、raise should > toCall
			const bool human = !m_player[ix].m_AI;
			if( human ) {
				std::string buffer;
				for(;;) {
					std::cout << "What wold do ? [F: fold, C: call/check, Digits: raise] \n";
					std::cin >> buffer;
					if( buffer[0] == 'f' || buffer[0] == 'F' ) {
						act = FOLD;
						break;
					}
					if( buffer[0] == 'c' || buffer[0] == 'C' ) {
						if( toCall != 0 ) {
							act = CALLED;
						} else
							act = CHECKED;
						break;
					}
					if( isdigit(buffer[0]) ) {
						raise = atoi(buffer.c_str());
						if( raise < toCall ) {
							std::cout << "raise chips should glater than " << toCall << ".\n";
							continue;
						}
						if( raise > m_player[ix].m_chip ) {
							std::cout << "you don't have " << raise << " chips.\n";
							continue;
						}
						act = RAISED;
						break;
					}
				}
			} else {
				//	コンピュータの手番
				act = action(ix);
#if	0
				if( toCall != 0 ) {
					std::cout << "COM called.\n";
					act = CALLED;
				} else {
					std::cout << "COM checked.\n";
					act = CHECKED;
				}
#endif
			}
			switch( act ) {
				case FOLD:
					if( !human )
						std::cout << "COM folded.\n";
					m_folded[ix] = true;
					--nActPlayer;
					break;
				case CALLED:
					if( !human )
						std::cout << "COM called.\n";
					m_bets[ix][t] += toCall;
					m_player[ix].m_chip -= toCall;
					m_pot += toCall;
					if( lastRaisedIX < 0 ) lastRaisedIX = ix;
					break;
				case CHECKED:
					if( !human )
						std::cout << "COM checked.\n";
					if( lastRaisedIX < 0 ) lastRaisedIX = ix;
					break;
				case RAISED:
					if( !human )
						std::cout << "COM raised.\n";
					m_bets[ix][t] += raise;
					m_player[ix].m_chip -= raise;
					m_pot += raise;
					m_call += raise;
					lastRaisedIX = ix;
					break;
			}
			if( nActPlayer == 1 ) break;
			print();
			if( ++ix >= m_nPlayer ) ix = 0;
		}
		if( nActPlayer == 1 ) break;
	}
	if( nActPlayer == 1 ) {		//	一人以外全部降りた場合
		int ix = std::find(m_folded.begin(), m_folded.end(), false) - m_folded.begin();
		m_player[ix].m_chip += m_pot;
		return;
	} else {
		showDown();
	}
}
void TexasHoldem::forwardDealer()			//	デーラーをひとつ進める
{
	if( ++m_dealerIX >= m_nPlayer )
		m_dealerIX = 0;
}
//	チップを持ったプレイヤーを追加、プレイヤーIDを返す
int TexasHoldem::addPlayer(const Player &p)
{
	int ix = m_nPlayer++;
	m_player.push_back(p);
	//m_holeCards.resize(m_nPlayer);
	return ix;
}
void TexasHoldem::setChip(int ix, int cp)
{
	m_player[ix].m_chip = cp;
}
int TexasHoldem::SBIX() const
{
	int ix = m_dealerIX + 1;
	if( ix >= m_nPlayer )
		ix -= m_nPlayer;
	return ix;
}
int TexasHoldem::BBIX() const
{
	int ix = SBIX() + 1;
	if( ix >= m_nPlayer )
		ix -= m_nPlayer;
	return ix;
}
int TexasHoldem::bet(int ix) const
{
	const std::vector<int> &b = m_bets[ix];
	return std::accumulate(b.begin(), b.end(), 0);
}
int TexasHoldem::bet(int ix, int round) const
{
	return m_bets[ix][round];
}
//	プレイヤーのチップのみ表示
void TexasHoldem::printPlayerChips() const
{
	std::cout << "\n";
	for (int i = 0; i < m_nPlayer; ++i) {
		if( i == m_dealerIX )
			std::cout << "D ";
		else
			std::cout << "  ";
		const Player &ply = m_player[i];
		std::cout << ply.m_name << "\t"
						<< ply.m_chip << "\n";
	}
}
void TexasHoldem::print(const char *mess) const
{
	if( !m_nPlayer ) {
		std::cout << "No players at this table.\n";
	} else {
		std::cout << "\n";
		if( mess != 0 ) {
			setColor(COL_YELLOW);
			std::cout << mess << "\t";
			setColor(COL_GRAY);
		}
		if( !m_showDowned && m_pot != 0 )
			std::cout << "Pot = " << m_pot << "\n";	//" (BB = " << m_BB << ")\n";
		if( !m_communityCards.empty() ) {
			if( !m_showDowned )
				std::cout << "\t\t\t";
			else
				std::cout << "\t\t";
			for (int i = 0; i < (int)m_communityCards.size(); ++i) {
				m_communityCards[i].printW();
			}
			std::cout << "\n";
		}
		for (int i = 0; i < m_nPlayer; ++i) {
			if( i == m_dealerIX )
				std::cout << "D ";
			else
				std::cout << "  ";
			const Player &ply = m_player[i];
			std::cout << ply.m_name << "\t"
							<< ply.m_chip << "\t";
			if( !m_showDowned && i < (int)m_bets.size() )
				std::cout << std::accumulate(m_bets[i].begin(), m_bets[i].end(), 0) << "\t";
			if( i < (int)m_holeCards.size() ) {
				if( ply.m_AI && !m_showDowned ) {
					std::cout << "？ ？";
				} else {
					m_holeCards[i].first.printW();
					std::cout << " ";
					m_holeCards[i].second.printW();
				}
			}
			std::cout << "\n";
		}
	}
}
void TexasHoldem::printProb() const
{
	//std::vector<Card> comu;
	double p = calcWinSplitProb(m_holeCards[0].first, m_holeCards[0].second, m_communityCards);
	std::cout << "winSplit = " << p*100 << "\n";
	int call = m_call - std::accumulate(m_bets[0].begin(), m_bets[0].end(), 0);
	double threshold = (double)call / (m_pot + call);
	///std::cout << "threshold = " << threshold << "\n";
}
void TexasHoldem::addBet(int ix, int b)		//	ベット/コールを行う
{
	m_player[ix].m_chip -= b;
	m_bets[ix].push_back(b);
	m_pot += b;
	m_call = max(m_call, bet(ix));
}
bool TexasHoldem::isAllIn() const		//	一人以外全員がオールインか？
{
	int n = 0;		//	チップを持っている人の数
	for (int i = 0; i < m_nPlayer; ++i) {
		if( !m_folded[i] && m_player[i].m_chip != 0 )
			++n;
	}
	return n <= 1;
}
bool TexasHoldem::allCalled() const
{
	for (int i = 0; i < m_nPlayer; ++i) {
		if( !m_folded[i] ) {
			if( bet(i) != m_call && m_player[i].m_chip != 0 )	//	チップがまだあるのにコールしていない
				return false;
		}
	}
	return true;
}
