#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <string>
#include <memory>
#include <sstream>
#include <algorithm>
#include <stdexcept>


namespace ValueExtractors {
    class ValueExtractor {
    public:
        virtual ~ValueExtractor() = default;
        virtual int getValue(const std::vector<std::string>& row) const = 0;
    };

    class Class_Constant : public ValueExtractor {
    public:
        int value;

        explicit Class_Constant(int a) : value(a) {}

        int getValue(const std::vector<std::string>& row) const override {
            return value;
        }
    };

    class ConstantValue : public ValueExtractor {
    public:
        std::size_t CurrentIndex;

        explicit ConstantValue(std::size_t ind) : CurrentIndex(ind) {}

        int getValue(const std::vector<std::string>& row) const override {
            if (CurrentIndex < row.size()) {
                try {
                    return std::stoi(row[CurrentIndex]);
                } catch (const std::invalid_argument& e) {
                    // Handle invalid conversions here
                    return 0; // Return a default value
                }
            } else {
                return 0;
            }
        }
    };
}


namespace Comparators {
    struct Equal {
        bool operator()(int a, int b) const { return a == b; }
    };

    struct NotEqual {
        bool operator()(int a, int b) const { return a != b; }
    };

    struct Greater {
        bool operator()(int a, int b) const { return a > b; }
    };

    struct Less {
        bool operator()(int a, int b) const { return a < b; }
    };

    struct GreaterEqual {
        bool operator()(int a, int b) const { return a >= b; }
    };

    struct LessEqual {
        bool operator()(int a, int b) const { return a <= b; }
    };
}


namespace ExpressionComponents {
    class ExpressionComponent {
    public:
        virtual ~ExpressionComponent() = default;
        [[nodiscard]] virtual bool evaluate(const std::vector<std::string>& row) const = 0;
    };

    template<typename Comparator>
    class Class_evaluate : public ExpressionComponent {
    public:
        std::unique_ptr<ValueExtractors::ValueExtractor> left;
        std::unique_ptr<ValueExtractors::ValueExtractor> right;

        Class_evaluate(std::unique_ptr<ValueExtractors::ValueExtractor> a, std::unique_ptr<ValueExtractors::ValueExtractor> b)
                : left(std::move(a)), right(std::move(b)) {}

        [[nodiscard]] bool evaluate(const std::vector<std::string>& row) const override {
            Comparator compareOperator;
            return compareOperator(left->getValue(row), right->getValue(row));
        }
    };
}

namespace LogicalOperators {
    using namespace ExpressionComponents;
    using namespace ValueExtractors;

    class LogicalAnd : public ExpressionComponent {
    public:
        std::unique_ptr<ExpressionComponent> left;
        std::unique_ptr<ExpressionComponent> right;

        LogicalAnd(std::unique_ptr<ExpressionComponent> a, std::unique_ptr<ExpressionComponent> b) : left(std::move(a)), right(std::move(b)) {}

        [[nodiscard]] bool evaluate(const std::vector<std::string>& row) const override {
            return left->evaluate(row) && right->evaluate(row);
        };
    };

    class LogicalOr : public ExpressionComponent {
    public:
        std::unique_ptr<ExpressionComponent> left;
        std::unique_ptr<ExpressionComponent> right;

        LogicalOr(std::unique_ptr<ExpressionComponent> a, std::unique_ptr<ExpressionComponent> b) : left(std::move(a)), right(std::move(b)) {}

        [[nodiscard]] bool evaluate(const std::vector<std::string>& row) const override {
            return left->evaluate(row) || right->evaluate(row);
        };
    };
}

namespace Parser {
    using namespace ExpressionComponents;
    using namespace LogicalOperators;
    using namespace ValueExtractors;
    using namespace Comparators;

    using ExpressionPtr = std::unique_ptr<ExpressionComponent>;
    using StringVector = std::vector<std::string>;
    using HeaderMap = std::vector<std::pair<std::string, std::size_t>>;

    ExpressionPtr parse(const StringVector& input, const HeaderMap& header);

    namespace internals {
        class ParserClass {
        public:
            std::vector<std::string> Token;
            std::size_t CurrentIndex;
            HeaderMap sortedHeader;
            std::string CurrentToken;

            ParserClass(StringVector  input, HeaderMap  header)
                    : Token(std::move(input)),
                      CurrentIndex(0),
                      sortedHeader(std::move(header)) {
                CurrentToken = this->Token[this->CurrentIndex];
            }

            ExpressionPtr A() {
                ExpressionPtr x = M();

                while (next() == "--or") {
                    eat();
                    ExpressionPtr y = M();
                    x = std::make_unique<LogicalOr>(std::move(x), std::move(y));
                }

                return x;
            }

            ExpressionPtr M() {
                ExpressionPtr x = F();

                while (next() == "--and") {
                    eat();
                    ExpressionPtr y = F();
                    x = std::make_unique<LogicalAnd>(std::move(x), std::move(y));
                }
                return x;
            }

            ExpressionPtr F() {
                std::unique_ptr<ValueExtractor> x = T();
                ExpressionPtr z;

                if (next() == "--eq") {
                    eat();
                    std::unique_ptr<ValueExtractor> y = T();
                    z = std::make_unique<Class_evaluate<Equal>>(std::move(x), std::move(y));
                }
                else if (next() == "--lt") {
                    eat();
                    std::unique_ptr<ValueExtractor> y = T();
                    z = std::make_unique<Class_evaluate<Less>>(std::move(x), std::move(y));
                }
                else if (next() == "--ne") {
                    eat();
                    std::unique_ptr<ValueExtractor> y = T();
                    z = std::make_unique<Class_evaluate<NotEqual>>(std::move(x), std::move(y));
                }
                else if (next() == "--gt") {
                    eat();
                    std::unique_ptr<ValueExtractor> y = T();
                    z = std::make_unique<Class_evaluate<Greater>>(std::move(x), std::move(y));
                }
                else if (next() == "--ge") {
                    eat();
                    std::unique_ptr<ValueExtractor> y = T();
                    z = std::make_unique<Class_evaluate<GreaterEqual>>(std::move(x), std::move(y));
                }
                else if (next() == "--le") {
                    eat();
                    std::unique_ptr<ValueExtractor> y = T();
                    z = std::make_unique<Class_evaluate<LessEqual>>(std::move(x), std::move(y));
                }

                return z;
            }

            std::unique_ptr<ValueExtractor> T() {
                std::unique_ptr<ValueExtractor> x;

                std::string nextString = next();
                eat();
                if (nextString[0] == '[') {
                    nextString = nextString.substr(1, nextString.size() - 2);
                    std::size_t index = -1;
                    for (auto & i : sortedHeader) {
                        if (i.first == nextString) {
                            index = i.second;
                            break;
                        }
                    }

                    if (index != static_cast<std::size_t>(-1)) {
                        x = std::make_unique<ConstantValue>(index);
                    } else {
                        throw std::runtime_error("Invalid column name: " + nextString);
                    }
                }
                else {
                    int intValue;
                    std::stringstream(nextString) >> intValue;
                    x = std::make_unique<Class_Constant>(intValue);
                }

                return x;
            }

            void eat() {
                if (CurrentIndex + 1 < Token.size()) {
                    CurrentIndex++;
                    CurrentToken = Token[CurrentIndex];
                }
                else {
                    CurrentToken = "";
                }
            }

            [[nodiscard]] std::string next() const {
                return CurrentToken;
            }
        };
    }

    ExpressionPtr parse(const StringVector& input, const HeaderMap& header) {
        internals::ParserClass parser(input, header);
        ExpressionPtr expression = parser.A();
        if (!parser.next().empty()) {
            throw std::runtime_error("End of input expected, but found: " + parser.next());
        }
        return expression;
    }

    bool evaluate(const ExpressionPtr& root, const std::vector<std::string>& row) {
        return root->evaluate(row);
    }
}


class DataSheet {
    using HeaderMap = std::vector<std::pair<std::string, std::size_t>>;
    std::vector<std::vector<std::string>> lines;
    std::vector<std::string> header;

public:
    void readHeader(std::istream& file) {
        std::string line;
        std::getline(file, line);

        std::string column_name;

        for (char i : line) {
            if (i == ',') {
                header.push_back(column_name);
                column_name.clear();
            }
            else {
                column_name.push_back(i);
            }
        }
        header.push_back(column_name);
    }

    void readData(std::istream& file) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cell;
            std::vector<std::string> lin; // Change to store string values

            while (std::getline(ss, cell, ',')) {
                lin.push_back(cell); // Store string value directly
            }
            lines.emplace_back(lin);
        }
    }



    void displayHeader(std::ostream& out) {

        for (std::size_t i = 0; i < header.size() - 1; ++i) {
            out << header[i] << ",";
        }
        out << header[header.size() - 1] << std::endl;
    }

    HeaderMap createSortedHeader() {
        HeaderMap sortedHeader;

        for (std::size_t i = 0; i < header.size(); ++i) {
            sortedHeader.emplace_back(header[i], i);
        }
        return sortedHeader;
    }

    void displayData(const std::unique_ptr<ExpressionComponents::ExpressionComponent>& root, std::ostream& out) {
        for (auto & line : lines) {
            if (Parser::evaluate(root, line)) {
                for (std::size_t j = 0; j < line.size() - 1; ++j) {
                    out << line[j] << ",";
                }
                out << line[line.size() - 1] << std::endl;
            }
        }
    }
};

int main(int argc, char** argv) {
    try {
        std::vector<std::string> FilesName(argv + argc - 2, argv + argc);
        std::vector<std::string> queryConditions(argv + 1, argv + argc - 2);
        std::ifstream inputDataStream(FilesName[0]);
        std::ofstream outputFile(FilesName[1]);

        if (!inputDataStream.good()) {
            std::cout << "File input can't be open" << std::endl;
            return 1;
        }

        if (!outputFile.good()) {
            std::cout << "File output can't be open" << std::endl;
            return 1;
        }

        DataSheet input;
        input.readHeader(inputDataStream);
        input.readData(inputDataStream);
        input.displayHeader(outputFile);

        auto sortedHeader = input.createSortedHeader();
        auto expression = Parser::parse(queryConditions, sortedHeader);

        input.displayData(expression, outputFile);
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
