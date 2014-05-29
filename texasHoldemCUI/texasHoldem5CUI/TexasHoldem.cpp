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
	std::random_device rnd;     // �񌈒�I�ȗ���������
	//##m_mt.seed(rnd());		//	random_device �̗����ŃV�[�h��������
	initialize();
}
//	�e�[�u���̏�����
void TexasHoldem::initialize()
{
	m_nPlayer = 0;
	m_initBB = 2;
	m_dealerIX = 0;
	initForGame();
}
//	�P�Q�[���i�z�[���J�[�h�z�z�A�t���b�v�A�^�[���A���o�[�܂Łj�̂��߂̏�����
void TexasHoldem::initForGame()
{
	m_BB = m_initBB;
	m_pot = 0;
	m_round = 0;		//	0:�z�[���J�[�h�z�z�A1:�t���b�v�A2:�^�[���A3:���o�[
	m_showDowned = false;
	m_communityCards.clear();
	m_bets.clear();
	//m_deck.shuffle();
	m_holeCards.clear();		//	�z�[���J�[�h�z����N���A
	//m_bets.clear();
	m_bets.resize(m_nPlayer);
	m_folded.clear();
	m_folded.resize(m_nPlayer, false);
	m_deck.init(/*shuffle*/true);
}
//	�f�B�[���������_���Ɍ��߂�
void TexasHoldem::setDealer()
{
	std::uniform_int_distribution<> randCard(0, m_nPlayer - 1);
	m_dealerIX = randCard(m_mt);
	//m_dealerIX = rand() % m_nPlayer;
}
void TexasHoldem::dealHoleCards()			//	�z�[���J�[�h��z��
{
	initForGame();
	for (int i = 0; i < m_nPlayer; ++i) {
		HoleCards hc(m_deck.deal(), m_deck.deal());		//	�J�[�h���Q���z��
		m_holeCards.push_back(hc);					//	������z�[���J�[�h�z��ɒǉ�
		m_bets[i].resize(4, /*value:*/0);		//	m_bets �̓��E���h���̃x�b�g�z
	}
	int ix = m_dealerIX + 1;
	if( ix >= m_nPlayer ) ix = 0;
	m_player[ix].m_chip -= m_BB / 2;				//	Small Blind
	m_bets[ix][0] = m_BB / 2;
	if( ++ix >= m_nPlayer ) ix = 0;
	m_player[ix].m_chip -= m_BB;		//	Big Blind
	m_bets[ix][0] = m_BB;
	m_pot = m_BB + m_BB / 2;
	m_call = m_BB;		//	���݂̃g�[�^���R�[���z
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
void TexasHoldem::dealFlop()					//	�R�~���j�e�B�J�[�h3����z��
{
	m_round = FLOP;
	m_communityCards.push_back(m_deck.deal());
	m_communityCards.push_back(m_deck.deal());
	m_communityCards.push_back(m_deck.deal());
}
void TexasHoldem::dealTurn()					//	�R�~���j�e�B�J�[�h4���ڂ�z��
{
	m_round = TURN;
	m_communityCards.push_back(m_deck.deal());
}
void TexasHoldem::dealRiver()				//	�R�~���j�e�B�J�[�h5���ڂ�z��
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
//	�t�H�[���h
void TexasHoldem::fold(int ix)
{
	m_folded[ix] = true;
}
//	�t�H�[���h���Ă��Ȃ��v���C���[�l����Ԃ�
int TexasHoldem::nNotFoldPlayer() const
{
	int cnt = 0;
	for (int i = 0; i < (int)m_nPlayer; ++i) {
		if( !m_folded[i] ) ++cnt;
	}
	return cnt;
}
//	�t�H�[���h���Ă��Ȃ��v���C���[�l����Ԃ�
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
//	��������x�b�g�����̃v���C���[�ɕԂ�
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
void TexasHoldem::winner(int ix)			//	ix ���������ꍇ�̏���
{
	backBets();
	m_player[ix].m_chip += m_pot;
	m_pot = 0;
	//m_bets.clear();
}
void TexasHoldem::split(const std::vector<int> &v)		//	���������������ꍇ�̏���
{
	backBets();
	//	undone: �]�肪����ꍇ�̏��� for �R�l�ȏ�Ή�
	for (int i = 0; i < (int)v.size(); ++i) {
		m_player[i].m_chip += m_pot/v.size();
	}
	m_pot = 0;
	//m_bets.clear();
}
void TexasHoldem::showDown()				//	��D���N���A���s�����߁A�`�b�v�𐸎Z
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
void TexasHoldem::game()						//	1�Q�[���̏���
{
	std::cout << "\nNew Game:\n";
	int nActPlayer = m_nPlayer;		//	�A�N�e�B�u�v���C���[��
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
		//	�z�[���J�[�h�z�z��̃x�b�g���E���h
		//	�R�[�������� or ��l�ȊO���t�H�[���h���A�A�N�e�B�u���P�ɂȂ�܂Ń��[�v
		//	�R�[���������FlastRaisedIX �ɂ���Ă��āAtoCall ���[���̏ꍇ
		//int lastRaisedIX = m_dealerIX + 3;		//	D SB BB �̎����Ō�Ƀ��C�Y�����Ƃ݂Ȃ�
		//while( lastRaisedIX >= m_nPlayer )
		//	lastRaisedIX -= m_nPlayer;
		for (;;) {
			int bets = std::accumulate(m_bets[ix].begin(), m_bets[ix].end(), 0);
			int toCall = m_call - bets;		//	�R�[���ɕK�v�Ȋz
			if( !toCall && ix == lastRaisedIX )		//	�R�[�����������ꍇ
				break;
			int act = UNKNOWN;
			int raise = 0;		//	���C�Y���z�Araise should > toCall
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
				//	�R���s���[�^�̎��
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
	if( nActPlayer == 1 ) {		//	��l�ȊO�S���~�肽�ꍇ
		int ix = std::find(m_folded.begin(), m_folded.end(), false) - m_folded.begin();
		m_player[ix].m_chip += m_pot;
		return;
	} else {
		showDown();
	}
}
void TexasHoldem::forwardDealer()			//	�f�[���[���ЂƂi�߂�
{
	if( ++m_dealerIX >= m_nPlayer )
		m_dealerIX = 0;
}
//	�`�b�v���������v���C���[��ǉ��A�v���C���[ID��Ԃ�
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
//	�v���C���[�̃`�b�v�̂ݕ\��
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
					std::cout << "�H �H";
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
void TexasHoldem::addBet(int ix, int b)		//	�x�b�g/�R�[�����s��
{
	m_player[ix].m_chip -= b;
	m_bets[ix].push_back(b);
	m_pot += b;
	m_call = max(m_call, bet(ix));
}
bool TexasHoldem::isAllIn() const		//	��l�ȊO�S�����I�[���C�����H
{
	int n = 0;		//	�`�b�v�������Ă���l�̐�
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
			if( bet(i) != m_call && m_player[i].m_chip != 0 )	//	�`�b�v���܂�����̂ɃR�[�����Ă��Ȃ�
				return false;
		}
	}
	return true;
}
