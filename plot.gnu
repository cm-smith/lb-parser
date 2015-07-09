set terminal dumb
set boxwidth 0.1
set style fill solid
set terminal png
set output ofilename
plot filename using 1:2 with boxes
