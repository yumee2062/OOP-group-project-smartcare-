#include<iostream>
#include<fstream>
using namespace std;
int main(){
        fstream nfile;
        nfile.open("record.txt",ios::out);
        if(!nfile){
            cout << "Error while opening file" << endl;

        }
        else{
            nfile <<"Hello world\nThank you\n" << endl;
            nfile.close();
        }
        return 0;
}
