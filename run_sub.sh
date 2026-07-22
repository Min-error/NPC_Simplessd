#!/bin/bash
pathold='test1.revised'
indexold='FTLINDEX = 1'

cacheold='CacheSize=536870912'
blockold='Page=512'
dieold='Die=2'

for j in {1..1}
    do
        # j = j + 8
        path='test'$j'.revised'
        str='s/'${pathold}'/'${path}'/g'
        sed  -i $str config/sample.cfg 
        pathold=$path
        #echo $pathold
        l=0
        sed -i 's/FTLINDEX = .*/FTLINDEX = 64'$j'/g' simplessd/config/sample.cfg
        indexold=$index
        #echo $indexold
        echo '********************************************'
        echo $path
        echo "FTLINDEX = 64"$j
        echo '********************************************'
        ./simplessd-standalone config/sample.cfg simplessd/config/sample.cfg result
        echo '********************************************'
        echo $pathold
        echo $indexold
        echo $j      
    done    

# str='s/'${cacheold}'/CacheSize=536870912/g'
# sed  -i $str simplessd/config/sample.cfg
str='s/'${pathold}'/test1.revised/g'
sed  -i $str config/sample.cfg

sed -i 's/FTLINDEX = .*/FTLINDEX = 1/g' simplessd/config/sample.cfg


str='s/'${blockold}'/Page=512/g'
sed  -i $str simplessd/config/sample.cfg

str='s/'${dieold}'/Die=2/g'
sed  -i $str simplessd/config/sample.cfg