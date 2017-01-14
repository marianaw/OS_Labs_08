i=1
while [ $i -le 1000000 ]
do
	echo -en "\r$i";
  ./main 2305843009213693951 | grep "is not prime";
   i=`expr $i + 1`
done
