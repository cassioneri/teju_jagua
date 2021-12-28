set datafile separator ','

set lmargin 10
set bmargin 5

set xlabel "Floating point number" font "Noto Sans Black,12"
set ylabel "Time in nanoseconds" font "Noto Sans Black,12"

set tics font "Noto Sans,12"

set logscale x 2
set format x "2^{%L}"

set yrange [0:*]

set grid x y

set key autotitle columnhead
set key bottom center spacing 2 font "Arial,12" maxrows 1

plot "float.csv" using 2:3 with points pt 7 ps .5 lc "blue", "" using 2:4 with points pt 7 ps .5 lc "red"
