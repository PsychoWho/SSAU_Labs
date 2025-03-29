#include<iostream>
#include<cmath>
#include<string>

using namespace std;

void uncorrect() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "I don't understand you, sorry. Please, try again.\n";
}

bool is_not_sev(long long num){
    string str = to_string(num);
    for(int i = 0; i < str.length(); i++){
        if(str[i] == '7' || str[i] == '8' || str[i] == '9'){
            uncorrect();
            return true;
        }
    }
    return false;
}

void  start_msg(){
    cout << "Hello! \n" << "This program converts a number from septenary to decimal." << endl;
}

long long algorithm(long long num){
    long long dec = 0;
    int i = 0;
    while(num){
        dec += (num % 10) * pow(7,i);
        num = num/10;
        i++;
    }
    return dec;
}

void Program(){
     long long sevNum;
     do {
         cout << "Please, enter a non-negative number > ";
         cin >> sevNum;
                 while(cin.fail() || sevNum < 0 || cin.peek() != '\n'){
             uncorrect();
             cout << "Please, enter a non-negative number > ";
             cin >> sevNum;
         }
     }while(is_not_sev(sevNum));

     cout << algorithm(sevNum) << endl;
}

bool is_continue(){
    cout << "Do you want to continue? (y/n) > " ;
    char ans;
    cin >> ans;
    ans = tolower(ans);
    while (ans != 'y' && ans != 'n' || cin.peek() != '\n') {
        uncorrect();
        cout << "Do you want to continue? (y/n) > ";
        cin >> ans;
    }
    if(ans == 'y') return true;
    else return false;

}

int main() {
     start_msg();
     do {
         Program();
     }while(is_continue());
    cout << "Thanks for using this program.\n" << "Goodbye!";
}