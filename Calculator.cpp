#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QString>
#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <stack>
#include <cmath>
#include <sstream>
#include <vector>
#include <map>
#include <limits> // Для numeric_limits

using namespace std;

// класс калькулятора с т.з. графической составляющей
class Calculator: public QWidget
{
    Q_OBJECT
public:
    Calculator(QWidget *parent = nullptr) : QWidget(parent)
    {
        switcherWidget = new QStackedWidget(this);
        calculatorWidget = new QWidget;
        historyWidget = new QWidget;
        
        setupCalculatorUI();
        setupHistoryUI();
        
        switcherWidget->addWidget(calculatorWidget);
        switcherWidget->addWidget(historyWidget);
        
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(switcherWidget);
        switcherWidget->setCurrentIndex(0);
    }
    
private slots:
    void onDigitClicked()
    {
        QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
        if (clickedButton) {
            QString digit = clickedButton->text();
            inputLineEdit->setText(inputLineEdit->text() + digit);
        }
    }
    
    // клик на любой оператор
    void on_Operator_Clicked()
    {
        QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
        if (clickedButton){
            QString op = clickedButton->text();
            // Специальная обработка для log и √: добавляем сразу '('
            if (op == "log" || op == "√") {
                // Добавляем пробел перед функцией, чтобы отделить от предыдущего числа
                inputLineEdit->setText(inputLineEdit->text() + " " + op + "(");
            } else {
                // Для бинарных операторов добавляем пробелы с обеих сторон
                inputLineEdit->setText(inputLineEdit->text() + " " + op + " ");
            }
        }
    }
    
    void onEqualsClicked()
    {
        QString expression = inputLineEdit->text().trimmed();
        
        if (expression.isEmpty()){
            showError("Expression is empty!");
            return;
        }
        
        QString errorMessage;
        if (!isValidExpression(expression, errorMessage)) {
            showError(errorMessage);
            return;
        }
        
        try {
            // вычисление выражения
            double result = evaluateExpression(expression);
            
            // проверка на 0 или бесконечность
            if (std::isnan(result) || std::isinf(result)) {
                showError("Calculation resulted in an undefined or infinite value.");
                return;
            }

            QString resultStr = QString::number(result, 'f', 6);
            
            // убираю лишние нули после точки
            while (resultStr.contains('.') && (resultStr.endsWith('0') || resultStr.endsWith('.'))) {
                resultStr.chop(1);
                if (resultStr.endsWith('.')) {
                    resultStr.chop(1);
                    break;
                }
            }
            
            resultLineEdit->setText(resultStr);
            add_to_History(expression, resultStr);
        }
        catch (const exception& e) {
            showError(QString("Calculation error: %1").arg(e.what()));
        }
    }
    
    void onClearClicked()
    {
        inputLineEdit->clear();
        resultLineEdit->clear();
    }
    
    void onBackspaceClicked()
    {
        QString text = inputLineEdit->text();
        if (!text.isEmpty()){
            // удаление пробела перед оператором/функцией
            if (text.endsWith(" ")) {
                text.chop(1);
            }
            
            // обработка удаления целых операторов или функций
            if (text.endsWith("+") || text.endsWith("-") || text.endsWith("*") || text.endsWith("/") || text.endsWith("^") || text.endsWith("√") || text.endsWith("(") || text.endsWith(")")) {
                inputLineEdit->setText(text.left(text.length() - 1));
                return;
            }
            if (text.endsWith("sin") || text.endsWith("cos") || text.endsWith("ctg") || text.endsWith("log")) {
                inputLineEdit->setText(text.left(text.length() - 3));
                return;
            }
            if (text.endsWith("tg")) {
                 inputLineEdit->setText(text.left(text.length() - 2));
                 return;
            }
            
            inputLineEdit->setText(text.left(text.length() - 1));
        }
    }
    
    void on_ClearHistory_Clicked()
    {
        operationHistory.clear();
        historyTextEdit->clear();
        historyTextEdit->append("History has been cleared!");
    }
    
    void showHistory()
    {
        historyTextEdit->clear();
        if (operationHistory.empty()){
            historyTextEdit->append("History is empty!");
        } else {
            for (size_t i = 0; i < operationHistory.size(); ++i){
                const auto &operation = operationHistory[i];
                historyTextEdit->append(QString("%1. %2 = %3").arg(i+1).arg(operation.first).arg(operation.second));
            }
        }
        switcherWidget->setCurrentIndex(1);
    }
    
    void showCalculator()
    {
        switcherWidget->setCurrentIndex(0);
    }
    
    void exitApplication(){
        QApplication::quit();
    }
    
    void onTextChanged(const QString &text)
    {
        // для проверки символов используем isValidPartialExpression.
        if (!isValidPartialExpression(text)) {
            // если введен недопустимый символ - показываем ошибку, но не обрезаем текст.
            // при ошибке вычисления onEqualsClicked покажет более точное сообщение.
        }
    }
    
private:
    void setupCalculatorUI()
    {
        inputLineEdit = new QLineEdit;
        inputLineEdit->setPlaceholderText("Enter expression (e.g., 2 + 3)");
        inputLineEdit->setStyleSheet("QLineEdit { font-size:17px; padding: 8px; }");
        
        connect(inputLineEdit, &QLineEdit::textChanged, this, &Calculator::onTextChanged);
        
        resultLineEdit = new QLineEdit;
        resultLineEdit->setPlaceholderText("Result will be here");
        resultLineEdit->setReadOnly(true);
        resultLineEdit->setStyleSheet("QLineEdit {font-size: 17px; padding: 8px; background-color: #f0f0f0;}");
        
        historyButton = new QPushButton("Operation history");
        historyButton->setStyleSheet("QPushButton { font-size: 17px; padding: 8px; background-color: #4CAF50; color: white; }");
        
        QVBoxLayout *inputLayout = new QVBoxLayout;
        inputLayout->addWidget(inputLineEdit);
        inputLayout->addWidget(resultLineEdit);
        inputLayout->addWidget(historyButton);
        
        QGridLayout *digitLayout = new QGridLayout;
        digitLayout->setSpacing(3);
        digitLayout->setContentsMargins(2,2,2,2);
        
        for (int j=0;j < 9; ++j){
            QPushButton *button = createButton(QString::number(j+1),SLOT(onDigitClicked()));
            button->setStyleSheet("QPushButton { font-size: 18px; padding: 12px; background-color: #e0e0e0; }"
                                 "QPushButton:hover { background-color:#d0d0d0; }");
            digitLayout->addWidget(button, j / 3, j % 3);
        }
        
        QPushButton *zeroButton = createButton("0",SLOT (onDigitClicked()));
        zeroButton->setStyleSheet("QPushButton { font-size: 18px; padding: 12px; background-color: #e0e0e0; }"
                                 "QPushButton:hover { background-color:#d0d0d0; }");
        digitLayout->addWidget(zeroButton,3,0,1,3);
        
        // кнопка для запятой
        QPushButton *commaButton = createButton(",",SLOT(onDigitClicked()));
        commaButton->setStyleSheet("QPushButton { font-size: 17px; padding: 8px; background-color: #e0e0e0; }"
                                  "QPushButton:hover { background-color:#d0d0d0; }");
        digitLayout->addWidget(commaButton,4,1); 

        // кнопка для точки
        QPushButton *dotButton = createButton(".",SLOT(onDigitClicked()));
        dotButton->setStyleSheet("QPushButton { font-size: 17px; padding: 8px; background-color: #e0e0e0; }"
                                  "QPushButton:hover { background-color:#d0d0d0; }");
        digitLayout->addWidget(dotButton,4,0);

        QPushButton *exitButton = new QPushButton("Exit");
        exitButton->setStyleSheet("QPushButton { font-size: 17px; padding: 8px; background-color: #FF5722; color: white; }");
        connect(exitButton,&QPushButton::clicked,this,&Calculator::exitApplication);
        digitLayout->addWidget(exitButton,4,2); 
        
        QVBoxLayout *operatorLayout = new QVBoxLayout;
        operatorLayout->setSpacing(3);
        
        QPushButton *clearButton = createButton("C",SLOT(onClearClicked()));
        clearButton-> setStyleSheet("QPushButton { font-size: 18px; padding: 10px; background-color:#f44336; color: white;}");
        operatorLayout->addWidget(clearButton);
        
        QPushButton *backspaceButton = createButton("⌫",SLOT(onBackspaceClicked()));
        backspaceButton->setStyleSheet("QPushButton { font-size: 18px; padding: 10px; color: grey}");
        operatorLayout->addWidget(backspaceButton);
        
        QStringList operatorButtons = {"(",")","+", "-", "*", "/", "√", "^", "log","e", "sin", "cos", "tg", "ctg"};
        for (const QString &text : operatorButtons) {
            QPushButton *button = createButton(text,SLOT(on_Operator_Clicked()));
            button->setStyleSheet("QPushButton { font-size: 17px; padding: 10px; background-color: #FF9800; color: white; }");
            operatorLayout->addWidget(button);
        }
        
        QPushButton *equalButton = createButton("=",SLOT(onEqualsClicked()));
        equalButton-> setStyleSheet("QPushButton { font-size: 16px; padding: 10px; background-color:#2196F3; color: white;}");
        operatorLayout->addWidget(equalButton);
        
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->setSpacing(5);
        buttonLayout->addLayout(digitLayout);
        buttonLayout->addLayout(operatorLayout);
        
        QVBoxLayout *calculatorLayout = new QVBoxLayout(calculatorWidget);
        calculatorLayout->setSpacing(8);
        calculatorLayout->addLayout(inputLayout);
        calculatorLayout->addLayout(buttonLayout);
        
        connect(historyButton, &QPushButton::clicked,this,&Calculator::showHistory);
    }
    
    void setupHistoryUI()
    {
        QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);
        
        QLabel *historyLabel = new QLabel ("History list");
        historyLabel -> setStyleSheet("QLabel { font-size: 18px; font-weight: bold; padding: 10px; }");
        
        historyTextEdit = new QTextEdit;
        historyTextEdit->setReadOnly(true);
        historyTextEdit->setStyleSheet("QTextEdit { font-size: 14px; padding: 10px; }");
        
        backButton = new QPushButton("Back");
        backButton->setStyleSheet(
            "QPushButton {"
            "    font-size: 14px;"
            "    padding: 8px;"
            "    font-weight: bold;"
            "    background-color: #2196F3;"
            "    color: white; "
            "}");
        
        clearHistoryButton = new QPushButton("Clear history");
        clearHistoryButton->setStyleSheet("QPushButton { font-size: 14px; padding: 8px; font-weight: bold; background-color: #f44336; color: white; }");
        
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(backButton);
        buttonLayout->addWidget(clearHistoryButton);
        
        historyLayout->addWidget(historyLabel);
        historyLayout->addWidget(historyTextEdit);
        historyLayout->addLayout(buttonLayout);
        
        connect(backButton, &QPushButton::clicked,this,&Calculator::showCalculator);
        connect(clearHistoryButton,&QPushButton::clicked,this,&Calculator::on_ClearHistory_Clicked);
    }
    
    QPushButton *createButton(const QString &text, const char *member)
    {
        QPushButton *button = new QPushButton(text);
        button -> setStyleSheet("QPushButton { font-size: 22px; padding: 20px; background-color: #e0e0e0; }" "QPushButton:hover { background-color:#d0d0d0; }");
        connect(button,SIGNAL(clicked()),this,member);
        return button;
    }
    
    void add_to_History(const QString &operation, const QString &result)
    {
        operationHistory.push_back(qMakePair(operation,result));
    }
    
    // ФУНКЦИИ ВЫЧИСЛЕНИЙ ОТ КОЛЛЕГ

    // Ульянов Валерий - Возведение в степень 
    double Degree(double num, double deg) {
        return std::pow(num, deg);
    }
    
    //Дмитриев Никита - Операция деления
    double divider(double dividend, double divisor) {
        if (divisor == 0.0) {
            throw invalid_argument("Division by zero");
        }
        return dividend / divisor;
    }
    
    //Ивлева Диана - Вычисление логарифма
    double ln(double x) {
        if (x <= 0) return 0.0;
        
        double y = (x - 1.0) / (x + 1.0);
        double y2 = y * y;
        double term = y;
        double sum = term;
        
        for (int k = 1; k < 100000; ++k) {
            term *= y2;
            double add = term / (2 * k + 1);
            sum += add;
            if (add > 0 && add < 1e-14) break;
            if (add < 0 && -add < 1e-14) break;
        }
        return 2.0 * sum;
    }
    
    // Логарифм с основанием
    double log(double base, double value) {
        if (base <= 0 || base == 1.0) {
            throw invalid_argument("Base must be > 0 and != 1");
        }
        if (value <= 0) {
            throw invalid_argument("Value must be > 0");
        }
        if (value == 1.0) return 0.0;
        
        double ln_b = ln(value);
        double ln_a = ln(base);
        
        if (ln_a == 0.0) {
            throw invalid_argument("ln(base) = 0");
        }
        
        return ln_b / ln_a;
    }
    
    //Лёдов Даниил - Умножение, синус 
    double multiplication(double a, double b) {
        return a * b;
    }
    
    double sinus(double degrees) {
        double radians = degrees * (M_PI / 180.0);
        return sin(radians);
    }
    
    //Григорьев Игнат - Операция тангенса
    double tangent_deg(double degrees) {
        double radians = degrees * M_PI / 180.0;
        double mod = fmod(fabs(degrees), 180.0);
        if (fabs(mod - 90.0) < 1e-6) {
            throw runtime_error("Tangent undefined at 90 + 180*k degrees");
        }
        return tan(radians);
    }
    
    // Даша - функции вычисления любого корня
    double root(double a, int n, double eps = 1e-10) {
        if (a == 0) return 0;
        if (n == 1) return a;
        
        bool neg = false;
        if (a < 0) {
            if (n % 2 == 0) return numeric_limits<double>::quiet_NaN();
            neg = true;
            a = -a;
        }
        
        double x = (a > 1) ? a / n : 1.0;
        double prev;
        
        do {
            prev = x;
            double pow = 1.0;
            for (int i = 0; i < n - 1; i++) {
                pow *= x;
            }
            x = ((n - 1) * x + a / pow) / n;
        } while ((x > prev ? x - prev : prev - x) > eps);
        
        return neg ? -x : x;
    }
    
    // Кулиев Заур - вычитание и сложение
    double substraction(double a, double b) {
        return a - b;
    }
    
    double summation(double a, double b) {
        return a + b;
    }
    
    // Малаев Давид - Операция котангенса
    double cotangent_deg(double degrees) {
        double radians = degrees * M_PI / 180.0;
        double mod = fmod(fabs(degrees), 180.0);
        
        if (fabs(mod) < 1e-6 || fabs(mod - 180.0) < 1e-6) {
            throw runtime_error("Cotangent undefined at 0 + 180*k degrees");
        }
        
        double tan_value = tan(radians);
        // Дополнительная проверка на ноль для безопасности
        if (fabs(tan_value) < 1e-10) {
             throw runtime_error("Cotangent undefined (tangent near zero)");
        }
        return 1.0 / tan_value;
    }
    
    // Косинус
    double cosinus(double degrees) {
        double radians = degrees * (M_PI / 180.0);
        return cos(radians);
    }
    
    // вспомогательная функция для нахождения соответствующей закрывающей скобки
    int findMatchingParen(const QString& expression, int start) {
        if (expression[start] != '(') return -1;
        int count = 1;
        for (int i = start + 1; i < expression.length(); ++i) {
            if (expression[i] == '(') {
                count++;
            } else if (expression[i] == ')') {
                count--;
                if (count == 0) {
                    return i;
                }
            }
        }
        return -1;
    }
    
    // функция для парсинга и вычисления выражений
    double evaluateExpression(const QString& expression) {
        QString expr = expression;
        expr.replace(" ", "");
        
        // заменяю функции с одним аргументом на символы для упрощения парсинга
        expr.replace("sin(", "s(");
        expr.replace("cos(", "c(");
        expr.replace("tg(", "t(");
        expr.replace("ctg(", "g(");
        expr.replace("√(", "r(");
      
        
        // обрабатываю степени
        expr.replace("^", "p");
        
        // создаю стеки для чисел и операторов
        stack<double> values;
        stack<QChar> ops;
        
        for (int i = 0; i < expr.length(); i++) {
            bool isUnaryMinus = false;
            if (expr[i] == '-'){
                if (i == 0 || expr[i-1] == '(' || isOperator(expr[i-1])){
                    isUnaryMinus = true;
                }
            }
            // если текущий символ - цифра или точка или унарный минус, то парсим число
            if (expr[i].isDigit() || expr[i] == '.' || isUnaryMinus) {
                QString numStr;
                if (isUnaryMinus){
                    numStr += '-';
                    i++;
                }

                while (i < expr.length() && (expr[i].isDigit() || expr[i] == '.')) {
                    numStr += expr[i];
                    i++;
                }
                i--;

                // заменяю запятую на точку для toDouble
                numStr.replace(",", "."); 
                
                // если numStr содержит только '-', это не число
                if (numStr == "-" || numStr.isEmpty()) {
                     throw runtime_error("Invalid number format.");
                }

                values.push(numStr.toDouble());
            }
            else if (expr[i] == 'e'){
              values.push(M_E); // M_E - cmath константа для е
            }
            
            // специальная обработка для log(base, value)
            else if (i + 3 < expr.length() && expr.mid(i, 3) == "log" && expr[i+3] == '(') {
                int openParen = i + 3;
                int closeParen = findMatchingParen(expr, openParen);
                
                if (closeParen == -1) {
                    throw runtime_error("Mismatched parentheses for log");
                }
                
                QString logContent = expr.mid(openParen + 1, closeParen - openParen - 1);
                
                int commaIndex = logContent.indexOf(',');
                if (commaIndex == -1) {
                    throw runtime_error("Log function requires base and value separated by comma: log(base,value)");
                }
                
                QString baseStr = logContent.left(commaIndex).trimmed();
                QString valueStr = logContent.mid(commaIndex + 1).trimmed();

                // рекурсивно вызываю evaluateExpression для вычисления аргументов.
                double base = evaluateExpression(baseStr); 
                double value = evaluateExpression(valueStr);
                
                double result = log(base, value);
                values.push(result);
                
                i = closeParen; // пропускаю всю обработанную часть log(...)
                continue;
            }
            // Если открывающая скобка - добавляем в стек операторов
            else if (expr[i] == '(') {
                ops.push('(');
            }
            // Если закрывающая скобка - вычисляем выражение в скобках
            else if (expr[i] == ')') {
                while (!ops.empty() && ops.top() != '(') {
                    if (values.size() < 2) throw runtime_error("Missing operands");
                    double val2 = values.top(); values.pop();
                    double val1 = values.top(); values.pop();
                    QChar op = ops.top(); ops.pop();
                    values.push(applyOperator(op, val1, val2));
                }
                if (!ops.empty()) ops.pop(); // удаляю '('
                else throw runtime_error("Unmatched closing parenthesis ')'");
                
                // проверяю, была ли перед скобкой функция (s, c, t, g, r)
                if (!ops.empty() && isFunction(ops.top())) {
                    QChar func = ops.top(); ops.pop();
                    if (values.empty()) throw runtime_error("Missing operand for function");
                    double val = values.top(); values.pop();
                    values.push(applyFunction(func, val));
                }
            }
            
            else if (isOperator(expr[i]) || isFunction(expr[i])) {
                // обработка унарного минуса (если - стоит в начале или после ( )
                if (expr[i] == '-') {
                    bool isUnary = (i == 0 || expr[i-1] == '(');
                }

                // если это функция - добавляю в стек операторов
                if (isFunction(expr[i])) {
                    ops.push(expr[i]);
                }
                // если это оператор
                else {
                    while (!ops.empty() && precedence(ops.top()) >= precedence(expr[i])) {
                        if (values.size() < 2) throw runtime_error("Missing operands");
                        double val2 = values.top(); values.pop();
                        double val1 = values.top(); values.pop();
                        QChar op = ops.top(); ops.pop();
                        values.push(applyOperator(op, val1, val2));
                    }
                    ops.push(expr[i]);
                }
            } else {
                // игнорирую запятую, если она не была обработана log(...)
                if (expr[i] != ',') {
                     throw runtime_error(QString("Unknown symbol: %1").arg(expr[i]).toStdString());
                }
            }
        }
        
        // применяю оставшиеся операторы
        while (!ops.empty()) {
            if (values.size() < 2) throw runtime_error("Missing operands");
            double val2 = values.top(); values.pop();
            double val1 = values.top(); values.pop();
            QChar op = ops.top(); ops.pop();
            values.push(applyOperator(op, val1, val2));
        }
        
        if (values.empty() || values.size() > 1) {
            throw runtime_error("Invalid expression");
        }
        
        return values.top();
    }
    
    bool isOperator(QChar op) {
        return op == '+' || op == '-' || op == '*' || op == '/' || op == 'p'; // p - степень
    }
    
    bool isFunction(QChar op) {
        return op == 's' || op == 'c' || op == 't' || op == 'g' || op == 'r';
    }
    
    int precedence(QChar op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        if (op == 'p') return 3; // Степень
        return 0;
    }
    
    double applyOperator(QChar op, double a, double b) {
        switch (op.toLatin1()) {
            case '+': return summation(a, b);
            case '-': return substraction(a, b);
            case '*': return multiplication(a, b);
            case '/': return divider(a, b);
            case 'p': return Degree(a, b); // возведение в степень
            default: throw runtime_error("Unknown operator");
        }
    }
    
    double applyFunction(QChar func, double val) {
        switch (func.toLatin1()) {
            case 's': return sinus(val);      // sin
            case 'c': return cosinus(val);    // cos
            case 't': return tangent_deg(val); // tg
            case 'g': return cotangent_deg(val); // ctg
            // log обрабатывается отдельно в evaluateExpression
            case 'r': return root(val, 2);    // √ (квадратный корень)
            default: throw runtime_error("Unknown function");
        }
    }
    
    // проверки выражений
    bool isValidExpression(const QString &expression, QString &errorMessage) {
        QString cleanExpression = expression;
        cleanExpression.replace(" ","");
        
        if (!checkValidSymbols(cleanExpression,errorMessage)) return false;
        if (!checkSkobki(cleanExpression,errorMessage)) return false;
        if (!checkDoubleOp(cleanExpression,errorMessage)) return false;
        if (!checkStructure(cleanExpression, errorMessage)) return false;
        
        return true;
    }
    
    bool checkSkobki(const QString &expression,QString &errorMessage) {
        stack <QChar> skobki;
        
        for (int i = 0;i <expression.length();++i) {
            QChar elem = expression[i];
            if (elem == '(') {
                skobki.push(elem);
            } else if (elem == ')') {
                if (skobki.empty()){
                    errorMessage = "Unmatched closing parenthesis ')'";
                    return false;
                }
                skobki.pop();
            }
        }
        if (!skobki.empty()) {
            errorMessage = "Unmatched opening parenthesis '('";
            return false;
        }
        return true;
    }
    
    bool checkDoubleOp(const QString &expression,QString &errorMessage) {
        QString operators = "+-*/^";
        
        for (int i = 0; i < expression.length() - 1; ++i) {
            QChar current = expression[i];
            QChar next = expression[i+1];
            
            if (operators.contains(current) && operators.contains(next)) {
                if (next == '-') continue;
                // Разрешаем ++ или -- (хотя это редкость, но математически допустимо как унарный плюс/минус)
                if ((current == '-' && next == '-') || (current == '+' && next == '+')) continue;
                    errorMessage = QString("Double operators '%1%2' are not allowed!").arg(current).arg(next);
                    return false;
                }
            
            
            if (current == '(' && operators.contains(next) && next != '+' && next !='-') {
                errorMessage = QString("Operator '%1' cannot follow opening parenthesis").arg(next);
                return false;
            }
            
            if (operators.contains(current) && next == ')') {
                errorMessage = QString("Operator '%1' cannot precede closing parenthesis").arg(current);
                return false;
            }
        }    
        return true;
    }
    
    bool checkValidSymbols(const QString &expression, QString &errorMessage) {
        // добавлена запятая для log(base, value)
        // мы проверяем только, что символы принадлежат допустимому набору
        QRegularExpression validChars("[0-9+\\-*/.\\^√()\\s,e]+|log|sin|cos|tg|ctg");
        QRegularExpressionMatchIterator i = validChars.globalMatch(expression);
        
        QString matched;
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            matched += match.captured(0);
        }
        
        if (matched.length() != expression.length()) {
            errorMessage = "Invalid symbols in expression!";
            return false;
        }
        return true;
    }
    
    bool checkStructure (const QString &expression, QString &errorMessage) {
        if (expression.isEmpty()) return true;
        
        QChar first_ch = expression[0];
        QString operators = "+-*/^";
        
        // разрешаю + или - в начале
        if (operators.contains(first_ch) && first_ch != '+' && first_ch != '-') {
            errorMessage = QString ("Expression cannot start with operator '%1'").arg(first_ch);
            return false;
        }
        
        if (expression.contains("()")) {
            errorMessage = QString ("Empty parentheses '()' are not allowed!");
            return false;
        }
        return true;
    }
    
    
    bool isValidPartialExpression(const QString &expression) {
        // допустимые символы, которые могут быть вводиться по одному
        QRegularExpression validChars("[0-9+\\-*/.\\^()\\s,e]+"); 
        
        int i = 0;
        while (i < expression.length()) {
            QChar ch = expression[i];
            
            // если символ не является одиночным допустимым символом
            if (!validChars.match(ch).hasMatch()) {
                QString remaining = expression.mid(i);
                
                if (remaining.startsWith("log")) {
                    i += 3; // пропускаем 'l', 'o', 'g'
                } else if (remaining.startsWith("sin") || remaining.startsWith("cos") || remaining.startsWith("ctg")) { // обработка sin cos ctg
                    i += 3; // пропускаем 's', 'i', 'n' и т.д.
                } else if (remaining.startsWith("tg")) { // обработка символа tg
                    i += 2; // пропускаем 't', 'g'
                } else if (remaining.startsWith("√")) { // обработка символа корня
                    i += 1; // пропускаем '√'
                } else {
                    return false;
                }
            } else {
                i++;
            }
        }
        return true;
    }
    
    void showError(const QString &message) {
        QMessageBox::warning(this,"Input Error",message);
    }
    
    QStackedWidget *switcherWidget;
    QWidget *calculatorWidget;
    QWidget *historyWidget;
    
    QLineEdit *inputLineEdit;
    QLineEdit *resultLineEdit;
    QPushButton *historyButton;
    QPushButton *exitButton;
    
    QTextEdit *historyTextEdit;
    QPushButton *backButton;
    QPushButton *clearHistoryButton;
    
    vector<QPair<QString, QString>> operationHistory;
};

int main(int argc, char *argv[]) {
    QApplication app(argc,argv);
    
    Calculator calculator;
    calculator.resize(1350, 600);
    calculator.show();
    
    return app.exec();
}

#include "Calculator.moc"
