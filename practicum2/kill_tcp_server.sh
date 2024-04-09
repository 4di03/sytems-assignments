ps -a | grep 'server' | awk '{print $1}' | xargs kill -TERM 
# kill server with sigint

kill -9 $(lsof -t -i:2000 -sTCP:LISTEN)