#include <string>
#include <map>

std::vector<std::string> inputSplitter(std::string str, int num_args) {
    std::vector<std::string> args;
    int argnum = 0;
    int strSize = str.length();
    int start = 0;
    for (int i = 0; i<str.length(); i++) {
        if(str[i]==' ') {
            args.push_back(str.substr(start, i-start));
            start = i+1;
            argnum++;
        }
        if(argnum == num_args) {
            args.push_back(str.substr(start, strSize - start +1));
            break;
        }
    }
    return args;
}

std::vector<std::string> inputSplitter_p(std::string str, int num_args) {
    std::vector<std::string> args;
    int argnum = 0;
    int strSize = str.length();
    int start = 0;
    for (int i = 0; i<str.length(); i++) {
        if(str[i]==' ') {
            args.push_back(str.substr(start, i-start));
            start = i+1;
            argnum++;
        }
        if(argnum == num_args) {
            args.push_back(str.substr(start, strSize - start +1));
            break;
        }
    }
    return args;
}

