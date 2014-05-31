#include <iostream>
#include <vector>
#include "Deck.h"
#include "poker.h"

int main()
{
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
	getchar();
	return 0;
}