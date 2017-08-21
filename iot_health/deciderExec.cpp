/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <string>
#include <sstream>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

std::map<std::string, int> critical_vitals = {{"temperature", 38}, {"heartrate", 50}};

struct StringArray {
    char **array;
};

int vitals_danger(std::string sensor_type, int reading) {
    if(sensor_type=="temperature" && reading>=critical_vitals["temperature"]) {
        return 1;
    } else if(sensor_type=="heartrate" && reading<=critical_vitals["heartrate"]) {
        return 1;
    }
    return 0;
}

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

void decider_enclave_execute(std::string smessage, int n, void *sender, StringArray** retmessage, int ** retlen, int * nc, int *dangerous) {
    unsigned int j = 0;
    int count = 1;

    *nc = count;
    *retmessage = (StringArray *) malloc(sizeof(StringArray));
    (*retmessage)->array = (char**) malloc(count * sizeof (char *));

    *retlen = (int *)malloc(count * sizeof (int));

    std::vector<std::string> args = inputSplitter(smessage, 5);
    std::string sensor_type = args[4];
    int reading = std::stoi(args[5]);

    //*dangerous = 1;
    *dangerous = vitals_danger(sensor_type, reading);
    std::string new_message = smessage.substr(smessage.find(" ") + 1);

    int i = 0;
    while (i<1) {
        std::hash<std::string> hasher;
        auto hashed = hasher(new_message);
        j = hashed % n;

        *((*retlen)+i) = new_message.length() + std::to_string(j).length() + 2;
        (*retmessage)->array[i] = (char *) malloc(*((*retlen)+i) * sizeof(char));
        snprintf((*retmessage)->array[i], *((*retlen)+i), "%d %s", j, new_message.c_str());

        i = i+1;
    }
}
