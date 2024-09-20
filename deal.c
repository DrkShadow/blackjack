
// deal out x decks of cards. Supply array of 52 * decks length of
// integers; each integer, 1-52, is a card; 0-12: spade; 13-25: diamond;
// 26-38: club; 39-51: hearts

#include <stdlib.h>

void createDeck(int numdecks, int *output);
void shuffle(int numdecks, int *deck);

void createDeck(int numdecks, int *output) {
	// Assume enough space in the output for numdecks * 52 cards of type int.
	
	int i = 0;
	int j = 0;
	for (i = 0; i < 52 * numdecks; i++) {
		if (j == 52) {
			j = 0;
		}
		output[i] = j++;
	}

	shuffle(numdecks, output);
}

// Loop through some random data rearranging numbers in the deck two at a time.
void shuffle(int numdecks, int *deck) {

	// max of random() is 2147483647
	// Add +1 to the divisor to guarantee we'll never get back exactly the number of
	// decks, i.e. 1/1, overflow. 
	
	int i = 0;
	for (i = 0; i < 52 * numdecks; i++) {
		int randcard, randcard2, tempcard;
		randcard = random() / (2147483647 / (52 * numdecks) + 1);
		randcard2 = random() / (2147483647 / (52 * numdecks) + 1);
	
		tempcard = deck[randcard2];
		deck[randcard2] = deck[randcard];
	}
}
