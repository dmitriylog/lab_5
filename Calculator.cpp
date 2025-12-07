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

using namespace std;

// класс калькулятора с т.з. графической составляющей
class Calculator: public QWidget
{
    Q_OBJECT
  public:
                        // параметр указывающий на родительский виджет, nullptr - значение по умолчанию
    Calculator(QWidget *parent = nullptr) : QWidget(parent) // вызывает конструктор базового класса QWidget и передает ему параметр parent
    {
      switcherWidget = new QStackedWidget(this); // переключатель между калькулятором и историей

      calculatorWidget = new QWidget;
      historyWidget = new QWidget;

      setupCalculatorUI(); // настройка интерфейсов
      setupHistoryUI();

      // добавление виджетов калькулятора и истории в свитчер
      switcherWidget->addWidget(calculatorWidget);
      switcherWidget->addWidget(historyWidget);

      QVBoxLayout *mainLayout = new QVBoxLayout(this); // для текущего объекта, mainLayout - вертикальное расположение для главного окна калькулятора
      mainLayout->addWidget(switcherWidget); // занимает всё место в окне

      switcherWidget->setCurrentIndex(0); // по умолчанию показывается калькулятор

    }

  private slots:

    void onDigitClicked()
    {
      QPushButton *clickedButton = qobject_cast<QPushButton *>(sender()); // возвращает объект, который отправил сигнал - кнопка
                                                                          // преобразовывает sender в указатель на QPushButton
      if (clickedButton) { // если преобразование прошло
        QString digit = clickedButton->text(); //получаем текст с кнопки - цифру от 0 до 9
        inputLineEdit->setText(inputLineEdit->text() + digit); // добавляем цифру в поле ввода
      }
    }

    void on_Operator_Clicked()
    {
      QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
      if (clickedButton){
        QString op = clickedButton->text(); // получаем текст оператора
        inputLineEdit->setText(inputLineEdit->text() + " " + op + " "); // добавление оператора с пробелами вокруг
      }
    }

    void onEqualsClicked()
    {
      QString expression = inputLineEdit ->text().trimmed();

      // проверка не пустое ли выражение
      if (expression.isEmpty()){
        showError("Expression is empty!");
        return;
      }

      // проверка корректности выражения
      QString errorMessage;
      if (!isValidExpression(expression,errorMessage)) {
        showError(errorMessage);
        return;
      }



      // здесь логика вычислений остальных ребят
      // все, кто писал свои функции по вычислениям, добавляйте их сюда(перед QString result = "Expression" + expression;) последовательно, состыкую с интерфейсом уже я
      //================================= временно создаю строку "Вычисление" + само выражение ===============================================================

      // Ульянов Валерий - Возведение в степень
      float Degree(float num, float deg) {
      int k = 1; float sum = num;
      while (k < deg) {
    	  sum *= num;
    	  k += 1;
    	  }
      return sum;
     }
     //Дмитриев Никита -Операция деления
     double divider(double dividend, double divisor) {
    if (divisor == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    return dividend / divisor;
}

	//Ивлева Диана - Вычисление логарифма_______________________________________
// Функция принимает число x
double ln(double x) {

    if (x <= 0) return 0.0; // в основании логарифма не может быть ноль

    double y = (x - 1.0) / (x + 1.0);
    double y2 = y * y;

    double term = y;      // первый член ряда
    double sum = term;    // сумма начинается с первого члена
    int n = 1;            // n - номера знаменателей

    for (int k = 1; k < 100000; ++k) {
        term *= y2;                 // чтобы степени были 3,5,7...
        double add = term / (2 * k + 1); // знаменатель: 3,5,7,...
        sum += add;
        if (add > 0 && add < 1e-14) break; // достаточно малая точность
        if (add < 0 && -add < 1e-14) break;
    }
    return 2.0 * sum;
}

// Основная функция логарифма по основанию base: log_base(value) = ln(value) / ln(base)
double log(double base, double value) {
    if (base <= 0 || base == 1.0) {
        cout << "Error: base will be > 0 and != 1\n";  //ограничение
        return 0.0;
    }
    if (value <= 0) {
        cout << "Error: value will be > 0\n";         //ограничение
        return 0.0;
    }

    // если value == 1 -> логарифм всегда 0
    if (value == 1.0) return 0.0;


    double ln_b = ln(value);
    double ln_a = ln(base);

    // защита от деления на ноль
    if (ln_a == 0.0) {
        cout << "Error: ln(base) = 0\n";
        return 0.0;
    }

    return ln_b / ln_a;
}

    //Лёдов Даниил - Умножение, синус 
    double multiplication(int a, int b) {
        return a * b;
    }

    double sinus(double degrees) {
        // 1. Конвертируем градусы в радианы
        double radians = degrees * (3.14159265 / 180.0);

        // 2. Вычисляем синус от радиан
        double result = std::sin(radians);

        return result;
    }


//_______________________________________________________________________

      //Григорьев Игнат - Операция тангенса
      float tangent_deg(float degrees) {
        float radians = degrees * M_PI / 180.0f;
        float mod = fmod(fabs(degrees), 180.0f);
        if (fabs(mod - 90.0f) < 1e-6) {
          throw std::runtime_error("Тангенс не определён при углах 90 + 180*k градусов!");
        }
        return tan(radians);
     }
//  функции вычисления любого корня - Даша
double root(double a, int n, double eps = 1e-10) {
    if (a == 0) return 0;
    if (n == 1) return a;

    // Обработка отрицательных чисел
    bool neg = false;
    if (a < 0) {
        if (n % 2 == 0) return 0.0/0.0; // NaN
        neg = true;
        a = -a;
    }



    // Кулиев Заур -вычитание и сложение
    int substraction(int a, int b) {
        return a - b;
    }

    int summation(int a, int b) {
        return a + b;
    }





    // Начальное приближение
    double x = (a > 1) ? a / n : 1.0;
    double prev;

    do {
        prev = x;

        // Вычисляем x^(n-1)
        double pow = 1.0;
        for (int i = 0; i < n - 1; i++) {
            pow *= x;
        }

    // Малаев Давид - Операция котангенса
float cotangent_deg(float degrees) {
    float radians = degrees * M_PI / 180.0f;
    float mod = fmod(fabs(degrees), 180.0f);
    
    if (fabs(mod) < 1e-6) {
        throw std::runtime_error("Котангенс не определён при углах 0 + 180*k градусов! ");
    }
    
    float tan_value = tan(radians);
    return 1.0f / tan_value;
}

        
        // Формула Ньютона
        x = ((n - 1) * x + a / pow) / n;

    } while ((x > prev ? x - prev : prev - x) > eps);

    return neg ? -x : x;
}
      QString result = "Expression" + expression;
      resultLineEdit->setText(result);

      // добавляю в историю
      add_to_History(expression,result);
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
          historyTextEdit->append(QString("%1.%2 = %3").arg(i+1).arg(operation.first).arg(operation.second));
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

    // проверка ввода
    void onTextChanged(const QString &text)
    {
      if (!isValidPartialExpression(text)) {
        showError("Incorrect input! Use only digits and operations");
        QString currentText = inputLineEdit->text();
        if (!currentText.isEmpty()) {
          inputLineEdit->setText(currentText.left(currentText.length() - 1));
        }
      }
    }



  private:
    void setupCalculatorUI()
    {
      inputLineEdit = new QLineEdit; // объект - поле ввода
      inputLineEdit->setPlaceholderText("Enter your sample.."); // вспомогательный текст
      inputLineEdit->setStyleSheet("QLineEdit { font-size:17px; padding: 8px; }"); // устанавливаем параметры шрифта в поле ввода

      // проверка ввода в реальном времени
      connect(inputLineEdit, &QLineEdit::textChanged,this,&Calculator::onTextChanged);

      resultLineEdit = new QLineEdit;
      resultLineEdit->setPlaceholderText("Result will be here");
      resultLineEdit->setReadOnly(true); // только на чтение, т.к. это результат примера
      resultLineEdit->setStyleSheet("QLineEdit {font-size: 17px; padding: 8px; background-color: #f0f0f0;}"); // белый цвет фона

      historyButton = new QPushButton("Operation history"); // кнопка перехода к журналу операций
      historyButton->setStyleSheet("QPushButton { font-size: 17 px; padding: 8px; background-color: #4CAF50; color: white; }"); // зеленый фон, кнопка - белая

      //exitButton = new QPushButton("Exit");
      //exitButton->setStyleSheet("QPushButton { font-size: 17px; padding: 8px; background-color: #FF5722; color: white; }"); // оранжево - красный цвет

      QVBoxLayout *inputLayout = new QVBoxLayout; // макет для поля ввода, результата и истории операций
      inputLayout->addWidget(inputLineEdit);
      inputLayout->addWidget(resultLineEdit);
      inputLayout->addWidget(historyButton);

      //QHBoxLayout *topButtonLayout = new QHBoxLayout;
      //topButtonLayout->addWidget(historyButton);
      //topButtonLayout->addWidget(exitButton);

      //inputLayout->addLayout(topButtonLayout);

      // кнопки для цифр
      QGridLayout *digitLayout = new QGridLayout;
      digitLayout->setSpacing(3); // уменьшаем расстояние между кнопками
      digitLayout->setContentsMargins(2,2,2,2); //уменьшаем отступы

      for (int j=0;j < 9; ++j){
        QPushButton *button = createButton(QString::number(j+1),SLOT(onDigitClicked()));
        button->setStyleSheet("QPushButton { font-size: 18px; padding: 12px; background-color: #e0e0e0; }"
                             "QPushButton:hover { background-color:#d0d0d0; }");
        digitLayout->addWidget(button, j / 3, j % 3); // 3 на 3 сетка для цифр с 1 до 9
      }

      QPushButton *zeroButton = createButton("0",SLOT (onDigitClicked()));
      zeroButton->setStyleSheet("QPushButton { font-size: 18px; padding: 12px; background-color: #e0e0e0; }"
                               "QPushButton:hover { background-color:#d0d0d0; }");
      digitLayout->addWidget(zeroButton,3,0,1,3); // занимает всю строку: третья строка, 1 столбец и занимает всю первую строку и 3 столбца

      QPushButton *exitButton = new QPushButton("Exit");
      exitButton->setStyleSheet("QPushButton { font-size: 17px; padding: 8px; background-color: #FF5722; color: white; }"); // оранжево - красный цвет
      connect(exitButton,&QPushButton::clicked,this,&Calculator::exitApplication);
      digitLayout->addWidget(exitButton,4,0,1,3);

      QVBoxLayout *operatorLayout = new QVBoxLayout; // для кнопок операторов +,-,*,/ и тд
      operatorLayout->setSpacing(3);

      // очистка =
      QPushButton *clearButton = createButton("C",SLOT(onClearClicked()));
      clearButton-> setStyleSheet("QPushButton { font-size: 18px; padding: 10px; background-color:#f44336; color: white;}"); // задний фон красный, цвет белый
      operatorLayout->addWidget(clearButton);

      //backsapce
      QPushButton *backspaceButton = createButton("⌫",SLOT(onBackspaceClicked()));
      operatorLayout->addWidget(backspaceButton);

      QStringList operatorButtons = {"(",")","+", "-", "*", "/", "√", "^", "log", "sin", "cos", "tg", "ctg"};
      for (const QString &text : operatorButtons) {
        QPushButton *button = createButton(text,SLOT(on_Operator_Clicked()));
        button->setStyleSheet("QPushButton { font-size: 20px; padding: 10px; background-color: #FF9800; color: white; }"); // задний фон оранжевый цвет белый
        operatorLayout->addWidget(button);
      }

      // равно =
      QPushButton *equalButton = createButton("=",SLOT(onEqualsClicked()));
      equalButton-> setStyleSheet("QPushButton { font-size: 16px; padding: 10px; background-color:#2196F3; color: white;}"); // задний фон синий, цвет белый
      operatorLayout->addWidget(equalButton);

      QHBoxLayout *buttonLayout = new QHBoxLayout;
      buttonLayout->setSpacing(5);
      buttonLayout->addLayout(digitLayout);
      buttonLayout->addLayout(operatorLayout);

      QVBoxLayout *calculatorLayout = new QVBoxLayout(calculatorWidget);
      buttonLayout->setSpacing(8);
      calculatorLayout->addLayout(inputLayout);
      calculatorLayout->addLayout(buttonLayout);

      connect(historyButton, &QPushButton::clicked,this,&Calculator::showHistory);
      //connect(exitButton, &QPushButton::clicked,this,&Calculator::exitApplication);

    }

    void setupHistoryUI()
    {
      QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);

      // текст
      QLabel *historyLabel = new QLabel ("History list");
      historyLabel -> setStyleSheet("QLabel { font-size: 18px; font-weight: bold; padding: 10px; }");

      // поле отображение истории
      historyTextEdit = new QTextEdit;
      historyTextEdit->setReadOnly(true);
      historyTextEdit->setStyleSheet("QTextEdit { font-size: 14px; padding: 10px; }");

      // кнопка вернуться
      backButton = new QPushButton("Back");
      backButton->setStyleSheet(
                                "QPushButton {"
                                    "font-size: 14px;"
                                    "padding: 8px;"
                                    "font-weight: bold;"
                                    "background-color: #2196F3;"
                                    "color: white; "
                                "}"); // небесного цвета задний фон кнопки

      // кнопка очистить историю
      clearHistoryButton = new QPushButton("Clear history");
      clearHistoryButton->setStyleSheet("QPushButton { font-size: 14px; padding: 8px; font-weight: bold; background-color: #f44336; color: white; }"); // красный цвет задний фон

      // макеты для кнопок
      QHBoxLayout *buttonLayout = new QHBoxLayout;
      buttonLayout->addWidget(backButton);
      buttonLayout->addWidget(clearHistoryButton);

      historyLayout->addWidget(historyLabel);
      historyLayout->addWidget(historyTextEdit);
      historyLayout->addLayout(buttonLayout);

      // подключение кнопок
      connect(backButton, &QPushButton::clicked,this,&Calculator::showCalculator);
      connect(clearHistoryButton,&QPushButton::clicked,this,&Calculator::on_ClearHistory_Clicked);

    }

    // создание кнопок
    QPushButton *createButton(const QString &text, const char *member)
    {
      QPushButton *button = new QPushButton(text);
      button -> setStyleSheet("QPushButton { font-size: 22px; padding: 20px; background-color: #e0e0e0; }" "QPushButton:hover { background-color:#d0d0d0; }"); // серый цвет у кнопки,чуть темнее серый при наведении мышкой
      connect(button,SIGNAL(clicked()),this,member);
      return button;
    }

    void add_to_History(const QString &operation, const QString &result)
    {
      operationHistory.push_back(qMakePair(operation,result));
    }

    bool isValidExpression(const QString &expression, QString &errorMessage) {

      // удаление пробелов
      QString cleanExpression = expression;
      cleanExpression.replace(" ","");

      // проверяем скобки
      if (!checkSkobki(cleanExpression,errorMessage)) {
        return false;
      }

      if (!checkDoubleOp(cleanExpression,errorMessage)) {
        return false;
      }

      if (!checkValidSymbols(cleanExpression,errorMessage)) {
        return false;
      }

      if (!checkStructure(cleanExpression, errorMessage)) {
        return false;
      }

      return true;

}

    bool checkSkobki(const QString &expression,QString &errorMessage)
    {
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

    bool checkDoubleOp(const QString &expression,QString &errorMessage)
    {
      QString operators = "+-*/^";

      for (int i = 0; i < expression.length() - 1; ++i) {
        QChar current = expression[i];
        QChar next = expression[i+1];

        // проверяется два оператора подряд
        if (operators.contains(current) && operators.contains(next)) {
          // исключение - начало ввода. Оно может начинается с минуса или с плюса
          if (!((current == '-' && next == '-') ||(current == '+' && next == '+'))) {
            if (i == 0 || expression[i-1] == '(') {
              continue;
            }
            errorMessage = QString("Double operators '%1%2' are not allowed!").arg(current).arg(next);
            return false;
          }
        }

        // проверка оператора после открывающей скобкой (кроме - и +)
        if (current == '(' && operators.contains(next) && next != '+' && next !='-') {
          errorMessage = QString("Operator '%1' cannot follow opening parenthesis").arg(next);
          return false;
        }

        // проверка оператора перед закрывающей скобкой
        if (operators.contains(current) && next == ')') {
          errorMessage = QString("Operator '%1' cannot precede closing parenthesis").arg(current);
          return false;
        }
      }
      return true;
    }


    bool checkValidSymbols(const QString &expression, QString &errorMessage)
    {
        QRegularExpression validChars("[0-9+\\-*/.\\^√()\\s]+|log|sin|cos|tg|ctg");
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
      if (expression.isEmpty()){
        return true;
      }

      // проверка начала и конца вырадения
      QChar first_ch = expression[0];
      QChar last_ch = expression[expression.length() - 1];
      QString operators = "+-*/^";

      // не начинается  с оператора (кроме - и +)
      if (operators.contains(first_ch) && first_ch != '+' && first_ch != '-') {
        errorMessage = QString ("Expression cannot start with operator '%1'").arg(last_ch);
        return false;
      }

      // проверка на пустые скобки
      if (expression.contains("()")) {
        errorMessage = QString ("Empty parentheses '()' are not allowed!");
        return false;
      }
      return true;
    }

    bool isValidPartialExpression(const QString &expression)
    {
      QRegularExpression validChars("[0-9+\\-*/.\\^√()\\s]");

      for (int i = 0; i < expression.length(); ++i) {
        QChar ch = expression[i];
        if (!validChars.match(ch).hasMatch() && expression.mid(i).startsWith("log") && expression.mid(i).startsWith("sin") && expression.mid(i).startsWith("cos") && expression.mid(i).startsWith("tg") && expression.mid(i).startsWith("ctg")) {
          return false;
        }
      }
      return true;
    }

    void showError(const QString &message)
    {
      QMessageBox::warning(this,"Input Error",message);
    }

    // основа
    QStackedWidget *switcherWidget;
    QWidget *calculatorWidget;
    QWidget *historyWidget;

    // элементы калькулятора
    QLineEdit *inputLineEdit;
    QLineEdit *resultLineEdit;
    QPushButton *historyButton;
    QPushButton *exitButton;

    // элементы истории
    QTextEdit *historyTextEdit;
    QPushButton *backButton;
    QPushButton *clearHistoryButton;

    vector<QPair<QString, QString>> operationHistory; // контейнер для хранения истории операций калькулятора

};


int main(int argc, char *argv[]) {
  QApplication app(argc,argv);

  Calculator calculator;
  calculator.resize(1350, 600);
  calculator.show();

  return app.exec();
}

#include "Calculator.moc"
