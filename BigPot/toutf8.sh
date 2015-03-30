echo -ne '\xEF\xBB\xBF' > temp.txt
iconv -f cp936 -t utf-8 $1 >> temp.txt
cp temp.txt $1