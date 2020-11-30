#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <sys/types.h>
#include <dirent.h>
#include <vector>

using namespace std;

class Triple{
    public:
        int i, j;
        float val;

        Triple();
        Triple(int i, int j, float val);
};

class TSMatrix{
    public:
        int mu, nu, tu;
        vector<Triple> data;
        // Triple data[MAXSIZE];

        TSMatrix();
        TSMatrix(int mu, int nu, int tu);
};

class Node{
    public:
        int idx;
        float val;
        Node(int idx, float val);
};

// void listFilesRecursively(char *basePath, map<string, int>& url_to_idx, map<int, string>& idx_to_url);

//遍历文件夹获取.htm,.html,.shtml结尾的文件作为key，自增的idx作为value
void urlToIndex(string path, map<string, int>& url_to_idx, map<int, string>& idx_to_url);


//遍历每个文件，筛选出new.sohu.com里的文件，文件的idx为i，每次搜到则查找该url是否存在map中，如果存在，则取出该url对应idx记为j，添加graph的节点为(j, i, 1)
void createGraph(string path, TSMatrix& tsmatrix, map<string, int>& url_to_idx);


//读取文件到字符串
string readFileIntoString(char * filename);

//写入图到bin文件
void writeToBinFile(TSMatrix& tsmatrix);


