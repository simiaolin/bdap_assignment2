#!/bin/sh -v
eval `ssh-agent -s` && ssh-add ~/.ssh/id_rsa
scp -r /Users/ary/CLionProjects/bdap_2/lib  r0829520@balen.cs.kotnet.kuleuven.be:/home/r0829520/assign2/bdap_2/
scp /Users/ary/CLionProjects/bdap_2/*.sh  r0829520@balen.cs.kotnet.kuleuven.be:/home/r0829520/assign2/bdap_2/
#scp  /Users/ary/CLionProjects/bdap_2/CMakeLists.txt  r0829520@balen.cs.kotnet.kuleuven.be:/home/r0829520/assign2/bdap_2
#scp -r /Users/ary/CLionProjects/bdap_2/test/data/  r0829520@balen.cs.kotnet.kuleuven.be:/home/r0829520/assign2/bdap_2/test/
scp  /Users/ary/CLionProjects/bdap_2/test/*.cpp  r0829520@balen.cs.kotnet.kuleuven.be:/home/r0829520/assign2/bdap_2/test
#scp  /Users/ary/CLionProjects/bdap_2/test/CMakeLists.txt r0829520@balen.cs.kotnet.kuleuven.be:/home/r0829520/assign2/bdap_2/test
scp  /Users/ary/CLionProjects/bdap_2/test/*.h r0829520@balen.cs.kotnet.kuleuven.be:/home/r0829520/assign2/bdap_2/test
scp  /Users/ary/CLionProjects/bdap_2/test/*.sh r0829520@balen.cs.kotnet.kuleuven.be:/home/r0829520/assign2/bdap_2/test
ssh r0829520@balen.cs.kotnet.kuleuven.be

cd /home/r0829520/assign2/bdap_2 &&./install.sh
cd /home/r0829520/assign2/bdap_2/test/build &&./DecisionTreeTest full


ssh -L 8080:mysql.cs.kotnet.kuleuven.be:80 r0829520@st.cs.kuleuven.be
ssh r0829520@ans.cs.kotnet.kuleuven.be
ssh r0829520@heers.cs.kotnet.kuleuven.be
ssh r0829520@hasselt.cs.kotnet.kuleuven.be
ssh r0829520@ohey.cs.kotnet.kuleuven.be
ssh r0829520@knokke.cs.kotnet.kuleuven.be
ssh r0829520@gent.cs.kotnet.kuleuven.be

