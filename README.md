# GetQueenHome

Solution to Reddit Daily Programmer idea https://www.reddit.com/r/dailyprogrammer_ideas/comments/bpgs79/hard_get_the_queen_home/.

The program expects the below arguments on standard input:

- Board size (8 for standard game)

- Queen home row/column (0-indexed, 0 3 for standard game)

- King starting row/column (0-indexed)

- Queen starting row/column (0-indexed)

- On-move (0 = King, 1 = Queen)

First all possible positions are generated (8192 for standard game): for each one it is determined if it is legal and a winning state of the game.

Then an incremental alpha-beta pruning search is performed until a winner is found, each side playing optimally.

The bash script gqh_standard_test.sh runs the program for all possible starting positions of the standard game. It completes after 6 minutes on my current computer.
