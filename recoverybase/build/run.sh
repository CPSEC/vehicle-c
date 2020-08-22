#!/bin/bash

sudo rm log.txt
sudo rm -rf ./checkpoint

sudo ps -ef | grep recovery | grep -v grep | awk '{print $2}' | xargs sudo kill -9 > /dev/null 2>&1
echo -e "\n\n\n" >>log.txt

sudo rm -rf ./checkpoint

#make clean
cmake .. > /dev/null
make >>log.txt 2>&1
echo -e "\n\n\n" >>log.txt

echo -e "start ..."
echo -e "start ...\n" >>log.txt

for ((i = 1; i <= 10; i++)); do
    sudo ./recovery >>log.txt 2>&1

    sudo ps -ef | grep recovery | grep -v grep | awk '{print $2}' | xargs sudo kill -9 > /dev/null 2>&1
    echo -e "\n\n\n" >>log.txt
done

cat log.txt | grep total

cat log.txt | awk '{sum+=$6;} END {print sum/10}'
