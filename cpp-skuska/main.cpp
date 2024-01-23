#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <fstream>

class Element {
public:
    enum Type { INTEGER, STRING };

    Type type;
    std::string stringValue;
    int intValue;

    Element(const std::string& value) : type(STRING), stringValue(value), intValue(0) {}
    Element(int value) : type(INTEGER), stringValue(""), intValue(value) {}
};

class Line {
public:
    void parseString(const std::string& input) {
        std::string currentToken;

        bool prev_int = false;
        for (char ch : input) {
            if (std::isdigit(ch)) {
                if (prev_int) {
                    currentToken += ch;
                } else {
                    if (!currentToken.empty()) {
                        elements.emplace_back(Element(currentToken));
                        currentToken.clear();
                    }
                    currentToken += ch;
                    prev_int = true;
                }
            } else {
                if (prev_int) {
                    elements.emplace_back(Element(std::stoi(currentToken)));
                    currentToken.clear();
                    prev_int = false;
                }
                currentToken += ch;
            }
        }

        if (!currentToken.empty()) {
            if (prev_int) {
                elements.emplace_back(Element(std::stoi(currentToken)));
            } else {
                elements.emplace_back(Element(currentToken));
            }
        }
    }

    std::pair<bool, int> compareWithLine(const Line& line1) {
        const std::vector<Element>& elements1 = line1.elements;
        const std::vector<Element>& elements2 = elements;

        if (elements1.size() != elements2.size()) {
            return { false, 0 };
        }

        bool stringElementsEqual = true;
        int differingNumbers = 0;

        for (size_t i = 0; i < elements1.size(); ++i) {
            if (elements1[i].type == elements2[i].type) {
                if (elements1[i].type == Element::STRING) {
                    if (elements1[i].stringValue != elements2[i].stringValue) {
                        stringElementsEqual = false;
                        break;
                    }
                } else {
                    if (elements1[i].intValue != elements2[i].intValue) {
                        ++differingNumbers;
                    }
                }
            } else {
                stringElementsEqual = false;
                break;
            }
        }

        return { stringElementsEqual , differingNumbers };
    }

    std::vector<Element> elements;
};


struct Pair {
    int distance;
    int numberChanges;

    Pair(int d, int n) : distance(d), numberChanges(n) {}
    Pair() : distance(0), numberChanges(0) {}
};


std::pair<int, int> levenshteinDistance(std::vector<Line>& file1, std::vector<Line>& file2) {
    std::vector<Pair> dist(file1.size() + 1);
    std::vector<Pair> new_dist(file1.size() + 1);


    for (size_t i = 0; i <= file1.size(); ++i) {
        dist[i] = Pair(i, 0);
    }

    for (size_t j = 1; j <= file2.size(); ++j) {
        new_dist[0] = Pair(j, 0);

        for (size_t i = 1; i <= file1.size(); ++i) {
            Pair cost;
            std::pair<bool, int> compare = file1[i - 1].compareWithLine(file2[j - 1]);
            
            if (compare.first) {
                cost = Pair(0, compare.second);
            } else {
                cost =  Pair(1, 0);
            }

            new_dist[i] = std::min({
                Pair(dist[i - 1].distance + cost.distance, dist[i - 1].numberChanges + cost.numberChanges),
                Pair(dist[i].distance + 1, dist[i].numberChanges),
                Pair(new_dist[i - 1].distance + 1, new_dist[i - 1].numberChanges),
                }, [](const Pair& a, const Pair& b) {
                    return (a.distance < b.distance) || (a.distance == b.distance && a.numberChanges < b.numberChanges);
                }
            );
        }

        swap(dist, new_dist);
    }

    return { dist[file1.size()].distance, dist[file1.size()].numberChanges };
}


int main(int argc, char* argv[]) {
    std::ifstream file1(argv[1]);
    std::ifstream file2(argv[2]);

    std::vector<Line> lines1;
    std::vector<Line> lines2;

    std::string line;
    while (getline(file1, line)) {
        Line newLine;
        newLine.parseString(line);
        lines1.push_back(newLine);
    }

    while (getline(file2, line)) {
        Line newLine;
        newLine.parseString(line);
        lines2.push_back(newLine);
    }

    std::pair<int, int> result = levenshteinDistance(lines1, lines2);

    std::cout << "line changes: " << result.first << std::endl;
    std::cout << "number changes: " << result.second << std::endl;    

    return 0;
}
