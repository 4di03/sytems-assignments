ipcs -m | awk '$5 <= 64 && $2 != "shmid" {print $2}' | xargs -n 1 ipcrm -m
