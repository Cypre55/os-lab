(for i in {1..150};do for j in {1..10};do (printf "%d," $RANDOM)done;echo;done;)|sed -n 's/,$//p'>$1
output=$(awk -v N=$2 -F "," '{print $N}' $1|grep -e $3)
[ -z "$output" ]&&echo "NO"||echo "YES"
