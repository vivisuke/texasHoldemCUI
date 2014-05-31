#include <iostream>
#include <vector>
#include <Windows.h>
#include "../lib/Deck.h"
#include "../lib/poker.h"
#include <assert.h>

//----------------------------------------------------------------------
#pragma comment(lib, "winmm.lib")		//	for timeGetTime()
//	timeGetTime() を使った時間計測タイマー クラス
class Timer
{
public:
	Timer() { restart(); }
public:
	void	restart()
	{
		m_start = timeGetTime();		//	計測開始時間を保存
	}
	double	elapsed()		//	リスタートからの秒数を返す
	{
		DWORD end = timeGetTime();
		return (double)(end - m_start) / 1000;
	}
private:
	DWORD		m_start;		//	計測開始時間
};
//----------------------------------------------------------------------
int main()
{
	if( 1 ) {
		std::vector<Card> v;
		v.push_back(Card(Card::SPADES, Card::RANK_A));
		v.push_back(Card(Card::CLUBS, Card::RANK_A));
		v.push_back(Card(Card::HERTS, Card::RANK_A));
		v.push_back(Card(Card::SPADES, Card::RANK_Q));
		v.push_back(Card(Card::CLUBS, Card::RANK_Q));
		v.push_back(Card(Card::HERTS, Card::RANK_Q));
		v.push_back(Card(Card::HERTS, Card::RANK_K));
		assert( checkHand(v) == FULL_HOUSE );
		assert( checkHandBM(v) == FULL_HOUSE );
		uint odr1, odr2;
		assert( checkHand(v, odr1) == FULL_HOUSE );
		assert( checkHandBM(v, odr2) == FULL_HOUSE );
		assert( odr1 == odr2 );
	}
	if( 1 ) {
		int N = 10000;
		std::cout << "checkHandBM():\n";
		Timer tm;
		for (int i = 0; i < N; ++i) {
			std::vector<Card> v;
			Deck deck;
			deck.shuffle();
			for (int k = 0; k < 7; ++k) {
				v.push_back(deck.deal());
			}
			uint odr, odrBM;
			int h = checkHand(v, odr);
			checkHandBM(v, odrBM);
			if( odr != odrBM ) {
				std::cout << handName[h] << "\n";
				print(v, odr);
				print(v, odrBM);
				checkHandBM(v, odrBM);
			}
		}
	}
	{
		mlbnM(0x1fff);
		mlbnM(0x100);
		mlbnM(0x10);
		mlbnM(0x1);
	}
	if( 1 ) {
		//	ランダム５枚の場合
		std::vector<int> cnt(N_KIND_HAND, 0);		//	各役の出現回数
		int N = 10000;
		for (int i = 0; i < N; ++i) {
			std::vector<Card> v;
			Deck deck;
			deck.shuffle();
			for (int k = 0; k < 5; ++k) {
				v.push_back(deck.deal());
			}
			int h = checkHandBM(v);
			int h2 = checkHand(v);
			if( h != h2 ) {
				print(v);
				h = checkHandBM(v);
			}
			cnt[h] += 1;
		}
		for (int i = 0; i < N_KIND_HAND; ++i) {
			std::cout.width(8);
			std::cout << (double)cnt[i]*100/N << "% " << handName[i] << "\n";
		}
	}
	if( 1 ) {
		//	ランダム７枚の場合
		std::vector<int> cnt(N_KIND_HAND, 0);		//	各役の出現回数
		int N = 10000;
		for (int i = 0; i < N; ++i) {
			std::vector<Card> v;
			Deck deck;
			deck.shuffle();
			for (int k = 0; k < 7; ++k) {
				v.push_back(deck.deal());
			}
			int h = checkHandBM(v);
			int h2 = checkHand(v);
			if( h != h2 ) {
				print(v);
				std::cout << "BM:" << handName[h] << ", " << handName[h2] << "\n";
				h = checkHandBM(v);
				uint odr;
				int h3 = checkHand(v, odr);
			}
			cnt[h] += 1;
		}
		for (int i = 0; i < N_KIND_HAND; ++i) {
			std::cout.width(8);
			std::cout << (double)cnt[i]*100/N << "% " << handName[i] << "\n";
		}
	}
	if( 1 ) {
		int N = 10000;
		std::cout << "checkHandBM():\n";
		Timer tm;
		for (int i = 0; i < N; ++i) {
			std::vector<Card> v;
			Deck deck;
			deck.shuffle();
			for (int k = 0; k < 7; ++k) {
				v.push_back(deck.deal());
			}
			int h = checkHandBM(v);
		}
		std::cout << tm.elapsed() << "\n";
		std::cout << "checkHand():\n";
		for (int i = 0; i < N; ++i) {
			std::vector<Card> v;
			Deck deck;
			deck.shuffle();
			for (int k = 0; k < 7; ++k) {
				v.push_back(deck.deal());
			}
			int h = checkHand(v);
		}
		std::cout << tm.elapsed() << "\n";
	}
	getchar();
	return 0;
}