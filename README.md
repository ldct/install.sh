# install.sh
monitored install scripts

ssh -o "StrictHostKeyChecking no" root@104.236.190.235 "apt-get update; apt-get install --assume-yes git build-essential; git clone https://github.com/zodiac/install.sh.git; cd install.sh; ./make"