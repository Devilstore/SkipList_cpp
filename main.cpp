#include <string>
#include "skiplist.h"
#include <iostream>

int main()
{

    // 目前 load_file 函数只支持 key:value 字符串 类型进行了处理
    // 如果需要更改 键值对 类型，需要 自定义 key 的比较函数，还有需要 对loadFile 和 storeFile函数进行修改

    // SkipList<int, std::string> skiplist(5);

    SkipList<int, std::string> *skiplist = SkipList<int, std::string>::getInstance();
    skiplist->init();

    std::cout << "skiplist size : " << skiplist->size() << std::endl;
    skiplist->show();

    skiplist->insert(1, "hello");
    skiplist->insert(2, "world");
    skiplist->insert(3, "i");
    skiplist->insert(4, "love");
    skiplist->insert(5, "u");

    std::cout << "skiplist size : " << skiplist->size() << std::endl;
    skiplist->show();

    skiplist->revise(2, "iiiiiiiiiiiiii");

    std::cout << "skiplist size : " << skiplist->size() << std::endl;
    skiplist->show();

    skiplist->storeFile();

    return 0;
}