# pageranking

基于c++实现了pageranking算法，其中用于实现的文件有大约14万个url，文件较大不方便上传，有需要可以联系我的邮箱woodswang1025@gmail.com单独发送

包含了ReadFiles和pageranking两部分

其中ReadFiles读取所有url文件并将url文件建立图，写入graph.bin文件，考虑到文件稀疏性，采用了三元组方式存储

Pageranking算法实现对graph.bin文件的读取，并实现二维矩阵和一维矩阵相乘和二维矩阵相加的操作，具体原理请查阅pageranking算法，最后取前20项输出，
方式是使用优先队列取前20项，再输入到栈中逆序
