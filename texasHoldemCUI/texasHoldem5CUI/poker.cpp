#include "poker.h"
#include "Deck.h"
#include <vector>
#include <assert.h>

//----------------------------------------------------------------------
const char *handName[] = {
	"highCard",
	"onePair",
	"twoPair",
	"threeOfAKind",
	"straight",
	"flush",
	"fullHouse",
	"fourOfAKind",
	"straightFlush",
};
//	v のサイズは 7以下とする
//	要素はソートされていないものとする
int checkHand(const std::vector<Card> &v)
{
	std::vector<int> rcnt(13, 0);
	std::vector<int> scnt(4, 0);
	//int rcnt[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//int scnt[4] = {0, 0, 0, 0};
	for (uint i = 0; i < v.size(); ++i) {
		const Card c = v[i];
		rcnt[c.m_rank] += 1;
		scnt[c.m_suit] += 1;
	}
	int s = -1;
	if( scnt[s = 0] >= 5 || scnt[s = 1] >= 5 || scnt[s = 2] >= 5 || scnt[s = 3] >= 5 ) {
		//	フラッシュ確定
		uint bitmap = 0;
		for (uint i = 0; i < v.size(); ++i) {
			const Card c = v[i];
			if( c.m_suit == s )
				bitmap |= 1 << (c.m_rank);
		}
		uint mask = 0x1f00;		//	AKQJT
		for (int i = 0; i < 9; ++i, mask>>=1) {
			if( (bitmap & mask) == mask ) {
				return STRAIGHT_FLUSH;
			}
		}
		if( bitmap == 0x100f )	//	1 0000 00000 1111 = A5432
			return STRAIGHT_FLUSH;
	} else
		s = -1;
	int threeOfAKindIX = -1;
	int pairIX1 = -1;
	int pairIX2 = -1;
	for (int i = 0; i < 13; ++i) {
		switch( rcnt[i] ) {
			case 4:
				return FOUR_OF_A_KIND;
			case 3:
				threeOfAKindIX = i;
				break;
			case 2:
				pairIX2 = pairIX1;
				pairIX1 = i;
				break;
		}
	}
	if( threeOfAKindIX >= 0 && pairIX1 >= 0 )
		return FULL_HOUSE;
	if( s >= 0 )
		return FLUSH;
	uint bitmap = 0;
	uint mask = 1;
	for (int i = 0; i < 13; ++i, mask<<=1) {
		if( rcnt[i] != 0 )
			bitmap |= mask;
	}
	mask = 0x1f00;		//	AKQJT
	for (int i = 0; i < 9; ++i, mask>>=1) {
		if( (bitmap & mask) == mask )
			return STRAIGHT;
	}
	if( bitmap == 0x100f )	//	1 0000 00000 1111 = A5432
		return STRAIGHT;
	if( threeOfAKindIX >= 0 )
		return THREE_OF_A_KIND;
	if( pairIX2 >= 0 )
		return TWO_PAIR;
	if( pairIX1 >= 0 )
		return ONE_PAIR;
	return HIGH_CARD;
}
//	v のサイズは 7以下とする
//	要素はソートされていないものとする
//	強さを odr に格納するもとする、下位 5*4bit：手の強さ、その上位4ビット：役
int checkHand(const std::vector<Card> v, uint &odr)
{
	odr = 0;
	std::vector<int> rcnt(13, 0);
	std::vector<int> scnt(4, 0);
	//int rcnt[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//int scnt[4] = {0, 0, 0, 0};
	for (uint i = 0; i < v.size(); ++i) {
		const Card c = v[i];
		rcnt[c.m_rank] += 1;
		scnt[c.m_suit] += 1;
	}
	int s = -1;
	uint bitmap = 0;
	if( scnt[s = 0] >= 5 || scnt[s = 1] >= 5 || scnt[s = 2] >= 5 || scnt[s = 3] >= 5 ) {
		//	フラッシュ確定
		for (uint i = 0; i < v.size(); ++i) {
			const Card c = v[i];
			if( c.m_suit == s ) {
				bitmap |= 1 << (c.m_rank);
			}
		}
		uint mask = 0x1f00;		//	AKQJT
		for (int i = 0; i < 9; ++i, mask>>=1) {
			if( (bitmap & mask) == mask ) {
				//	ストレート・フラッシュの場合は、一番大きいカードランクのみで優劣が決まる
				odr |= (12 - i) + (STRAIGHT_FLUSH << 20);
				return STRAIGHT_FLUSH;
			}
		}
		if( bitmap == 0x100f ) {	//	1 0000 00000 1111 = A5432
			odr |= (5-2) + (STRAIGHT_FLUSH << 20);
			return STRAIGHT_FLUSH;
		}
	} else
		s = -1;
	int threeOfAKindIX = -1;
	int pairIX1 = -1;
	int pairIX2 = -1;
	for (int i = 0; i < 13; ++i) {
		switch( rcnt[i] ) {
			case 4:
				//	同じ数の４カードは無いので、４カードのランクのみで優劣が決まる
				odr |= (i << 4) + (FOUR_OF_A_KIND << 20);
				return FOUR_OF_A_KIND;
			case 3:
				threeOfAKindIX = i;
				break;
			case 2:
				pairIX2 = pairIX1;
				pairIX1 = i;
				break;
		}
	}
	if( threeOfAKindIX >= 0 && pairIX1 >= 0 ) {
		//	フルハウスの場合は、３カードのランクが優先、ついでペアのランク
		odr = threeOfAKindIX * 16 + pairIX1 + (FULL_HOUSE << 20);
		return FULL_HOUSE;
	}
	if( s >= 0 ) {
		uint mask = 0x1000 << 1;		//	A
		int r = 12 + 1;		//	A
		for (int i = 0; i < 5; ++i) {		//	大きい順に５枚取り出す
			do {
				--r;
				mask >>= 1;
				if( !mask ) {
					for (uint i = 0; i < v.size(); ++i) {
						std::cout << v[i].toString() << " ";
					}
					std::cout << "\n";
				}
				assert( mask );
			} while( !(bitmap & mask) );
			odr = (odr << 4) + r;
		}
		odr |= (FLUSH << 20);
		return FLUSH;
	}
	bitmap = 0;
	uint mask = 1;
	for (int i = 0; i < 13; ++i, mask<<=1) {
		if( rcnt[i] != 0 )
			bitmap |= mask;
	}
	mask = 0x1f00;		//	AKQJT
	for (int i = 0; i < 9; ++i, mask>>=1) {
		if( (bitmap & mask) == mask ) {
			odr = (12 - i) + (STRAIGHT << 20);
			return STRAIGHT;
		}
	}
	if( bitmap == 0x100f ) {	//	1 0000 00000 1111 = A5432
		odr = (5-2) + (STRAIGHT << 20);
		return STRAIGHT;
	}
	if( threeOfAKindIX >= 0 ) {
		odr = threeOfAKindIX;
		int r = 12 + 1;
		while( --r == threeOfAKindIX || !rcnt[r] ) { }
		odr = (odr << 4) + r;
		while( --r == threeOfAKindIX || !rcnt[r] ) { }
		odr = (odr << 4) + r;
		odr |= (THREE_OF_A_KIND << 20);
		return THREE_OF_A_KIND;
	}
	if( pairIX2 >= 0 ) {
		if( pairIX1 > pairIX2 )
			odr = (pairIX1 << 8) + (pairIX2 << 4);
		else
			odr = (pairIX2 << 8) + (pairIX1 << 4);
		int r = 12;		//	A
		while( !rcnt[r] || r == pairIX1 || r == pairIX2 ) --r;
		odr |= r;
		odr |= (TWO_PAIR << 20);
		return TWO_PAIR;
	}
	if( pairIX1 >= 0 ) {
		odr = pairIX1;
		int r = 12 + 1;		//	A
		while( !rcnt[--r] || r == pairIX1) { }
		odr = (odr << 4) + r;
		while( !rcnt[--r] || r == pairIX1) { }
		odr = (odr << 4) + r;
		while( !rcnt[--r] || r == pairIX1) { }
		odr = (odr << 4) + r;
		odr |= (ONE_PAIR << 20);
		return ONE_PAIR;
	}
	int r = 12 + 1;		//	A
	for (int i = 0; i < 5; ++i) {
		while( !rcnt[--r] ) { }
		odr = (odr << 4) + r;
	}
	odr |= (HIGH_CARD << 20);
	return HIGH_CARD;
}
void print(const std::vector<Card> v, uint odr)
{
	for (uint i = 0; i < v.size(); ++i) {
		//std::cout << v[i].toString() << " ";
		v[i].printW();
		//std::cout << " ";
	}
	std::cout << std::hex << odr << std::dec << "\n";
}
//----------------------------------------------------------------------
//	ランダムハンドの相手一人に対する勝率（勝ち or 引き分け）を求める
double calcWinSplitProb(Card c1, Card c2, const std::vector<Card> &comu)
{
	assert( comu.size() <= 5 );
	const int N_LOOP = 10000;
	std::vector<Card> v1(7), v2(7);
	v1[0] = c1;
	v1[1] = c2;
	for (int i = 0; i < (int)comu.size(); ++i)
		v1[i+2] = v2[i+2] = comu[i];
	Deck deck;
	//	take は O(N) の時間がかかるので、ループの前に処理を行っておく
	deck.take(c1);
	deck.take(c2);
	for (int k = 0; k < (int)comu.size(); ++k)
		deck.take(comu[k]);
	int nWinSplit = 0;
	for (int i = 0; i < N_LOOP; ++i) {
		//if( i == 222 ) {
		//	std::cout << 222;
		//}
		deck.setNDealt(2+comu.size());		//	ディール済み枚数
		deck.shuffle();
		v2[0] = deck.deal();
		v2[1] = deck.deal();
		for (int k = (int)comu.size(); k < 5; ++k) {
			v1[k+2] = v2[k+2] = deck.deal();
		}
		uint od1 = 0, od2 = 0;
		checkHand(v1, od1);
		checkHand(v2, od2);
		//print(v1, od1);
		//print(v2, od2);
		if( od1 >= od2 )
			++nWinSplit;
	}
	return (double)nWinSplit / N_LOOP;
}
//	ランダムハンドの相手 np - 1人に対する勝率（勝ち or 引き分け）を求める
double calcWinSplitProb(Card c1, Card c2, const std::vector<Card> &comu, int np)
{
	assert( comu.size() <= 5 );
	const int N_LOOP = 10000;
	std::vector<std::vector<Card>> vv;
	for (int i = 0; i < np; ++i) {
		vv.push_back(std::vector<Card>(7));
	}
	vv[0][0] = c1;
	vv[0][1] = c2;
	for (int k = 0; k < np; ++k) {
		for (int i = 0; i < (int)comu.size(); ++i) {
			vv[k][i+2] = comu[i];
		}
	}
	Deck deck;
	//	take は O(N) の時間がかかるので、ループの前に処理を行っておく
	deck.take(c1);
	deck.take(c2);
	for (int k = 0; k < (int)comu.size(); ++k)
		deck.take(comu[k]);
	int nWinSplit = 0;
	const int NL = N_LOOP / np;
	for (int i = 0; i < NL; ++i) {
		deck.setNDealt(2+comu.size());		//	ディール済み枚数
		deck.shuffle();
		for (int j = (int)comu.size(); j < 5; ++j) {
			vv[0][j+2] = deck.deal();
		}
		uint odr0 = 0, odr = 0;
		checkHand(vv[0], odr0);
		//std::vector<uint> odr;
		for (int k = 1; k < np; ++k) {
			vv[k][0] = deck.deal();
			vv[k][1] = deck.deal();
			for (int j = (int)comu.size(); j < 5; ++j) {
				vv[k][j+2] = vv[0][j+2];
			}
			checkHand(vv[k], odr);
			if( odr > odr0 )
				break;
		}
		//print(v1, od1);
		//print(v2, od2);
		if( odr0 >= odr )
			++nWinSplit;
	}
	return (double)nWinSplit / NL;
}
//	現状のレイズ・コールを含めたチップ合計が pot の時に、call するかどうかの勝率閾値を計算
//	プレイヤー人数は合計２人とする
double calcThreshold(int pot, int call)
{
	return (double)call / (pot + call);
}
