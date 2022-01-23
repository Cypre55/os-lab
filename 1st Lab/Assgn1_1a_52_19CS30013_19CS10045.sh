N=$1;for((i=2;i<=$N;i++));do if((N%i==0));then N=$((N/i));echo $i;((i--));fi;done;
