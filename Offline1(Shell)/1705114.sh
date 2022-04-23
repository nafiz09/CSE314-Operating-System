#!/bin/bash
working_directory=`pwd`
inputfile_name=""
current_directory=`pwd`

if [ $# = "0" ]; then
	echo "no cmd line arg: working directory name (optional) and input file name must be given"

elif [ $# = "1" ]; then
	if [[ $1 == *".txt"* ]]; then
		find ./ -type f -name $1 | grep -q $1 > /dev/null && inputfile_name=$1 
	else
		echo "input file name must be given"
		find ./ -type d -name $1 | grep -q $1 > /dev/null && working_directory=`find ./ -type d -name $1` 
	fi

elif [ $# = "2" ]; then
	if [[ $2 == *".txt"* ]]; then 
		find ./ -type f -name $2 | grep -q $2 > /dev/null && inputfile_name=$2
	else
		echo "input file name must be given"
	fi	

	find ./ -type d -name $1 | grep -q $1 > /dev/null && working_directory=`find ./ -type d -name $1` 

else
	echo "too much cmd line arg: only working directory name (optional) and input file name must be given"
fi

until [ "$inputfile_name" != "" ]
do
	echo -n "give valid input file name: "
	read temp

	if [[ $temp == *".txt"* ]]; then 
		find ./ -type f -name $temp | grep -q $temp > /dev/null && inputfile_name=$temp
	fi
done

it=(`cat $inputfile_name`)
cd $working_directory
cd ..
mkdir output_dir

find="find . ! ( -name *.${it[0]}"

for i in `cat in.txt`
do
    #echo $i
    find+=" -o -name *.$i"
done

#echo $var1
find+=' )'

echo $find

$find

for i in `$find`
do
    echo $i
    if [ ! -d $i ]
    then
        EXTENSION=`echo "$i" | cut -d'.' -f3`
        echo $EXTENSION
        if [ -z $EXTENSION ]
        then
            mkdir output_dir/others
            echo $i >> output_dir/others/desc_$EXTENSION.txt
            cp $i output_dir/others
        else      
            mkdir output_dir/$EXTENSION
            echo $i >> output_dir/$EXTENSION/desc_$EXTENSION.txt
            cp $i output_dir/$EXTENSION 
        fi
    fi    
done

csv_file_name="output.csv"
echo "File type, Number of files" > $csv_file_name
cd output_dir
for i in `ls -d */`
do
    cd $i
    var1=`ls -l | wc -l`
    #echo $var1
    var1=`expr $var1 - 2`
    #echo $var1
    cd ..
    EXTENSION=`echo "$i" | cut -d'/' -f1`
    echo $EXTENSION,$var1 >> ../$csv_file_name
done
