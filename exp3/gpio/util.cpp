#include <unordered_map>

using namespace std;



static std::unordered_map<int,int> key_led_table;


extern "C"{
    void bind_key_led(int keyfd, int ledfd){
        key_led_table[keyfd] = ledfd;
    }
    int key_to_led(int keyfd){
        if(key_led_table.count(keyfd)!=0){
            return key_led_table[keyfd];
        }
        return -1;
    }
}


