#include "ReadFiles.h"

//归一化
void graphNodeNormalizing(TSMatrix& tsmatrix);

//矩阵相加
void addMatrix(float* left, float* right, float* vk_1);

//矩阵乘法Gm
void mulMatrixLeft(TSMatrix& gm, float* vk, float* left, float alpha);

//矩阵乘法N维矩阵
void mulMatrixRight(float* vk, float* right, float alpha);

//两个向量的差
float deltaOfArray(float* vk, float* vk_1);

//读入文件到矩阵
void read_struct(TSMatrix& tsmatrix)