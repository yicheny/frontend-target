#include <iostream>

using std::cout;
using std::endl;
using std::string;

class Man {
private:
    string name;
    int age;

public:
    explicit Man(const string &name, const int &age) {
        this->name = name;
        this->age = age;
    }

    void eat(const string &food) {
        cout << this->name << " eat " << food << endl;
    }

    void addAge(){
        this->age++;
    }

    //常量成员函数
    //表示这个方法只能读取而不能修改实例的数据成员
    int getAge() const {
        return this->age;
    }

    string getName() const;
};

//可以在类外部定义成员函数【类必须提供接口】
string Man::getName() const{
    return this->name;
}

int main() {
    Man ming = *new Man("mark", 20);
    ming.eat("apple");
    cout << ming.getAge() << endl;
    ming.addAge();
    cout << ming.getAge() << endl;
    cout << ming.getName() << endl;
    return 0;
}