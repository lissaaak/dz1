#include <iostream>
#include <string>
#include <stack>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <map>
#include <cctype>
using namespace std;



// функция проверки корректности введенного выражения
bool isValidExpression(const string& s) {
    int b = 0;   // счетчик кол-ва открывающихся и закрывающихся скобок
    for (size_t i = 0; i < s.length(); ++i) { // проходим по строке 
        char c = s[i]; //  переменная, в кот сохраняется текущий символ в строке
        if (isspace(c)) continue; // если символ - пробел - игнорируем и переходим дальше 
        if (isdigit(c) || c == 'x' || c == '.' || c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/') {
            if (c == '(') b++;
            if (c == ')') b--;
            if (b < 0) return false;
        }
        else if (isalpha(c)) { // если символ равен букве, то это начало имени функции
            string func; // строка для хранения имени функции 
            while (i < s.length() && isalpha(s[i])) { // идем по всей строке, пока не встретим символ, не явл буквой 
                func += s[i++]; // добавляем в строку 
            }
            --i; // возвращаемся на последний символ функции 
            if (func != "sin" && func != "cos" && func != "tg" && func != "ctg" && func != "exp") {
                return false;
            }
        }
        else {
            return false;
        }
    }
    return b == 0; // если запись корректна, возвращаем true 
}



// функция для вычисления операций
double applyOperation(double a, double b, char op) {
    switch (op) {
    case '+': return a + b; // если оператор равен +, то выполняется сложение 
    case '-': return a - b;
    case '*': return a * b;
    case '/': // если оператор равен делению, то сначала проверяем, не равно ли второе число нулю
        if (b == 0) throw runtime_error("Делитель равен нулю");
        return a / b;
    default: throw runtime_error("Неправильный ввод"); // если оператор не равен ни одному из вышеперечисленных символов, то ошибка
    }
}




// функция для вычисления значения функции
double applyFunction(const string& s, double x) {
    if (s == "sin") return sin(x); // если строка равна sin, то выполняется операция sin над x 
    if (s == "cos") return cos(x);
    if (s == "tg") return tan(x);
    if (s == "ctg") return 1.0 / tan(x);
    if (s == "exp") return exp(x);
    throw runtime_error("Не знаю такую функцию"); // если строка не равна ничему из вышеперечисленного, то ошибка
}




// преобразование выражения в постфиксную запись 
string toRPN(const string& s) {

    map<string, int> precedence = { // таблица приоритетов операторов, ключи - операторы и фунуции, значения - их приоритет
        {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2},
        {"sin", 3}, {"cos", 3}, {"tg", 3}, {"ctg", 3}, {"exp", 3}
    };

    stack<string> operators; // стек для хранения операторов и функций во время преобразования 
    stringstream output; // строковый поток, в который будет записан результат преобразования 

    for (size_t i = 0; i < s.length(); ++i) { // перебираем все символы строки
        if (isspace(s[i])) continue; // если текущий символ - пробел - пропускаем

        if (isdigit(s[i]) || s[i] == 'x' || s[i] == '.') { // если цифра/ переменная x/ или точка
            while (i < s.length() && (isdigit(s[i]) || s[i] == 'x' || s[i] == '.')) { //добавляем символы, пока не встретим другие
                output << s[i++];
            }
            output << ' '; // добавляем пробел для разделения операндов 
            --i; // возвращаемся на последний символ
        }
        else if (isalpha(s[i])) { // если текущий символ - буква (то это название функции)
            string func; // создаем строку для этой функции 
            while (i < s.length() && isalpha(s[i])) {
                func += s[i++]; 
            }
            --i;
            operators.push(func); // добавляем функцию в стек 
        }
        else if (s[i] == '(') { // добавляем скобки 
            operators.push("(");
        }
        else if (s[i] == ')') {
            while (!operators.empty() && operators.top() != "(") { // пока в стеке не встретится открыв скобка
                output << operators.top() << ' '; // выталкиваем элементы из стека в поток output, разделяя пробелами
                operators.pop(); 
            }
            if (!operators.empty() && operators.top() == "(") operators.pop();
            if (!operators.empty() && precedence.count(operators.top())) {
                output << operators.top() << ' ';
                operators.pop();
            }
        }
        else { // если символ не скобка и не буква
            string op(1, s[i]); // создаем строку с этим оператором 
            while (!operators.empty() && precedence[operators.top()] >= precedence[op]) { // сравниваем приоритет текущего оператора с оператором в стеке 
                // пока в стеке есть операторы с равным или большим приоритетом их выталкиваем в поток 
                output << operators.top() << ' ';
                operators.pop();
            }
            operators.push(op); // текущий оператор добавляется в стек 
        }
    }

    while (!operators.empty()) { // после обработки всех символов оставшиеся операторы вытаскиваются и записываются в поток 
        output << operators.top() << ' ';
        operators.pop();
    }

    return output.str(); // возвращает строку, собранную в поток output,  которая содержит выражение в постфиксной записи
}




// Вычисление выражения 
double evaluateRPN(const string& rpn, double x) {
    stringstream ss(rpn);
    stack<double> values;
    string token;
    while (ss >> token) {
        if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-')) {
            values.push(stod(token));
        }
        else if (token == "x") {
            values.push(x);
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/") {
            if (values.size() < 2) throw runtime_error("Insufficient values for operation");
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            values.push(applyOperation(a, b, token[0]));
        }
        else {
            if (values.empty()) throw runtime_error("Function argument missing");
            double a = values.top(); values.pop();
            values.push(applyFunction(token, a));
        }
    }

    if (values.size() != 1) throw runtime_error("Invalid expression");
    return values.top();
}




int main() {
    setlocale(LC_ALL, "RU");
    string expr;
    cout << "Введите математическое выражение: ";
    getline(cin, expr);

    if (!isValidExpression(expr)) {
        cerr << "Ошибка: некорректное выражение" << endl;
        return 1;
    }

    string rpn = toRPN(expr);

    double x = 0;
    if (expr.find('x') != string::npos) {
        cout << "Введите значение x: ";
        cin >> x;
    }

    try { 
        double result = evaluateRPN(rpn, x);
        cout << "Результат: " << result << endl;
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}
