#include <string>
#include <iostream>
#include <experimental/filesystem>
#include <string.h>


using namespace std::experimental::filesystem;
using namespace std;


int main()
{
    std::string path = "/home/newftpuser";
    for (auto & entry : directory_iterator(path)){
        std::string substr = entry.path().string().substr(path.size()+1, entry.path().string().size()-4);
        int pos = substr.find(".");
        substr = substr.substr(0, pos);
        std::cout << substr << std::endl;
        break;
    }
        
}