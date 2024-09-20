
// Try the Match the Dealer's Card trick, with modification that I will only start a
// match attempt after at least 5(?) failed matches. The justification being not matching
// N times is very, very unlikely, despite the ods of getting a match being very unlikely.
//
// In theory, perhaps this boils down to, "After how many misses does the probability of
// another miss become less than the probability of a match?" and start playing cards,
// with an additional modifier to the bet amount to recoup previous losses.

#include <stdio.h>
#include <stdlib.h>

void createDeck(int numdecks, int *output);
void shuffle(int numdecks, int *deck);

const int numDecks = 8;

int main(int argc, char **argv) {
	int deck[52 * numDecks];

	// Start dealing cards..
	createDeck(numDecks, deck);

	// The cut will persist between games, being reset each shuffle.
	int deckcard = 0, cut = 0;
	int lastmatchdigit = 0, lastmatchsuit = 0;
	int bet = 0;
	int betstart = 0;

	int balance = 10000;

	long runtime = 0;
	int curloss = 0;			// The current cash loss
	int maxloss = 0;			// The maximum cash loss
	int lostbets = 0;			// The running total of number of failed bets since last win
	int maxlostbets = 0;		// The greatest number of lost bets in a row

	// Tunable parameters
	const int startbetting = 1;	// How many losses in, start placing bets.
	const int initialbet = 50;	// 15, table min, 25 after 9pm
	const int betinc = 5;			// Bet increase after each bet loss
	const int recountOnDeal = 0;


	// THe first cut... take random over the deck.. 45% to 94%, 187 to 391, difference 204
	// This allows 9 players (8 + dealer) two cards each (18) after the cut, just in case.
	cut = random() / (2147483647 / 204) + 187;

	do {
		int players;
		int mycard;
	
		if (deckcard > cut) {
			// If we've passed the cut in the deck, reshuffle and 
			shuffle(numDecks, deck);
			printf("Redeal.\n");

			cut = random() / (2147483647 / 204) + 187;
			deckcard = 0;

			// On reshuffle, the last match count resets.
			if (recountOnDeal) {
				lastmatchdigit = 0;
				bet = 0;
			}
		}

	
		if (lastmatchdigit == startbetting) {
			bet = initialbet;
		}
		else if (lastmatchdigit > startbetting && bet < 1000) {
			// We can bet up to 1000.
			bet += betinc;
		}

		// This hand,
		// 1. How many players? Up to eight.
		players = random() >> 28;
		if (!players)
			players = 1;
		// 2. Which player am I? Never more than four.
		mycard = random() >> 29;

		// Deal N players, where N can change from 2 to 8.
		// My cards are 3 and N + 1.
		// .... this is simply done programmatically. The dealer's card is players+1 (0-based).
		
		// Now: Examine the cards. Determine:
		// 1. Number cards since last match. Reset on match.
		// 2. If there is a match. Award money.
		// 3. If we're N since we started betting, subtract losses.
		// 4. Increment counters from match/bet start
		
		// If we're at lastmatch is 5 away, then if either of our cards matches the _one_ visible dealer card, ...
		if (lastmatchdigit >= startbetting && (deck[mycard + deckcard] % 13 == deck[deckcard + players + 1 ] % 13 ||
				deck[mycard + players + 1 + deckcard] % 13 == deck[deckcard + players + 1] % 13)) {
			// If I'm betting here, then add it.
			if (deck[mycard + deckcard] == deck[deckcard + players + 1 - mycard]) {
				// This is a full match, suit and number. Are the 45-1 payouts for face cards?
				// Assume this is a 25-to-1 payout.
				balance += 25 * bet;
				printf("Huge payout\n");
			}
			else {
				// no if: already verifide the 10-to-1 odds that the number matches.
				balance += 10 * bet;
			}

			if (lostbets > maxlostbets) {
				maxloss = curloss;
				maxlostbets = lostbets;
			}

			curloss = 0;
			lostbets = 0;
			lastmatchdigit = 0;
		}
		else {
			if (lastmatchdigit >= startbetting) {
				// Since we didn't match, we lose our bet. Increment bet by 5$ to recoup our losses.

				lostbets++;
				curloss += bet;
				balance -= bet;
			}

			lastmatchdigit++;
		}

		int deckstart = deckcard;
		deckcard += (players + 1) << 1;

		// Play the round. Just to burn cards and make things more real; this doesn't impact whether
		// we've won or lost.
		int j;
		for (j = 0; j <= players; j++) {	// <= bec. dealer.
			int ace = 0;
			int cards = 2;

			int handcount = deck[deckstart + j] % 13 + 1 + deck[deckstart + j + players + 1] % 13 + 1;

			// Look at the cards in the hand. Is either an ace?
			if (deck[deckstart + j] % 13 == 0) {
				ace++;
				handcount += 10;	// We already counted the 1 (0).
			}
			if (deck[deckstart + j + players + 1] % 13 == 0) {
				ace++;
				handcount += 10;
			}

			while  (handcount < 16) {
				// < 16, hit. Aiming for 19.
				// Is this new card an ace?
				int card = deck[deckcard++];
				if (card % 13 == 0) {
					ace++;
					handcount += 10;
				}
				handcount += card + 1;	// +1 here bec. cards are 0-based; we took this into account for the first two.
				cards++;

				// If we bust, subtract 10 off an an ace (ace becomes worth 1).
				while (handcount > 21 && ace > 0) {
					handcount -= 10; // ace 11 -> 1
					ace--;
				}

				// We can run over if hands go too long..
				if (deckcard == 52 * numDecks)
					break;
			}

			if (deckcard == 52 * numDecks)
				break;

			// Winners don't matter. I'm not playing to win the blackjack.
		}

		// Don't consider the main game. Each game is a loss.
		balance -= 15;

		printf("Run: %5li; this round: (%s, %s); lastmch: %2d; balance: %6d, bet: %3i, max lost: %i, max loss run: %i\n", runtime++, (lastmatchdigit == 0 && lostbets == 0 || lastmatchdigit >= startbetting ? "bet" : "abs"), (lastmatchdigit == 0 && lostbets == 0 ? " win": "loss"), lastmatchdigit, balance, (lastmatchdigit > 0 && lastmatchdigit < startbetting ? 0 : bet), maxloss, maxlostbets);

		// There comes a point where you just need to give up.
		if (balance <= -10000)
			break;

	} while (1);

	return 0;
}
