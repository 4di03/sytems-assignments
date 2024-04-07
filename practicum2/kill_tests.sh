ps -a | grep 'tests' | awk '{print $1}' | xargs kill -9
