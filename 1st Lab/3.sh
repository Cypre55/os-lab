find data -type f| sed -n 's/^.*\///pg' | awk -F "." '/.*\./ {print $NF} /[^.]/ {print "Nil"}' | sort |uniq|xargs mkdir
shopt -s globstar
for i in $(ls); do
    echo $i
    printf '%s\0' data/**/*.$i | xargs -0 mv -t ./$i
done;
find data -type f -print0 | xargs -0 mv -t ./Nil
