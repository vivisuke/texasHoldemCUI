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
	void	getHoleCards(int, Card &, Card &) const;
	void	print(const char * = 0) const;
	void	printPlayerChips() const;		//	�v���C���[�̃`�b�v�̂ݕ\��
	void	printProb() const;
	int	nPlayer() const { return m_nPlayer; }
	int	turn() const { return m_turn; }		//	�^�[���A0:�v���t���b�v�A1:�t���b�v�A2:�^�[��, 3:���o�[
	int	dealerIX() const { return m_dealerIX; }
	int	SBIX() const;
	int	BBIX() const;
	int	call() const { return m_call; }		//	�R�[������̂ɕK�v�ȃg�[�^���`�b�v�z
	int	chip(int ix) const { return m_player[ix].m_chip; }
	int	bet(int ix) const;
	int	pot() const { return m_pot; }
	bool	allCalled() const;

public:
	void	initialize();					//	�e�[�u���̏�����
	void	initForGame();			//	�P�Q�[���i�z�[���J�[�h�z�z�A�t���b�v�A�^�[���A���o�[�܂Łj�̂��߂̏�����
	int	addPlayer(const Player &);		//	�v���C���[��ǉ��A�v���C���[ID��Ԃ�
	void	setDealer();				//	�f�B�[���������_���Ɍ��߂�
	void	dealHoleCards();			//	�z�[���J�[�h��z��
	void	dealFlop();					//	�R�~���j�e�B�J�[�h3����z��
	void	dealTurn();					//	�R�~���j�e�B�J�[�h4���ڂ�z��
	void	dealRiver();				//	�R�~���j�e�B�J�[�h5���ڂ�z��
	void	showDown();				//	��D���N���A���s�����߁A�`�b�v�𐸎Z
	void	game();						//	1�Q�[���̏���
	void	forwardDealer();			//	�f�[���[���ЂƂi�߂�
	int	action(int);					//	AI
	void	setBet(int ix, int b);		//	�x�b�g/�R�[�����s��
	
private:
	Deck	m_deck;			//	�f�b�L�i��g�̃J�[�h�j
	int	m_nPlayer;		//	�e�[�u���̐l��
	int	m_initBB;			//	BB �����l
	int	m_BB;				//	big blind (sould be even)
	int	m_dealerIX;		//	�f�B�[���C���f�b�N�X
	int	m_pot;				//	�|�b�g�im_bets �̑S���v�j
	int	m_call;				//	�R�[�����邽�߂̃g�[�^���`�b�v�z
	int	m_turn;				//	�^�[���A0:�v���t���b�v�A1:�t���b�v�A2:�^�[��, 3:���o�[
	bool	m_showDowned;
	std::vector<Player>	m_player;		//	�e�v���C���[
	std::vector<bool>		m_folded;		//	FOLD�ς�
	std::vector<HoleCards>	m_holeCards;		//	�e�v���C���[�̃z�[���J�[�h
	std::vector<Card>		m_communityCards;	//	�R�~���j�e�B�J�[�h
	std::vector<std::vector<int>>	m_bets;			//	�e�v���C���[�̃^�[�����Ƃ̃x�b�g�z
};
