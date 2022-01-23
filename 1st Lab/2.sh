mkdir ../1.b.files.out&&ls|xargs -I {} bash -c 'cat "{}"|sort -n>../1.b.files.out/{}'
ls|xargs cat|sort -n|uniq -c|awk '{print $2" "$1}'>../1.b.out.txt