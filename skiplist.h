/*

使用C++开发，基于跳表实现的轻量级键值型数据库

使用 SkipList::getInstance() 获取 跳表单例
基于 此进行操作即可。

*/
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <fstream>
#include <random>
#include <string>
#include <mutex>

#define MAX_LEVEL 32
#define S_FILE "./store/dataFile"

#define P_FACTOR 0.4

// 用于生成 随机层数
std::mt19937 gen(std::random_device{}());
std::uniform_real_distribution<double> dis(0, 1);

// key:value 分隔符定义
std::string spliter = ":";

// 跳表 节点类 实现
template <typename K, typename V>
class SkipListNode
{
public:
    SkipListNode(){};                                     // 无参构造
    SkipListNode(const K _key, const V _val, int _level); // 有参构造
    ~SkipListNode();                                      // 析构函数
    K getKey() const;                                     // 获取节点健
    V getValue() const;                                   // 获取节点值
    void setValue(V);                                     // 设置 节点 值
    std::vector<SkipListNode<K, V> *> forward;            // 跳表 存储等所有层指针

private:
    K m_key;
    V m_value;
    int m_level; // 节点拥有的 forward 层数
};

template <typename K, typename V>
SkipListNode<K, V>::SkipListNode(const K _key, const V _val, int _level) : m_key(_key), m_value(_val), m_level(_level), forward(_level)
{
    // printf("调用 node 有参构造...\n");
}

template <typename K, typename V>
SkipListNode<K, V>::~SkipListNode() {}

template <typename K, typename V>
K SkipListNode<K, V>::getKey() const
{
    return m_key;
}

template <typename K, typename V>
V SkipListNode<K, V>::getValue() const
{
    return m_value;
}

template <typename K, typename V>
void SkipListNode<K, V>::setValue(V _val)
{
    this->m_value = _val;
}

// 跳表 skiplist 实现
template <typename K, typename V>
class SkipList
{
public:
    // C++11 局部静态变量 懒汉模式（单例模式）
    static SkipList<K, V> *getInstance()
    {
        std::cout << "单例模式已启动...." << std::endl;
        static SkipList<K, V> instance;
        return &instance;
    }

private:
    SkipList(int max_level = MAX_LEVEL); // 构造函数
    ~SkipList();                         // 析构函数

public:
    int insert(K, V); // 按 key 有序添加元素  0：插入成功。 1：元素已存在
    void show();      // 显示当前 跳表
    bool find(K);     // 查找 key
    bool erase(K);    // 删除元素
    bool storeFile(); // 存储 data 到文件
    bool loadFile();  // 加载 data 到内存
    int size();       // 获取当前 数据 条数
    void init();      // 初始化函数： 加载 数据

private:
    bool getInfo(const std::string &str, std::string *key, std::string *value); // 从 str 中 提取 key-value
    bool isValidString(const std::string &str);                                 // 判断 字符串 str 是否符合 key:value 标准
    SkipListNode<K, V> *createNode(K, V, int);                                  // 创建一个节点
    int getRandomLevel();                                                       // 生成随机层数，最大为 m_max_level

private:
    SkipListNode<K, V> *m_head;  // 跳表头结点
    SkipListNode<K, V> *m_tail;  // 跳表尾结点
    int m_max_level;             // 跳表内所有节点的 最大层数限制
    int m_level;                 // 跳表内所有节点的 当前最大层数，不包含头尾节点
    int m_size;                  // 跳表内有效节点数量
    std::ofstream m_file_writer; // 写文件 输出流
    std::ifstream m_file_reader; // 读文件 输入流
    std::mutex m_mutex;          // 互斥锁
};

template <typename K, typename V>
SkipList<K, V>::SkipList(int max_level) : m_size(0),
                                          m_max_level(max_level),
                                          m_level(0)
{
    K _k;
    V _v;
    m_head = new SkipListNode<K, V>(_k, _v, max_level);
    m_tail = new SkipListNode<K, V>(_k, _v, max_level);
    for (int i = 0; i < max_level; ++i)
    {
        m_head->forward[i] = m_tail;
    }
}

template <typename K, typename V>
SkipList<K, V>::~SkipList()
{
    storeFile(); // 析构前 进行存储
    if (m_file_reader.is_open())
    {
        m_file_reader.close();
    }
    if (m_file_writer.is_open())
    {
        m_file_writer.close();
    }
    if (m_head)
    {
        delete m_head;
    }
    if (m_tail)
    {
        delete m_tail;
    }
}

template <typename K, typename V>
int SkipList<K, V>::insert(K _key, V _val)
{
    m_mutex.lock();
    SkipListNode<K, V> *cur = m_head;
    std::vector<SkipListNode<K, V> *> update(m_max_level, m_head); // 创建 update 数组 用于插入之后 更新链表指针

    // 从当前最高层开始扫描 找到大于等于 待插入元素的 位置
    for (int i = m_level - 1; i >= 0; --i)
    {
        while (cur->forward[i] != m_tail && cur->forward[i]->getKey() < _key)
        {
            cur = cur->forward[i];
        }
        update[i] = cur; // 保存待插入节点位置的 i 层前驱
    }

    cur = cur->forward[0]; // 待插入节点位置现有元素

    // 如果当前节点为有效节点，并且 key 等于待插入 key， 说明节点已存在。返回 1
    if (cur != m_tail && cur->getKey() == _key)
    {
        // std::cout << "Failed insert, key:" << _key << ", exists" << std::endl;
        m_mutex.unlock();
        return 1;
    }

    // 待插入位置节点为 m_tail 或者 待插入节点位置 值不为 _key, 则在当前位置插入节点即可。
    int random_level = getRandomLevel(); // 生成随机层数

    m_level = random_level > m_level ? random_level : m_level; // 更新跳表当前最大层数

    // 为待插入数据创建新结点
    SkipListNode<K, V> *node = createNode(_key, _val, random_level);

    // 正式插入节点。 即链接所有指针
    for (int i = 0; i < random_level; ++i)
    {
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }

    // std::cout << "Successfully inserted key:" << _key << ",value:" << _val << std::endl;
    ++m_size;

    m_mutex.unlock();
    return 0;
}

// 输出跳表所有元素
template <typename K, typename V>
void SkipList<K, V>::show()
{
    std::cout << "\n**********SkipList**********\n"
              << std::endl;
    for (int i = 0; i < m_level; ++i)
    {
        SkipListNode<K, V> *cur = m_head->forward[i]; // i层第一个有效节点
        if (cur == m_tail)
        {
            continue; // 最高层没有节点 跳过
        }
        std::cout << "Level - " << i << ":  ";
        while (cur != m_tail)
        {
            std::cout << cur->getKey() << ":" << cur->getValue() << ";  ";
            cur = cur->forward[i];
        }
        std::cout << std::endl;
    }
    std::cout << "\n************over************\n"
              << std::endl;
}

// 查找 key
template <typename K, typename V>
bool SkipList<K, V>::find(K _key)
{
    SkipListNode<K, V> *cur = m_head;

    // std::cout << "find key:" << _key << "   >>>>>>   ";

    for (int i = m_level - 1; i >= 0; --i)
    {
        while (cur->forward[i] != m_tail && cur->forward[i]->getKey() < _key)
        {
            cur = cur->forward[i];
        }
    }

    cur = cur->forward[0]; //找到的节点位置

    // 如果找到的节点 为 尾结点， 或者 key 不等于 _key，说明不存在该节点
    if (cur == m_tail || cur->getKey() != _key)
    {
        // std::cout << "Not found key:" << _key << std::endl;
        return false; // 未找到节点
    }
    // std::cout << "Successfully found key:" << _key << ", value:" << cur->getValue() << std::endl;
    return true; // 找到节点
}

// 删除 key
template <typename K, typename V>
bool SkipList<K, V>::erase(K _key)
{
    m_mutex.lock();
    SkipListNode<K, V> *cur = m_head;
    std::vector<SkipListNode<K, V> *> update(m_max_level, m_head); // 创建 update 数组 用于插入之后 更新链表指针

    for (int i = m_level - 1; i >= 0; --i)
    {
        while (cur->forward[i] != m_tail && cur->forward[i]->getKey() < _key)
        {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }

    cur = cur->forward[0]; //待删除节点位置

    // 如果待删除节点位置 为 尾结点， 或者 key 不等于 _key，说明不存在待删除节点
    if (cur == m_tail || cur->getKey() != _key)
    {
        m_mutex.unlock();
        // std::cout << "key:" << _key << ", not exits." << std::endl;
        return false; // 删除失败 ，未找到节点
    }

    // 正式删除节点。 更新链接
    for (int i = 0; i < m_level; ++i)
    {
        // 直到当前层 不存在 节点cur 说明 已经更新完毕。更高的层 也不会存在 节点 cur
        if (update[i]->forward[i] != cur)
        {
            break;
        }
        update[i]->forward[i] = cur->forward[i];
    }

    delete cur; // 释放内存

    cur = m_head;

    // 每次删除之后，判断最高层 是否还存在节点，若不存在，最高层减少
    while (m_level > 0 && cur->forward[m_level - 1] == m_tail)
    {
        --m_level;
    }
    // std::cout << "Successfully erase key:" << _key << std::endl;
    --m_size;
    m_mutex.unlock();
    return true; // 成功删除
}

// 保存 跳表到文件
template <typename K, typename V>
bool SkipList<K, V>::storeFile()
{
    std::cout << "storeFile\t>>>>>>" << std::endl;
    try
    {
        m_file_writer.open(S_FILE);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        std::cout << "try" << std::endl;
        return false;
    }

    SkipListNode<K, V> *cur = m_head->forward[0]; // 第一个有效节点
    while (cur != m_tail)
    {
        m_file_writer << cur->getKey() << ":" << cur->getValue() << "\n";
        // std::cout << cur->getKey() << ":" << cur->getValue() << "\n";
        cur = cur->forward[0];
    }

    m_file_writer.flush();
    m_file_writer.close();
    return true;
}

// 加载 跳表到内存
template <typename K, typename V>
bool SkipList<K, V>::loadFile()
{
    try
    {
        m_file_reader.open(S_FILE);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        std::cout << "try" << std::endl;
        return false;
    }

    std::cout << "loadFile\t>>>>>>" << std::endl;
    std::string str;
    std::string *key = new std::string();
    std::string *value = new std::string();

    while (getline(m_file_reader, str))
    {
        if (getInfo(str, key, value))
        {
            if (key->empty() || value->empty())
            {
                continue;
            }
            insert(std::stoi(*key), V(*value));
            // std::cout << "key:" << *key << ", value:" << *value << std::endl;
        }
        else
        {
            m_file_reader.close();
            return false;
        }
    }
    std::cout << "Loading Data lines : " << m_size << std::endl;
    m_file_reader.close();
    return true;
}

// 获取当前跳表 有效节点个数
template <typename K, typename V>
int SkipList<K, V>::size()
{
    return m_size;
}

template <typename K, typename V>
void SkipList<K, V>::init()
{
    std::cout << "SkipList is starting ..." << std::endl;
    std::cout << "loading data ..." << std::endl;

    if (loadFile())
    {
        std::cout << "loading over ..." << std::endl;
    }
    else
    {
        std::cout << "loading fail ..." << std::endl;
        exit(-1);
    }
}

// 从 str 中 提取 key-value
template <typename K, typename V>
bool SkipList<K, V>::getInfo(const std::string &str, std::string *key, std::string *value)
{
    if (!isValidString(str))
    {
        return false;
    }
    *key = str.substr(0, str.find(spliter));
    *value = str.substr(str.find(spliter) + 1);
    return true;
}

// 判断 字符串 str 是否符合 key:value 标准
template <typename K, typename V>
bool SkipList<K, V>::isValidString(const std::string &str)
{
    return !(str.empty() || str.find(spliter) == std::string::npos); // str为空 或者未找到 spliter 则返回假 否则为真
}

// 创建新跳表节点
template <typename K, typename V>
SkipListNode<K, V> *SkipList<K, V>::createNode(const K _key, const V _val, int level)
{
    // printf("createNode\n");
    SkipListNode<K, V> *node = new SkipListNode<K, V>(_key, _val, level);
    return node;
}

template <typename K, typename V>
int SkipList<K, V>::getRandomLevel()
{
    int lv = 1;
    while (lv < m_max_level && dis(gen) < P_FACTOR)
    {
        ++lv;
    }
    // printf("生成随机层数:%d\n", lv);
    return lv;
}

// template <typename K, typename V>
// SkipList<K, V> *SkipList<K, V>::getInstance()
// {
//     static SkipList<K, V> instance;
//     return &instance;
// }