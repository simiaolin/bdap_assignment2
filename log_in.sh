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
ssh r0829520@ham.cs.kotnet.kuleuven.be

cd /home/r0829520/assign2/bdap_2 &&./install.sh
#cd /home/r0829520/assign2/bdap_2/test/build &&./DecisionTreeTest full
cd /home/r0829520/assign2/bdap_2/test/build &&./BaggingTest full

cd /home/r0829520/assign2/bdap_2/test/build &&  valgrind --tool=memcheck --leak-check=full ./BaggingTest full
cd /home/r0829520/assign2/bdap_2/test/build &&  valgrind --tool=memcheck --leak-check=yes ./BaggingTest full
cd /home/r0829520/assign2/bdap_2/test/build &&  valgrind --tool=memcheck --leak-check=full -v ./BaggingTest full


ssh -L 8080:mysql.cs.kotnet.kuleuven.be:80 r0829520@st.cs.kuleuven.be
ssh r0829520@ans.cs.kotnet.kuleuven.be
ssh r0829520@heers.cs.kotnet.kuleuven.be
ssh r0829520@hasselt.cs.kotnet.kuleuven.be
ssh r0829520@ohey.cs.kotnet.kuleuven.be
ssh r0829520@knokke.cs.kotnet.kuleuven.be
ssh r0829520@waterloo.cs.kotnet.kuleuven.be
ssh r0829520@yvoir.cs.kotnet.kuleuven.be
ssh r0829520@ham.cs.kotnet.kuleuven.be
ssh r0829520@gent.cs.kotnet.kuleuven.be
ssh r0829520@brugge.cs.kotnet.kuleuven.be

dmesg | grep "killed"

1. keep split size
2. use map to avoid multi read
3. parallel
4. report


#kill heers balen (1) gent waterloo（0）knokke (1) yvoir(1) ans(1)

#ohey hasselt ham brugge