 

vector 是什么？
-----------

> vector收录在STL（C++[标准库](https://edu.csdn.net/cloud/houjie?utm_source=highword&spm=1001.2101.3001.7020)）里，是一种特殊的数据结构，名字叫做“动态数组”或者“不定长数组”，有时也被翻译成“容器”。其实就是一种非静态[数组](https://edu.csdn.net/course/detail/40020?utm_source=glcblog&spm=1001.2101.3001.7020)，可以进行修改、增长。

vector简单用法, 使用vector需要引入#inlcude < vector > 头文件

```c
std::vector<int>test;//建立一个vector
test.push_back(1);
test.push_back(2);//把1和2压入vector这样test[0]就是1,test[1]就是2
/*就像声明定义了一个test数组一样。*/
vector<double>D;
D.push_back(1.9);
/*STL库中有很多容器，比如map还可以嵌套vector ，std::map<int ,std::vector<string>>multiMap; 等等，都很有趣*/
```

C语言没有这样便捷的工具，想要使用动态数组，就要自己自定义实现。  
下面展示的是关于C语言实现一个简易泛型vector。

*   下面是vector.h文件的内容

```c
#include <stdbool.h>        //为了在C语言文件中使用bool变量,C99规定,在本文件中并未用到bool类型
#ifndef __VECTOR_H_
#define __VECTOR_H_

typedef int (*VectorCompareFunction)(const void* elemAddr1,const void* elemAddr2);
typedef void (*VectorCopyFunction)(void* destAddr,const void* srcAddr);
typedef void (*VectorFreeFunction)(void* elemAddr);

typedef struct {
    void* data;                       //指向数据元素的指针
    int elemSize;                     //每个元素占内存的大小
    int allocatedSize;                //已分配内存元素的大小
    int logicalSize;                  //实际储存元素的个数
    VectorFreeFunction freefn;      //设置了一个free函数，根据数据地址释放不定长部分的内存空间
    VectorCopyFunction copyfn;      //提供源数据和目的数据的地址，为目的数据中不定长部分申请内存空间，将源数据中不定长部分拷贝到目的数据中不定长部分
    VectorCompareFunction cmpfn;    //根据参数地址比较两数据的大小
}vector;
void VectorNew(vector* v,int elemSize,VectorFreeFunction freefn,VectorCopyFunction copyfn,VectorCompareFunction cmpfn);
void VectorDispose(vector* v);
void* VectorNth(const vector* v,int position);
void VectorDelete(vector* v,int position);
void Vectorpush_back(vector* v,const void* elemAddr);
int VectorSize(const vector* v);
int VectorSearch(vector* v,const void* key,int startIndex);

#endif
```

vector.h中定义了vector的数据类型，以及基本函数原型声明。和编程范式课程中所教的泛型栈的实现类似，想要得到通用的，功能。我们就要在[数据结构](https://edu.csdn.net/course/detail/40020?utm_source=glcblog&spm=1001.2101.3001.7020)中增添函数，这种函数是用来实现针对不同数据类型的相同功能。

* * *

*   vector.c文件的内容

```c
/* loop--AC/DC
 * loop--AC/DC
 */
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

void VectorNew(vector* v,int elemSize,VectorFreeFunction freefn,VectorCopyFunction copyfn,VectorCompareFunction cmpfn) {
    v->elemSize = elemSize;
    v->allocatedSize = 4;
    v->logicalSize = 0;
    v->data = malloc(v->elemSize * v->allocatedSize);
    assert(v->data!=NULL);
    v->freefn = freefn;
    v->cmpfn = cmpfn;
    v->copyfn = copyfn;
}

void VectorDispose(vector* v) {
    if (v->freefn!=NULL) {
        for (int i=0; i < v->logicalSize; i++) {
            void* elemAddr = (char*)v->data + i*v->elemSize;
            v->freefn(elemAddr);
        }
    }
    free(v->data);     //如果没有析构函数，直接free掉data数据 
}

void* VectorNth(const vector* v,int position) {
    assert(position>=0);
    assert(position < v->logicalSize);
    return (char*)v->data + position * v->elemSize;
}
static void VectorGrow(vector* v) {
    v->data = realloc(v->data,2*v->allocatedSize*v->elemSize);
    assert(v->data!=NULL);
    v->allocatedSize = 2*v->allocatedSize;
}
void Vectorpush_back(vector* v,const void* elemAddr) {
    if(v->logicalSize == v->allocatedSize)
        VectorGrow(v);
    void* destAddr = (char*)v->data + v->logicalSize * v->elemSize;
    /* memcpy(destAddr, elemAddr, v->elemSize); */
	if(v->copyfn != NULL)
		v->copyfn(destAddr, elemAddr);
    memcpy(destAddr,elemAddr,v->elemSize);
    v->logicalSize++;
}

void VectorDelete(vector* v,int position) {
    assert(position>=0);
    assert(position < v->logicalSize);
    void* destAddr = (char*)v->data+position * v->elemSize;
    if (v->freefn!=NULL)
        v->freefn(destAddr);
    int byteSize = (v->logicalSize-1-position) * v->elemSize;
    memmove(destAddr,(char*)destAddr + v->elemSize, byteSize);
    v->logicalSize--;
}

int VectorSize(const vector* v) {
    return v->logicalSize;
}
int VectorSearch(vector* v,const void* key,int startIndex) {
    assert(v->cmpfn!=NULL);
    assert(startIndex >= 0);
    assert(startIndex <= v->logicalSize);
    void* base = (char*)v->data + startIndex * v->elemSize;
    int MAX = v->logicalSize - startIndex;
    int flag = -1;
    for (int i=0 ; i<MAX; i++) {
        void* elemAddr = (char*)base + i*v->elemSize;
        if (0 == v->cmpfn(elemAddr,key)) {
            flag = i+startIndex;
            return flag;
        }
    }
    return flag;
}
```

文件中的函数依次实现头文件中函数的功能。

* * *

*   测试文件

```c
/* loop--AC/DC
 * loop--AC/DC
 */
#include "vector.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*  在main函数中定义自定义的析构函数，比较函数
 *  复制函数
 */
//整型比较函数
int INT_cmp(const void* elemAddr1,const void* elemAddr2) {
    return *(int*)elemAddr1 - *(int*)elemAddr2;
}
//浮点数比较函数,以double为例
int FLOAT_cmp(const void* elemAddr1,const void* elemAddr2) {
    return *(double*)elemAddr1 - *(double*)elemAddr2;
}
//字符型比较函数
int STR_cmp(const void* elemAddr1,const void* elemAddr2) {
    return strcmp(*(char**)elemAddr1,*(char**)elemAddr2);
}
//字符串的析构函数
void STR_free(void* elemAddr) {
    free(*(char**)elemAddr);
}
//字符串的复制函数
void STR_copy(void* destAddr,const void* srcAddr) {
    *(char**)destAddr = strdup(*(char**)srcAddr);
}
int main() {
    //测试，分为三类，整数，浮点数，字符串类型的vector的使用。
    //整数
    vector v1;
    int Array[] = {1,2,3,4,5};
    VectorNew(&v1,sizeof(int),NULL,NULL,INT_cmp);   //不需要特定的free函数和copy函数，所以填NULL
    printf("vector v1 initialized!\n");
    printf("Now push_back to v1\n");
    for (int i=0; i<5; i++) {
        Vectorpush_back(&v1,&Array[i]);
    }
    printf("vector v1 :");
    for (int i=0; i<VectorSize(&v1); i++) {
        printf("%d ", *(int*)VectorNth(&v1,i));
    }
    printf("\n");
    printf("Delete all of v1.\n");
    for (int i=4; i>=0; i--) {
        VectorDelete(&v1,i);
    }
    printf("Now the v1.size() is :%d\n", VectorSize(&v1));
    VectorDispose(&v1);
    //测试浮点数
    printf("Now test double vector\n");
    vector v2;
    double ArrayDouble[] = {1.2,1.3,3.4,5.66,67.8888};
    VectorNew(&v2,sizeof(double),NULL,NULL,FLOAT_cmp);
    printf("vector v2 initialized\n");
    printf("Now push_back to v2\n");
    for (int i=0; i<5; i++) {
        Vectorpush_back(&v2,&ArrayDouble[i]);
    }
    printf("vector v2:");
    for (int i=0; i<VectorSize(&v2); i++) {
        printf("%.8lf ", *(double*)VectorNth(&v2,i));
    }
    printf("\n");
    printf("Delete all of v2.\n");
    for (int i=(sizeof(ArrayDouble)/sizeof(double))-1; i>=0; i--) {
        VectorDelete(&v2,i);
    }
    printf("Now the v2.size() is :%d\n", VectorSize(&v2));
    VectorDispose(&v2);
    //测试字符串
    char* strArray[] = {"hello", "world", "we", "broken", "people", "now"};
    vector vStr;
    VectorNew(&vStr,sizeof(char*), STR_free,STR_copy,STR_cmp);
    printf("vector vStr initialized\n");
    printf("vStr.size() is :%d\n", VectorSize(&vStr));
    for (int i=0; i < sizeof(strArray)/sizeof(char*); i++) {
        Vectorpush_back(&vStr,&strArray[i]);
    }
    printf("vStr:");
    for (int i=0; i<sizeof(strArray)/sizeof(char*); i++) {
        printf("%s ", *(char**)VectorNth(&vStr,i));
    }
    printf("\n");
    char* key = "broken";
    printf("Search 'broken' in vStr: %d\n ", VectorSearch(&vStr,&key,1));
    printf("vStr.size() is :%d\n", VectorSize(&vStr));
    return 0;
}
```

> 运行截图  
> ![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/a55704702b0309d777ccbc1dcf438a72.png)

本文转自 <https://blog.csdn.net/SuperBvs/article/details/92803278>，如有侵权，请联系删除。