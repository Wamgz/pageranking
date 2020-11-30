#include <iostream>
#include "ReadFiles.h"
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <cstdio>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <exception>
#include <cstring>
#include <queue>
#include <stack>
#include <map>
#include <sys/types.h>

using namespace std;

#define SIZE 140432

int idx = 0;
int* numsOfEachCol = new int[SIZE];

Node::Node(int idx, float val){
    this -> idx = idx;
    this -> val = val;
}
Triple::Triple(int i, int j, float val){
    this -> i = i;
    this -> j = j;
    this -> val = val;
}
Triple::Triple(){
    this -> i = 0;
    this -> j = 0;
    this -> val = 0.0;
}
TSMatrix::TSMatrix(){
    this -> mu = 0;
    this -> nu = 0;
    this -> tu = 0;
}
TSMatrix::TSMatrix(int mu, int nu, int tu){
    this -> mu = mu;
    this -> nu = nu;
    this -> tu = tu;
}

//遍历文件夹，获取每个.html .htm .shtml的文件和对应idx
void urlToIndex(char *basePath, map<string, int>& url_to_idx, map<int, string>& idx_to_url){
    //path = E:/coding environment/pageranking-dataset/news.sohu.com
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (dir == NULL)
        return;

    struct stat s_buf;
    regex pattern("news\.sohu\.com.*\..?htm.?$");

    smatch sm;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            stat(path, &s_buf);

            string s(path);
            bool match = regex_search(s, sm, pattern);
            if(match){
                for(int i = 0;i < sm.size();i++){
                    url_to_idx.insert(pair <string, int> (sm[i], idx));
                    idx_to_url.insert(pair <int, string> (idx, sm[i]));
                    idx++;
                }
            }
            urlToIndex(path, url_to_idx, idx_to_url);

        }
    }

    closedir(dir);
}

//数组归一化
void graphNodeNormalizing(TSMatrix& tsmatrix){
    int size = tsmatrix.tu;
    for(int i = 0;i < size;i++){
        int col = tsmatrix.data[i].j;
        if(numsOfEachCol[col] != 0)
            tsmatrix.data[i].val = (1.0 * tsmatrix.data[i].val) / (1.0 * numsOfEachCol[col]);
    }
}

//数组计算
void addMatrix(float* left, float* right, float* vk_1){
    /*
        gm: 1 * n
        vk: 1 * n
        vk_1: 1 * n，初始化为vk
    */
    for(int i = 0;i < SIZE;i++){
        vk_1[i] = left[i] + right[i];
    }
}
void mulMatrixLeft(TSMatrix& gm, float* vk, float* left, float alpha){
    /*
        gm: n * n;
        vk: 1 * n;
    */

    for(int i = 0;i < gm.tu;i++){
        int row = gm.data[i].i;
        int col = gm.data[i].j;
        left[row] += (1 - alpha) * vk[col] * gm.data[i].val;
    }
}
void mulMatrixRight(float* vk, float* right, float alpha){
    /*
        gm: n * n;
        vk: 1 * n;
    */
   float sum = 0.0;
    for(int i = 0;i < SIZE;i++){
        sum  += alpha * vk[i] / SIZE;
    }
    for(int i = 0;i < SIZE;i++){
        right[i] = sum;
    }
}

float deltaOfArray(float* vk, float* vk_1){
    float delta = 0.0;
    for(int i = 0;i < SIZE;i++){
        delta += (vk_1[i] - vk[i]) * (vk_1[i] - vk[i]);
    }

    return sqrt(delta);
}

//打印向量
void display(float* arr){
    for(int i = 0;i < SIZE;i++){
        cout<<arr[i]<<" ";
    }

    cout<<endl;
}

//打印数组
void displayMatrix(TSMatrix& S){
    float mat[SIZE][SIZE];
    for(int i = 0;i < SIZE;i++){
        memset(mat[i], 0, sizeof(mat[i]));
    }
    for(int i = 0;i < S.tu;i++){
        int row = S.data[i].i;
        int col = S.data[i].j;
        mat[row][col] = S.data[i].val;
    }

    for(int i = 0;i < SIZE;i++){
        for(int j = 0;j < SIZE;j++){
            cout<<mat[i][j]<<"     ";
        }
        cout<<endl;
    }
}

void read_struct(TSMatrix& tsmatrix){
    ifstream reader("./graph.bin", ios::in|ios::binary);

	int t;
	reader.read((char*)&t, sizeof(int));
    tsmatrix.tu = t;
    // cout<<"start read"<<endl;
	for (int i = 0;i < t;i++){
		Triple temp;
		reader.read((char*)&temp, sizeof(Triple));
		tsmatrix.data.push_back(temp);
	}

    // cout<<"end read, total size of edges:  "<<t<<endl;
}

void computeNumOfEachCol(TSMatrix& tsmatrix){
    for(int i = 0;i < SIZE;i++){
        numsOfEachCol[i] = 0;
    }

    for(int i = 0;i < tsmatrix.data.size();i++){
        int row = tsmatrix.data[i].i;
        int col = tsmatrix.data[i].j;
        numsOfEachCol[col] ++;
    }
}

//优先队列cmp
struct cmp{
    bool operator() (Node a, Node b){
        return a.val > b.val;   
    }
};

bool cmpTriple(Triple a, Triple b)
{
    if(a.i != b.i){
        return a.i < b.i;
    }
    
    return a.j < b.j;
}
int main(){

    time_t start, finish;
	start = clock();
    char* basePath = "E:/coding environment/pageranking-dataset/news.sohu.com";  //真实路径
    // char* basePath = "C:/Users/Woods_WANG/Desktop/news.sohu.com/testdir";   //测试数据路径

    //获取每个url对应的idx
    map<string, int> url_to_idx;
    map<int, string> idx_to_url;

    urlToIndex(basePath, url_to_idx, idx_to_url);

    //开始建图

    //建图
    TSMatrix tsmatrix(SIZE, SIZE, 0);

    read_struct(tsmatrix);
    memset(numsOfEachCol, 0, sizeof(numsOfEachCol));    //初始化数组
    computeNumOfEachCol(tsmatrix);    


    // 对每一列进行归一化
    graphNodeNormalizing(tsmatrix);
    
    sort(tsmatrix.data.begin(), tsmatrix.data.end(), cmpTriple);


    float* vk = new float[SIZE];
    float* vk_1 = new float[SIZE];
    float* left = new float[SIZE];
    float* right = new float[SIZE];

    //初始化数组
    for(int i = 0;i < SIZE;i++){
        vk_1[i] = 0.0;
        vk[i] = 1.0 * numsOfEachCol[i] / tsmatrix.tu;
    }
    ofstream OutFile("resultPR.txt"); //利用构造函数创建txt文本，并且打开该文本

    float epsilon = 0.001;
    float alpha = 0.15;
    // 矩阵乘法，加法
    for(int i = 0;i < 100;i++){
        for(int j = 0;j < SIZE;j++){
            left[j] = 0.0;
            right[j] = 0.0;
        }
        mulMatrixLeft(tsmatrix, vk, left, alpha);
        mulMatrixRight(vk, right, alpha);
        addMatrix(left, right, vk_1);
        float delta = deltaOfArray(vk, vk_1);
        if(delta < epsilon){
            OutFile<<i<<" iteration"<<endl;
            break;
        }
        for(int j = 0;j < SIZE;j++){
            vk[j] = vk_1[j];
        }
    }
    int k = 20;
    priority_queue<Node,vector<Node>, cmp> pq;
    for(int i = 0;i < SIZE;i++){
        if(pq.size() < k){
            pq.push(Node(i, vk_1[i]));
        }
        else{
            if(vk_1[i] > pq.top().val){
                pq.pop();
                pq.push(Node(i, vk_1[i]));
            }
        }
    }

    stack<Node> st;
    for(int i = 0;i < k;i++){
        int idxOfVk_1 = pq.top().idx;
        float val = pq.top().val;
        st.push(Node(idxOfVk_1, val));
        pq.pop();
    }
    int i = 0;

    while(!st.empty()){
        Node cur = st.top();
        cout<<"i: "<<i++<<"  "<<cur.val<<"  "<<idx_to_url[cur.idx]<<endl;
        st.pop();
    }
    finish = clock();
    
	printf("pagerank runtime: %d\n",(finish - start) );

    return 0;
}