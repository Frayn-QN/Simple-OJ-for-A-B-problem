# Simple-OJ-for-ABproblem
微光招新题c-05。Linux系统下实现一个简单的OJ来判断客户端提交的“A+B Problem"是否正确。运行截图放在了题解中。

head为头文件目录。

服务端(server)：temp存放判题时的临时文件，正常情况下应该为空(git无法上传空文件夹，需要添加.gitkeep空文件)。data存放.in文件和.ans文件（题解中有说明）

客户端1(client1)：upload内为上传的题目，为正确的a+b.c

客户端2(client2)：upload内为上传的题目，为无法通过编译的a+b.c
