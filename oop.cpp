#include <iostream>

using namespace std;

// This is an abstract class
class AbstractEmployee {
  // whichever class extends it, it's obligatory to implement askForPermission (that is ensured by `virtual`) & so now is an abstract method
  virtual void askForPermission() = 0;
};

// So, here Student inherit from the AbstractEmployee
class Student:AbstractEmployee {
  // N.B: by default attributes are private
  // string name;
  // int age;

  // This is just being explicit , this does same as above
  // private:
  //         string name;
  //         int age;

  private:
        string address;
        int rollNo;
        string dept;

  protected:
        string name;

  public:
        Employee(string name, string address, int rollNo, intn deptNo) {
          name = name;
          address = address;
          rollNo = rollNo;
          dept = deptNo;
        }

        void askForPermission() {
          if (age > 30) {
            cout << "getting promoted" << endl;
          } else {
            cout << "negotiating" << endl;
          }
        }

        void introduceYourself() {
          cout << "Hello, " << "My name is " << name << endl;
        }

        // setter
        void setName(string name) {
          name = name;
        }

        // getter
        string getName() {
          return name;
        }
};

class Developer:Employee {
  public:
       string favoriteProgrammingLang;

       Developer(string name, string address, int rollNo, intn deptNo, string favoriteProgrammingLang) {
          :Employee(name, address, rollNo, deptNo);
          favoriteProgrammingLang = favoriteProgrammingLang;
       }

       void fixBug () {
        cout << name << " Fixed the bug using " << favoriteProgrammingLang << endl;
       }

}

int main (int argc, char *argv[]) {
  Employee employee1 = Employee("John", "Boston", 30, "Wrestling");
  Employee employee2 = Employee("Jose", "Madrid", 20, "Football");

  employee1.askForPermission();
  employee2.introduceYourself();

  Developer developer = new Developer('Johnson', 'UK', 40, 'Engineering', 'Java')
  developer.fixBug();
};
