#!/bin/sh
export PIN_ROOT=~/pin-2.13-61206-gcc.4.4.7-linux
make PIN_ROOT=$PIN_ROOT

# LRU
datFilename=plotLRU.dat
policy=LRU
rm -rf $datFilename

echo "# LRU" > $datFilename
echo "# Size 1-way 2-way 4-way 8-way Fully" >> $datFilename

echo -n "4096" >> $datFilename
for ASSOC in 1 2 4 8 64
do
echo "Running ${ASSOC}-way, 4K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 4096 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

echo -n "32768" >> $datFilename
for ASSOC in 1 2 4 8 512
do
echo "Running ${ASSOC}-way, 32K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 32768 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

echo -n "262144" >> $datFilename
for ASSOC in 1 2 4 8 4096
do
echo "Running ${ASSOC}-way, 256K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 262144 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

echo -n "2097152" >> $datFilename
for ASSOC in 1 2 4 8 32768
do
echo "Running ${ASSOC}-way, 2048K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 2097152 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

cat $datFilename.
gnuplot plotLRU.plg
echo "================================="
echo "Image exported to results-LRU.png"
echo "================================="

# RANDOM
datFilename=plotRandom.dat
policy=RANDOM
rm -rf $datFilename

echo "# RANDOM" > $datFilename
echo "# Size 1-way 2-way 4-way 8-way Fully" >> $datFilename

echo -n "4096" >> $datFilename
for ASSOC in 1 2 4 8 64
do
echo "Running ${ASSOC}-way, 4K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 4096 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

echo -n "32768" >> $datFilename
for ASSOC in 1 2 4 8 512
do
echo "Running ${ASSOC}-way, 32K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 32768 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

echo -n "262144" >> $datFilename
for ASSOC in 1 2 4 8 4096
do
echo "Running ${ASSOC}-way, 256K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 262144 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

echo -n "2097152" >> $datFilename
for ASSOC in 1 2 4 8 32768
do
echo "Running ${ASSOC}-way, 2048K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 2097152 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

cat $datFilename
gnuplot plotRandom.plg
echo "===================================="
echo "Image exported to results-RANDOM.png"
echo "===================================="

# FIFO
datFilename=plotFIFO.dat
policy=FIFO
rm -rf $datFilename

echo "# FIFO" > $datFilename
echo "# Size 1-way 2-way 4-way 8-way Fully" >> $datFilename

echo -n "4096" >> $datFilename
for ASSOC in 1 2 4 8 64
do
echo "Running ${ASSOC}-way, 4K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 4096 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

echo -n "32768" >> $datFilename
for ASSOC in 1 2 4 8 512
do
echo "Running ${ASSOC}-way, 32K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 32768 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

echo -n "262144" >> $datFilename
for ASSOC in 1 2 4 8 4096
do
echo "Running ${ASSOC}-way, 256K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 262144 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

echo -n "2097152" >> $datFilename
for ASSOC in 1 2 4 8 32768
do
echo "Running ${ASSOC}-way, 2048K, ${policy}"
$PIN_ROOT/pin -t obj-intel64/pincache.so -s 2097152 -a $ASSOC -replace $policy -- tar czf data1.tgz ../data1
cat cache.out | tr -d "\n" >> $datFilename
done
echo "" >> $datFilename

cat $datFilename
gnuplot plotFIFO.plg
echo "=================================="
echo "Image exported to results-FIFO.png"
echo "=================================="