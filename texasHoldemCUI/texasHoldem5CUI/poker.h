#include <vector>

struct Card;

enum {
	HIGH_CARD = 0,
	ONE_PAIR,
	TWO_PAIR,
	THREE_OF_A_KIND,
	STRAIGHT,
	FLUSH,
	FULL_HOUSE,
	FOUR_OF_A_KIND,
	STRAIGHT_FLUSH,
	N_KIND_HAND,
};
const char *handName[];
typedef unsigned int uint;
int checkHand(const std::vector<Card> &v);
int checkHand(const std::vector<Card> v, uint &odr);
double calcWinSplitProb(Card c1, Card c2, const std::vector<Card> &comu);
double calcWinSplitProb(Card c1, Card c2, const std::vector<Card> &comu, int np);
double calcThreshold(int pot, int call);
