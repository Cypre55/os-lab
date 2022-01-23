curl https://www.example.com/>example.html
curl 'http://ip.jsontest.com/'|awk '{print $2}'|tr -d '\"\}'
curl "http://headers.jsontest.com">tmp.json
IFS=,
for i in $REQ_HEADERS; do result=$(cat tmp.json|grep $i|awk '{print $2}');echo $i:$result;done;
IFS=$' \t\n'
for i in $(ls JSONData); do cat ./JSONData/$i|jq -e .>/dev/null 2>&1;if [[ "${PIPESTATUS[1]}" -eq 0 ]];then echo $i>>valid.txt;else echo $i>>invalid.txt;fi;done;
