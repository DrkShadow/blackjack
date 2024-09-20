

gcc -g -c blackjack.c -o blackjack.o
gcc -g -c deal.c -o deal.o
gcc -g -o blackjack deal.o blackjack.o
