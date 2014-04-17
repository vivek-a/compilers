
# !/bin/bash

make -f Makefile.cfglp

rm -rf tmp
mkdir tmp

 for file in "test_files"/*.c 
 do
 	file=`echo $file | cut -d '/' -f2`
 	make -f Makefile.cfg FILE=$file
 done

echo "\nProcessing correct files"
echo "-------------------------\n"

for file in "test_files"/*.cs306.cfg 
do
	echo $file
	f=`echo $file | cut -d '/' -f2`
	f=`echo $f | cut -d '.' -f1`
	./cfglp $file  -d   -symtab -tokens -ast -eval > out1
	./cfglp64 $file  -d  -symtab -tokens -ast -eval  > out2
	diff  out1 out2  > tmp/$f
done

# echo "\n\nProcessing error files"
# echo "-----------------------\n"

# for file in "test_files"/*.ecfg 
# do
# 	echo $file
# 	f=`echo $file | cut -d '/' -f2`
# 	f=`echo $f | cut -d '.' -f1`
# 	./cfglp $file -icode -tokens -ast -lra  -d > out1
# 	./cfglp64 $file -icode -tokens -ast -lra  -d > out2
# 	diff out1 out2 > tmp/$f
# 	echo "-------------------------------------------"
# done

ls -l tmp
