#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <cmath>

using namespace std;

const int ATTRIBUTES_AMOUNT = 6;

class Sample {
    vector<string> attributes;
    string sClass;
public:
    void attr_push_back(string attribute) {
        attributes.push_back(attribute);
    }
    void setClass(string sClass) {
        this->sClass = sClass;
    }
    vector<string> & getAttributes() {
        return attributes;
    }
    string getAttribute(size_t i) {
        return attributes[i];
    }
    string getClass() {
        return sClass;
    }
    void print() {
        for (int i = 0; i < attributes.size(); ++i) {
            cout << attributes[i] << '\t';
        }
        cout << sClass << endl;
    }
};

void printData(vector<Sample> & data) {
    for (int i = 0; i < data.size(); ++i) {
        data[i].print();
    }
}
/*
float infGain(std::vector<) {

}
*/
int main()
{
    ifstream file;
    file.open("car.data");
    char c;

    vector<Sample> data;

    while (!file.eof()) {
        Sample sample;
        string sClass = "";;
        string attribute = "";
        for (int i = 0; i < ATTRIBUTES_AMOUNT; ++i) {
            file >> c;
            while (c != ',' && !file.eof()) {
                attribute += c;
                file >> c;
            }
            sample.attr_push_back(attribute);
            attribute = "";
        }
        file >> sClass;
        sample.setClass(sClass);
        data.push_back(sample);
    }

    //printData(data);

    std::unordered_map<std::string, size_t> clazzes;
    std::string maxClazz;

    std::array<std::unordered_map<std::string, std::unordered_map<std::string, size_t>>, ATTRIBUTES_AMOUNT> count;
    for (auto it = data.begin(); it != data.end(); ++it) {
      for (size_t i = 0; i < ATTRIBUTES_AMOUNT; ++i) {
        count[i][it->getAttribute(i)][it->getClass()]++;
      }
      clazzes[it->getClass()]++;
    }

    // Compute the entropy of the current set.
    auto entropy = 0.0f;
    float total = data.size();
    //auto total = end - begin;
    for (auto clazz : clazzes) {
      auto p = clazz.second / (float)total;
      entropy -= p * log(p) / log(2.0f);

      if (!maxClazz.empty() && clazz.second > clazzes[maxClazz]) {
        maxClazz = clazz.first;
      }
    }
    cout << entropy << endl;



    return 0;
}
