#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <cmath>
#include <utility>

using namespace std;

const int ATTRIBUTES_AMOUNT = 6;
//bool flags[] = {false, false, false, false, false, false}


class Node {
    int attribute = -1;
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
    void deleteNodes() {
        for (int i = 0; i < decisions.size(); ++i) {
            decisions[i]->deleteNodes();
            delete decisions[i];
        }
    }
    /*Node & getNext(string value) {
        for (int i = 0; i < decision.size(); ++i) {
            if (decision[i].first == value) {
                return decision[i].second;
            }
        }
    }*/
    Node* getNext(int index) {
        return decisions[index];
    }
    void print(int level = 0) {
        for (int i = 0; i < level; ++i) {
            cout << "  ";
        }
        if (attribute == -1) {
            cout << "X" << endl;
        }
        else {
            cout << attribute << endl;
            for (int i = 0; i < decisions.size(); ++i) {
                decisions[i]->print(level + 1);
            }
        }
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
    }
    Node* getRoot() {
        return root;
    }
    void print() {
        root->print();
    }
};

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

        something(data, this->node);
    }
    void print() {
        tree.print();
    }
private:
    void something(vector<Sample> data, Node* node, int attributesAmount = ATTRIBUTES_AMOUNT) {
        if (attributesAmount == 0)
            return;
        bool flag = true;
        auto claz = data[0].getClass();
        for (int i = 0; i < data.size() && flag; ++i) {
            if (claz != data[i].getClass())
                flag = false;
        }
        if (flag) {
            cout << "FAJNIE" << endl;
            return;
        }

        if (attributesAmount == 4)
            ;//printData(data);
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
        //cout << entropy << endl;

        int maxAttribute = -1;
        float maxGain = 0.0f;
        for (int i = 0; i < attributesAmount; ++i) {
            float gain = entropy;
            std::unordered_map<std::string, std::unordered_map<std::string, size_t>> attributeCount = attributesCount[i];
            //cout << attributeCount[0].first << endl;
            int classesAmount = 0;

            for (auto attr : attributeCount) {
                // ilosc klas po wybraniu atrybutu
                int classesAmount = 0;
                for (auto sClass : attr.second) {
                    classesAmount += sClass.second;
                    //cout << attr.first << '\t' << sClass.first << '\t' << sClass.second << endl;
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
           // cout << "Zdobycz informacji dla atrybutu " << i << ": " << gain << endl;
        }
        //cout << "Najwieksza zdobycz informacji dla atrybutu " << maxAttribute << ": " << maxGain << endl;

        vector<string> dec;
        for (auto attr : attributesCount[maxAttribute]) {
            dec.push_back(attr.first);
        }
        node->set(maxAttribute, dec);

        /*for (int i = 0; i < attributesCount[maxAttribute].size(); ++i) {
            something(costam(data, i), node->getNext(i), attributesAmount-1);
        }*/
        int i = 0;
        for (auto attr : attributesCount[maxAttribute]) {
            something(costam(data, maxAttribute, attr.first), node->getNext(i), attributesAmount-1);
            ++i;
        }

    }

    vector<Sample> costam(vector<Sample> & data ,int attrId, string attr) {
        vector<Sample> result;
        for (int i = 0; i < data.size(); ++i) {
            Sample temp = data[i];
            if (data[i].getAttribute(attrId) == attr) {
                temp.deleteAttribute(attrId);
                result.push_back(temp);
            }

        }
        //printData(result);
        //cout << "------------------" << endl << endl;;
        return result;
    }
};

int main()
{
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
    //getBestAttribute(data);
    //printData(data);
    ID3 id3(data);
    id3.print();


    //for (int i = 0; i < ATTRIBUTES_AMOUNT; ++i) {
    //    for (auto attr : attributesCount[maxAttribute]) {
    //        cout << attr.first << endl;
    //    }
    //}
    //attributesCount[maxAttribute];

    /*
    vector<string> dec;
    for (auto attr : attributesCount[maxAttribute]) {
        dec.push_back(attr.first);
    }
    node->set(maxAttribute, dec);
    */
    //for (int i = 0; i < ; ++i) {

    //}

    //node = node->getNext(0);
    //node->set(maxAttribute, dec);
    //tree.print();
    return 0;
}
