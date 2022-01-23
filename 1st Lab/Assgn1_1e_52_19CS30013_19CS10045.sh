function log () {if [[ "$2" == *"v"* ]];then echo "$1";fi;}
log "Downloading example.html" $1
curl https://www.example.com/>example.html
log "Getting IP" $1
curl 'http://ip.jsontest.com/'|awk '{print $2}'|tr -d '\"\}'
log "Checking Headers" $1
curl "http://headers.jsontest.com">tmp.json
IFS=,
for i in $REQ_HEADERS; do result=$(cat tmp.json|grep $i|awk '{print $2}');echo $i:$result;
done;
log "Validating JSONs" $1
IFS=$' \t\n'
for i in $(ls JSONData); do cat ./JSONData/$i|jq -e .>/dev/null 2>&1;if [[ "${PIPESTATUS[1]}" -eq 0 ]];then echo $i>>valid.txt;else echo $i>>invalid.txt;fi;done;
