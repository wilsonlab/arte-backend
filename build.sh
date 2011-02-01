clear
rm test6
rm test.txt
g++ src/*.cpp -pthread -ldl -lnidaqmx -I /usr/local/boost_1_45_0/ -g -o test6