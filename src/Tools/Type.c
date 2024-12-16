#include"Tools/Type.h"

int _default_cmp(const void *a,const void *b,size_t itemSize){
    _default_cmp_if_for(int)
    else _default_cmp_if_for(float)
    else _default_cmp_if_for(double)
    else _default_cmp_if_for(long long)
    else _default_cmp_if_for(long double)
    else _default_cmp_if_for(char)
    else{
        assert(0&&"_default_cmp 传入了不支持的比较类型");
        return 0;
    }
}