#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <cmath>
#include <time.h>

using namespace std;

const int ATTRIBUTES_AMOUNT = 6;
const int K = 5;

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
    vector<string>& getAttributes() {
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
    void deleteAttribute(int attrId) {
        attributes.erase(attributes.begin() + attrId);
    }
};

class Node {
    int attribute = -1;
    string sClass;
    vector<string> attributes;
    vector<Node*> decisions;
public:
    void set(int attribute, vector<string> & attr) {
        this->attribute = attribute;
        for (int i = 0; i < attr.size(); ++i) {
            Node* newNode = new Node;
            attributes.push_back(attr[i]);
            this->decisions.push_back(newNode);
        }
    }
    void setTerminalNode(string sClass) {
        this->attribute = -1;
        this->sClass = sClass;
    }
    void deleteNodes() {
        for (int i = 0; i < decisions.size(); ++i) {
            decisions[i]->deleteNodes();
            delete decisions[i];
        }
    }
    Node* getNext(int index) {
        return decisions[index];
    }
    void print(int level = 0) {
        for (int i = 0; i < level; ++i) {
            cout << "  ";
        }
        if (attribute == -1) {
            cout << sClass << endl;
        }
        else {
            cout << attribute << endl;
            for (int i = 0; i < decisions.size(); ++i) {
                decisions[i]->print(level + 1);
            }
        }
    }
    string classify(Sample & sample) {
        if (attribute == -1) {
            return sClass;
        }
        else {
            string attr = sample.getAttribute(attribute);
            for (int i = 0; i < attributes.size(); ++i) {
                if (attributes[i] == attr) {
                    sample.deleteAttribute(attribute);
                    return decisions[i]->classify(sample);
                }
            }
        }
        return "zle";
    }
};

class Tree {
    Node* root;
public:
    Tree() {
        root = new Node();
    }
    ~Tree() {
        root->deleteNodes();
        delete root;
    }
    Node* getRoot() {
        return root;
    }
    void print() {
        root->print();
    }
    string classify(Sample & sample) {
        return root->classify(sample);
    }
};



void printData(vector<Sample>& data) {
    for (int i = 0; i < data.size(); ++i) {
        data[i].print();
    }
}

class ID3 {
    Tree tree;
    Node* node;
    vector<Sample> data;
public:
    ID3(vector<Sample> & data) {
        node = tree.getRoot();
        this->data = data;
        train(data, node);
    }
    void print() {
        tree.print();
    }
    string classify(Sample s) {
        return tree.classify(s);
    }
private:
    void train(vector<Sample> data, Node* node, int attributesAmount = ATTRIBUTES_AMOUNT) {
        // warunek koncowy - jesli nie ma wiecej atrybutow to utworz wezel terminalny z najczestsza klasa
        if (attributesAmount == 0) {
            std::unordered_map<std::string, size_t> classesCount;
            for (auto it = data.begin(); it != data.end(); ++it) {
                classesCount[it->getClass()]++;
            }

            std::string maxClass;
            int maximum = 0;
            for (auto sClass : classesCount) {
                if (sClass.second > maximum) {
                    maximum = sClass.second;
                    maxClass = sClass.first;
                }
            }
            node->setTerminalNode(maxClass);
            return;
        }

        // warunek koncowy - jesli wszystkie klasy w podzbiorze takie same, to utworz wezel terminalny z ta klasa
        bool flag = true;
        auto claz = data[0].getClass();
        for (int i = 0; i < data.size() && flag; ++i) {
            if (claz != data[i].getClass())
                flag = false;
        }
        if (flag) {
            node->setTerminalNode(claz);
            return;
        }


        std::unordered_map<std::string, size_t> classesCount;
        std::string maxClazz;

        std::array<std::unordered_map<std::string, std::unordered_map<std::string, size_t>>, ATTRIBUTES_AMOUNT> attributesCount;
        for (auto it = data.begin(); it != data.end(); ++it) {
            for (size_t i = 0; i < attributesAmount; ++i) {
                attributesCount[i][it->getAttribute(i)][it->getClass()]++;
            }
            classesCount[it->getClass()]++;
        }

        float entropy = 0.0f;
        for (auto clazz : classesCount) {
            float p = clazz.second / (float)data.size();;
            entropy -= p * log(p) / log(2.0f);

            if (!maxClazz.empty() && clazz.second > classesCount[maxClazz]) {
                maxClazz = clazz.first;
            }
        }

        int maxAttribute = -1;
        float maxGain = 0.0f;
        for (int i = 0; i < attributesAmount; ++i) {
            float gain = entropy;
            std::unordered_map<std::string, std::unordered_map<std::string, size_t>> attributeCount = attributesCount[i];
            int classesAmount = 0;

            for (auto attr : attributeCount) {
                // ilosc klas po wybraniu atrybutu
                int classesAmount = 0;
                for (auto sClass : attr.second) {
                    classesAmount += sClass.second;
                }

                // obliczenie entropii wybranego atrybutu
                float attributeEntropy = 0.0f;
                for (auto sClass : attr.second) {
                    float p = (float)sClass.second / classesAmount;
                    attributeEntropy -= p * log(p) / log(2.0f);
                }
                gain -= attributeEntropy * classesAmount / data.size();
            }
            if (gain > maxGain) {
                maxAttribute = i;
                maxGain = gain;
            }
        }
        vector<string> dec;
        for (auto attr : attributesCount[maxAttribute]) {
            dec.push_back(attr.first);
        }
        node->set(maxAttribute, dec);

        int i = 0;
        for (auto attr : attributesCount[maxAttribute]) {
            train(divideDataByAttribute(data, maxAttribute, attr.first), node->getNext(i), attributesAmount-1);
            ++i;
        }

    }
    vector<Sample> divideDataByAttribute(vector<Sample> & data ,int attrId, string attr) {
        vector<Sample> result;
        for (int i = 0; i < data.size(); ++i) {
            Sample temp = data[i];
            if (data[i].getAttribute(attrId) == attr) {
                temp.deleteAttribute(attrId);
                result.push_back(temp);
            }
        }
        return result;
    }
};

void shuffleData(vector<Sample> & data) {
    vector<Sample> result;
    while (!data.empty()) {
        int i = rand() % data.size();
        result.push_back(data[i]);
        data.erase(data.begin() + i , data.begin() + i +1);
    }
    data = result;
}

int main()
{
    srand(time(NULL));
    ifstream file;
    file.open("car.data");
    if (!file.good()) {
        return -1;
    }
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
    shuffleData(data);

    cout << "\tk-krotna walidacja krzyzowa\n\n";
    vector<Sample> dataSubsets[K];
    float avgError;
    for (int i = 0; i < K; ++i) {
        copy(data.begin() + i*data.size()/K, data.begin() + (i+1)*data.size()/K, back_inserter(dataSubsets[i]));
    }
    for (int j = 0; j < K; ++j) {
        vector<Sample> trainingSet;
        vector<Sample> testingSet;
        for (int i = 0; i < K; ++i) {
            if (j != i)
                trainingSet.insert(trainingSet.end(), dataSubsets[i].begin(), dataSubsets[i].end());
            else
                testingSet = dataSubsets[i];
        }

        //testingSet = data;
        ID3 id3(trainingSet);

        int correctCount = 0;
        int incorrectCount = 0;
        for (int i = 0; i < testingSet.size(); ++i) {
            string result = id3.classify(testingSet[i]);
            if (result == testingSet[i].getClass()) {
                ++correctCount;
            }
            else {
                ++incorrectCount;
            }
        }
        cout << "\t" << j+1 << "/" << K << " jako zbior testowy" << endl;
        cout << "Rozmiar zbioru testowego:\t" << testingSet.size() << endl;
        cout << "Poprawnie zaklasyfikowane:\t" << correctCount << "\t"
             << (float)correctCount / testingSet.size() * 100 << "%" << endl;
        cout << "Niepoprawnie zaklasyfikowane:\t" << incorrectCount << "\t"
             << (float)incorrectCount / testingSet.size() * 100 << "%" << endl;
        cout << endl;

        avgError += (float)incorrectCount / testingSet.size() * 100;
    }
    avgError /= K;
    cout << "Sredni blad: " << avgError << "%" << endl << endl;

    cout << "\tCaly zbior jako zbior uczacy i testowy\n\n";
    ID3 id3(data);
    //id3.print();

    int correctCount = 0;
    int incorrectCount = 0;
    for (int i = 0; i < data.size(); ++i) {
        string result = id3.classify(data[i]);
        if (result == data[i].getClass()) {
            ++correctCount;
        }
        else {
            ++incorrectCount;
        }
    }
    cout << "Rozmiar zbioru testowego:\t" << data.size() << endl;
    cout << "Poprawnie zaklasyfikowane:\t" << correctCount << "\t"
         << (float)correctCount / data.size() * 100 << "%" << endl;
    cout << "Niepoprawnie zaklasyfikowane:\t" << incorrectCount << "\t"
         << (float)incorrectCount / data.size() * 100 << "%" << endl;
    return 0;
}
