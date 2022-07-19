# ACM输入输出处理

##   1.最简单的输入输出

```markdown
Description
计算 a+b

Input
两个整数 a,b

Output
a+b的值

Sample Input
1 2

Sample Output
3
```

```c++
#include <iostream>
using namespace std;
int main(){
    int a, b;
    cin >> a >> b;
    cout << a + b << endl;
    return 0;
}
```

## 2.一次运行需要多数数据

直到读至输入文件末尾(EOF)为止

```markdown
Description
计算 a + b

Input
多组由两个整数（a和b）构成的输入，a和b之间用空格隔开，每组输入单独占一行 

Output
每组的两个整数（a和b）求和并输出，每组的求和结果独占一行

Sample Input
1 5
10 20
400 516

Sample Output
6
30
916
```

```c++
#include <iostream>
using namespace std;
int main(){
    int a, b;
    while(cin >> a >> b){
        cout << a + b << endl;
    }
}
```

##  3.一次运行，要输入多组数据(提示组数)

组数由第一个输入数据决定，（在开始的时候输入一个N，接下来是N组数据）

```markdown
Description
计算 a + b

Input
第一行是数据的组数N，从第二行是N组由两个整数（a和b）构成的输入，a和b之间用空格隔开，每组输入单独占一行

Output
每组的两个整数（a和b）求和并输出，每组的求和结果独占一行

Sample Input
2
1 5
10 20

Sample Output
6
30
```



```c++
#include <iostream>

using namespace std;

int main(){
    int a, b, n;
    cin >> n;
    while(n--){
        cin >> a >> b;
        cout << a + b << n;
    }
    return 0;
}

```

## 4.  输入不说明有多少组数据，但以某个特殊输入为结束标志。

平时做题中常见诸如“输入学生成绩，以 -1 结束”，没有学生得 -1 分的，这个结束数据可以要得。

```markdown
Description
计算 a + b

Input
多组由两个整数（a和b）构成的输入，a和b之间用空格隔开，每组输入单独占一行。当输入为 0 0 时，输入结束。

Output
每组的两个整数（a和b）求和并输出，每组的求和结果独占一行。

Sample Input
1 5
10 20
0 0

Sample Output
6
30

```

```c++
#include <iostream>
using namespace std;
int main(){
    int a, b;
    while(cin >> a >> b && (a || b)){
        cout << a + b << endl;
    }
    return 0;
}
```

## 5. 利用文件重定向提高调试效率

用下面的形式调用函数 freopen() 会将标准输入 stdin 重定向到文件 input.txt （这个名字可以自己定义）。

```c++
#include <iostream>
#include <cstdio>

using namespace std;
int main(){
    // 文件重定向 读取文件内容当做键盘输入。仅用于调试
	freopen("input.txt", 'r', stdin);	// 提交的时候记得注释掉这一行
	int a, b;
	cin >> a >> b;
	cout << a + b << endl;
	return 0;
}
```

## 6. C++ 的一些输入输出函数

- cin.get(char * str, int count);   与getiline()类似，但是不处理换行符，下一字符仍为换行符
    - 可以采用 cin.get() 来获取一个字符，跳过换行符，再继续进行获取。
- **cin.getline**(char * str, int count);   **接收一个字符串，可以接收空格并输出**
- **getline**(cin, str);  **接收一个字符串，可以接收空格并输出，需包含“#include<string**>”



- cin >> int ,char ,string, **输入一个数字或字符,接收一个字符串，遇“空格”、“TAB”、“回车”就结束**



## ACM模式参考程序

```c++
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<algorithm>
#include<limits.h>  //INT_MIN 和 INT_MAX的头文件  

using namespace std;

struct stu {
	string name;
	int num;
};

// 1. 直接输入一个数
int main() {
	int n = 0;
	while (cin >> n) { 
		cout << n << endl;
	}
	return -1;
}

// 2. 直接输入一个字符串
int main() {
	string str;
	while (cin >> str) {
		cout << str << endl;
	}
	return -1;
}

// 3. 只读取一个字符 
int main() {
	char ch;
	//方式1
	while (cin >> ch) {
		cout << ch << endl;
	}
	//方式2： cin.get(ch) 或者 ch = cin.get() 或者 cin.get()
	while (cin.get(ch)) {   
		cout << ch << endl;
	}
	//方式3 ：ch = getchar()  
	while (getchar()) {
		cout << ch << endl;
	}
	return -1;
}


// 3.1给定一个数，表示有多少组数（可能是数字和字符串的组合），然后读取
int main() {
	int n = 0; 
	while (cin >> n) {   //每次读取1 + n 个数，即一个样例有n+1个数 
		vector<int> nums(n);
		for (int i = 0; i < n; i++) {
			cin >> nums[i];
		}
		//处理这组数/字符串
		for (int i = 0; i < n; i++) {
			cout << nums[i] << endl;
		}
	}
	return -1;
}

//3.2 首先给一个数字，表示需读取n个字符，然后顺序读取n个字符
int main() {
	int n = 0;
	while (cin >> n) {  //输入数量
		vector<string> strs;
		for (int i = 0; i < n; i++) {
			string temp;
			cin >> temp;
			strs.push_back(temp);
		}
		//处理这组字符串
		sort(strs.begin(), strs.end());
		for (auto& str : strs) {
			cout << str << ' ';
		}
	}
	return 0;
}


//4.未给定数据个数，但是每一行代表一组数据，每个数据之间用空格隔开
//4.1使用getchar() 或者 cin.get() 读取判断是否是换行符，若是的话，则表示该组数（样例）结束了，需进行处理
int main() {
	int ele;
	while (cin >> ele) {
		int sum = ele;
		// getchar()   //读取单个字符
		/*while (cin.get() != '\n') {*/   //判断换行符号
		while (getchar() != '\n') {  //如果不是换行符号的话，读到的是数字后面的空格或者table
			int num;
			cin >> num;
			sum += num;
		}
		cout << sum << endl;
	}
	return 0;
}

//4.2 给定一行字符串，每个字符串用空格间隔，一个样例为一行
int main() {
	string str;
	vector<string> strs;
	while (cin >> str) {
		strs.push_back(str);
		if (getchar() == '\n') {  //控制测试样例
			sort(strs.begin(), strs.end());
			for (auto& str : strs) {
				cout << str << " ";
			}
			cout << endl;
			strs.clear();
		}
	}
	return 0;
}


//4.3 使用getline 读取一整行数字到字符串input中，然后使用字符串流stringstream，读取单个数字或者字符。
int main() {
	string input;
	while (getline(cin, input)) {  //读取一行
		stringstream data(input);  //使用字符串流
		int num = 0, sum = 0;
		while (data >> num) {
			sum += num;
		}
		cout << sum << endl;
	}
	return 0;
}


//4.4 使用 getline 读取一整行字符串到字符串input中，然后使用字符串流stringstream，读取单个数字或者字符。
int main() {
	string words;
	while (getline(cin, words)) {
		stringstream data(words);
		vector<string> strs;
		string str;
		while (data >> str) {
			strs.push_back(str);
		}
		sort(strs.begin(), strs.end());
		for (auto& str : strs) {
			cout << str << " ";
		}
	}
}

//4.5 使用getline 读取一整行字符串到字符串input中，然后使用字符串流stringstream，读取单个数字或者字符。每个字符中间用','间隔
int main() {
	string line;
	
	//while (cin >> line) {  //因为加了“，”所以可以看出一个字符串读取
	while(getline(cin, line)){
		vector<string> strs;
		stringstream ss(line);
		string str;
		while (getline(ss, str, ',')) {
			strs.push_back(str);
		}
		//
		sort(strs.begin(), strs.end());
		for (auto& str : strs) {
			cout << str << " ";
		}
		cout << endl;
	}
	return 0;
}



int main() {
	string str;

	
	//C语言读取字符、数字
	int a;
	char c;
	string s;

	scanf_s("%d", &a);
	scanf("%c", &c);
	scanf("%s", &s);
	printf("%d", a);


	//读取字符
	char ch;
	cin >> ch;
	ch = getchar();
	while (cin.get(ch)) { //获得单个字符
		;
	}
	
	//读取字符串
	cin >> str;  //遇到空白停止
	getline(cin, str);  //读入一行字符串

}
```



