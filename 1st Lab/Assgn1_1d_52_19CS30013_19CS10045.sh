mkdir -p ../files_mod&&sed 's/ /,/g' $1|awk '{print NR","$0}'>../files_mod/$1
