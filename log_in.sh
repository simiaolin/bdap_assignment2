#!/bin/sh -v
eval `ssh-agent -s` && ssh-add ~/.ssh/id_rsa
scp -r /Users/ary/CLionProjects/bdap_2/  r0829520@balen.cs.kotnet.kuleuven.be:/home/r0829520/assign2
ssh r0829520@balen.cs.kotnet.kuleuven.be
cd /home/r0829520/assign2/bdap_2
./install.sh
cd /home/r0829520/assign2/bdap_2/test/build
