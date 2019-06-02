make -f get_queen_home.make
for K_ROW in 0 1 2 3 4 5 6 7
do
	for K_COL in 0 1 2 3 4 5 6 7
	do
		for Q_ROW in 0 1 2 3 4 5 6 7
		do
			for Q_COL in 0 1 2 3 4 5 6 7
			do
				for ON_MOVE in 0 1
				do
					echo "King $K_ROW $K_COL Queen $Q_ROW $Q_COL On-Move ${ON_MOVE}"
					echo 8 0 3 $K_ROW $K_COL $Q_ROW $Q_COL $ON_MOVE | ./get_queen_home
				done
			done
		done
	done
done
