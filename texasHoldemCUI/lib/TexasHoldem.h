#pragma once

#include <vector>
#include <random>
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
	short		m_AI;		//	AI type, 0 �͐l��
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
	Player	player(int ix) const { return m_player[ix]; }
	void	getHoleCards(int, Card &, Card &) const;
	void	print(const char * = 0) const;
	void	printPlayerChips() const;		//	�v���C���[�̃`�b�v�̂ݕ\��
	void	printProb() const;
	int	nPlayer() const { return m_nPlayer; }
	int	nNotFoldPlayer() const;
	int	nNotFoldPlayer(int &) const;
	int	round() const { return m_round; }		//	���E���h�A0:�v���t���b�v�A1:�t���b�v�A2:�^�[��, 3:���o�[
	int	BB() const { return m_BB; }
	int	dealerIX() const { return m_dealerIX; }
	int	SBIX() const;
	int	BBIX() const;
	int	call() const { return m_call; }		//	�R�[������̂ɕK�v�ȃg�[�^���`�b�v�z
	int	chip(int ix) const { return m_player[ix].m_chip; }
	int	bet(int ix) const;
	int	bet(int ix, int round) const;
	int	pot() const { return m_pot; }
	int	foldedRound(int ix) const { return m_foldedRound[ix]; }
	bool	isFolded(int ix) const { return m_folded[ix]; }
	bool	allCalled() const;
	bool	isAllIn() const;		//	��l�ȊO�S�����I�[���C�����H
	const std::vector<Card>	&communityCards() const { return m_communityCards; }
	void	playersCard(int ix, std::vector<Card> &);
	bool	folded(int ix) const { return m_folded[ix]; }

public:
	void	initialize();					//	�e�[�u���̏�����
	void	initForGame();			//	�P�Q�[���i�z�[���J�[�h�z�z�A�t���b�v�A�^�[���A���o�[�܂Łj�̂��߂̏�����
	int	addPlayer(const Player &);		//	�v���C���[��ǉ��A�v���C���[ID��Ԃ�
	void	setChip(int ix, int cp);
	void	setDealer();				//	�f�B�[���������_���Ɍ��߂�
	void	dealHoleCards();			//	�z�[���J�[�h��z��
	void	dealFlop();					//	�R�~���j�e�B�J�[�h3����z��
	void	dealTurn();					//	�R�~���j�e�B�J�[�h4���ڂ�z��
	void	dealRiver();				//	�R�~���j�e�B�J�[�h5���ڂ�z��
	void	showDown();				//	��D���N���A���s�����߁A�`�b�v�𐸎Z
	void	game();						//	1�Q�[���̏���
	void	forwardDealer();			//	�f�[���[���ЂƂi�߂�
	void	fold(int ix);					//	�t�H�[���h
	int	action(int);					//	AI
	void	addBet(int ix, int b);		//	�x�b�g/�R�[�����s��
	void	addBet(int ix, int round, int b);		//	�x�b�g/�R�[�����s��
	void	winner(int ix);			//	ix ���������ꍇ�̏���
	void	winner(const std::vector<int> &sv);			//	sv ���������ꍇ�̏���
	void	split(const std::vector<int> &v);		//	���������������ꍇ�̏���

protected:
	void	backBets();			//	��������x�b�g�����̃v���C���[�ɕԂ�

private:
	Deck	m_deck;			//	�f�b�L�i��g�̃J�[�h�j
	int	m_nPlayer;		//	�e�[�u���̐l��
	int	m_initBB;			//	BB �����l
	int	m_BB;				//	big blind (sould be even)
	int	m_dealerIX;		//	�f�B�[���C���f�b�N�X
	int	m_pot;				//	�|�b�g�im_bets �̑S���v�j
	int	m_call;				//	�R�[�����邽�߂̃g�[�^���`�b�v�z
	int	m_round;			//	���E���h�A0:�v���t���b�v�A1:�t���b�v�A2:�^�[��, 3:���o�[
	bool	m_showDowned;
	std::vector<Player>	m_player;		//	�e�v���C���[
	std::vector<bool>		m_folded;		//	FOLD�ς�
	std::vector<int>			m_foldedRound;		//	FOLD�������E���h�i0:�v���t���b�v�A1:�t���b�v�A2:�^�[��, 3:���o�[�j
	std::vector<HoleCards>	m_holeCards;		//	�e�v���C���[�̃z�[���J�[�h
	std::vector<Card>		m_communityCards;	//	�R�~���j�e�B�J�[�h
	std::vector<std::vector<int>>	m_bets;			//	�e�v���C���[�̃��E���h���Ƃ̃x�b�g�z
	//std::random_device	m_rnd;     // �񌈒�I�ȗ���������
	std::mt19937	m_mt ;
};