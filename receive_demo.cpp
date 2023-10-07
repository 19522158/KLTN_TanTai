#include "lib.hpp"

using namespace std;

int main(){
    int socket_desc;
    int c;
    int i = 1;
    double time_spend = 0.0;
    double sum_time = 0.0;
    clock_t begin,end;
    struct sockaddr_in client;
    socket_init(&socket_desc,&c);
    int new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c);
    close(socket_desc);
    while(new_socket > 0){
        int t =receive_image2("HD.jpg", &new_socket);
        if(new_socket <= 0)
            return 0;
        if (t==2)
            continue;
        i = i + 1;
        cout << "So anh nhan duoc: " << i << endl;
        cout << endl;
    /*  
        begin = clock();
        send_image2(&new_socket);
        end = clock();
        time_spend = (double)(end - begin) /CLOCKS_PER_SEC;
        cout << "Thoi gian gui anh: " << time_spend << endl;
        sum_time = sum_time + time_spend;
        cout << endl;
        cout << endl;
        i = i + 1;
        if (i == 100)
            break;
        sleep(1);*/
    }
 //   cout << "TONG SO ANH SD DA GUI: 100" << endl;
 //   cout << "TRUNG BINH THOI GIAN GUI 100 ANH SD: " << sum_time / 100 << endl;
    close(new_socket);
    return 0;

}