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
#include <cstdio>


#define SIZE 140432

using namespace std;

//全局变量
int idx = 0;
vector<int> edges[SIZE];
//类初始化
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
    // memset(this -> data, 0, sizeof(this -> data));
}
TSMatrix::TSMatrix(int mu, int nu, int tu){
    this -> mu = mu;
    this -> nu = nu;
    this -> tu = tu;
    // memset(this -> data, 0, sizeof(this -> data));
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

//读取文件为字符串
string readFileIntoString(char * filename)
{
    ifstream ifile(filename);
    //将文件读入到ostringstream对象buf中
    ostringstream buf;
    char ch;
    while(buf && ifile.get(ch))
    buf.put(ch);

    ifile.close();
    //返回与流对象buf关联的字符串
    return buf.str();
}

//建图
void createGraph(char* basePath, TSMatrix& tsmatrix, map<string, int>& url_to_idx){
    // cout<<basePath<<endl;
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    // cout<<idx++<<".  "<<basePath<<endl;
    if (dir == NULL)
        return;

    struct stat s_buf;
    regex patternfile("news\.sohu\.com.*\..?htm.?$");
    regex patternurl("news\.sohu\.com[0-9a-zA-Z/]*?\..?htm[l]?");
    smatch smfile;
    smatch smurl;

    while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            //将path加上前缀
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            stat(path, &s_buf);
            string s(path);

            //判断是否是.html类似的文件
            bool matchfile = regex_search(s, smfile, patternfile);
            //如果是.html文件
            if(matchfile){
                int row = -1;
                if(url_to_idx.count(smfile[0]) > 0){
                    row = url_to_idx[smfile[0]];
                }
                //读取整个文件
                string filecontent = readFileIntoString(path);
                std::sregex_iterator iter(filecontent.begin(), filecontent.end(), patternurl);     //获取匹配所有可能
                std::sregex_iterator end;
                while (iter != end){
                    string tmp_s = iter -> str();
                    int col = -1;
                    if(url_to_idx.count(tmp_s) > 0){
                        col = url_to_idx[tmp_s];
                    }
                    if(col >= 0){
                        bool flag = true;
                        for(int i = 0;i < edges[col].size();i++){
                            if(edges[col][i] == row){
                                flag = false;
                                break;
                            }
                        }
                        if(flag == true){
                            Triple writetmp(col, row, 1.0);
                            if(row < 0){
                                cout<<"row: -1  col: "<<col<<endl;
                            }
                            edges[col].push_back(row);
                            if(tsmatrix.tu % 10000 == 0){
                                cout<<tsmatrix.tu<<"  "<<"row  "<<col<<"  col  "<<row<<"  "<<tmp_s<<"  "<<path<<endl;
                            }
                            tsmatrix.data.push_back(writetmp);
                            tsmatrix.tu++;
                        }
                    }
                    iter++;
                }   

            }
            createGraph(path, tsmatrix, url_to_idx);
        }
    }

    closedir(dir);
}


//写入bin文件
void writeToBinFile(TSMatrix& tsmatrix){
    ofstream writer("./graph.bin", ios::out|ios::binary);
	int t = tsmatrix.data.size();
	writer.write((char*)&t, sizeof(int));

	for (int i = 0; i < tsmatrix.data.size(); i++){
		Triple temp = tsmatrix.data[i];
		writer.write((char*)&temp, sizeof(Triple));

	}
    writer.close();
}

//读取bin文件
void read_struct(TSMatrix& tsmatrix){
    ifstream reader("./graph.bin", ios::in|ios::binary);

	vector<Triple> vec;
	int t;
	reader.read((char*)&t, sizeof(int));
	for (int i = 0;i < t;i++){
		Triple temp;
		reader.read((char*)&temp, sizeof(Triple));
		vec.push_back(temp);
	}
}
struct cmp{
    bool operator() ( Node a, Node b ){
        return a.val < b.val;   
    }
};

int main()
{
    time_t start, finish;
	start = clock();
    char* basePath = "./news.sohu.com";  //真实路径
    // char* basePath = "C:/Users/Woods_WANG/Desktop/news.sohu.com/testdir";   //测试数据路径

    //开始建图
    //获取每个url对应的idx
    map<string, int> url_to_idx;
    map<int, string> idx_to_url;

    urlToIndex(basePath, url_to_idx, idx_to_url);

    //建图
    TSMatrix tsmatrix(SIZE, SIZE, 0);
    createGraph(basePath, tsmatrix, url_to_idx);

    //写入到bin文件
    writeToBinFile(tsmatrix);
	finish = clock();
    // cout<<"**********************end**********************"<<endl;
	printf("buildgraph runtime: %d\n",(finish - start));


    return 0;
}
