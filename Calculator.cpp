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
      QString expression = inputLineEdit ->text();
      
      // здесь логика вычислений остальных ребят
      //================================= временно создаю строку "Вычисление" + само выражение ===============================================================
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
      historyTextEdit->append("History  has been cleared!");
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
    
  private:
    void setupCalculatorUI()
    {
      inputLineEdit = new QLineEdit; // объект - поле ввода
      inputLineEdit->setPlaceholderText("Enter your sample.."); // вспомогательный текст
      inputLineEdit->setStyleSheet("QLineEdit { font-size:17px; padding: 8px; }"); // устанавливаем параметры шрифта в поле ввода
      
      resultLineEdit = new QLineEdit;
      resultLineEdit->setPlaceholderText("Result will be here");
      resultLineEdit->setReadOnly(true); // только на чтение, т.к. это результат примера
      resultLineEdit->setStyleSheet("QLineEdit {font-size: 17px; padding: 8px; background-color: #f0f0f0;}"); // белый цвет фона
      
      historyButton = new QPushButton("Operation history"); // кнопка перехода к журналу операций
      historyButton->setStyleSheet("QPushButton { font-size: 14px; padding: 8px; background-color: #4CAF50; color: white; }"); // зеленый фон, кнопка - белая
      
      QVBoxLayout *inputLayout = new QVBoxLayout; // макет для поля ввода, результата и истории операций
      inputLayout->addWidget(inputLineEdit);
      inputLayout->addWidget(resultLineEdit);
      inputLayout->addWidget(historyButton);
      
      // кнопки для цифр
      QGridLayout *digitLayout = new QGridLayout;
      for (int j=0;j < 9; ++j){
        QPushButton *button = createButton(QString::number(j+1),SLOT(onDigitClicked()));
        digitLayout->addWidget(button, j / 3, j % 3); // 3 на 3 сетка для цифр с 1 до 9 
      }
      
      QPushButton *zeroButton = createButton("0",SLOT (onDigitClicked()));
      digitLayout->addWidget(zeroButton,3,0,1,3); // занимает всю строку: третья строка, 1 столбец и занимает всю первую строку и 3 столбца
      
      QVBoxLayout *operatorLayout = new QVBoxLayout; // для кнопок операторов +,-,*,/ и тд
      
      QStringList operatorButtons = {"+", "-", "*", "/", "root", "pow", "log", "sin", "cos", "tg", "ctg"};
      for (const QString &text : operatorButtons) {
        QPushButton *button = createButton(text,SLOT(on_Operator_Clicked()));
        button->setStyleSheet("QPushButton { font-size: 14px; padding: 10px; background-color: #FF9800; color: white; }"); // задний фон оранжевый цвет белый
        operatorLayout->addWidget(button);
      }
      
      // равно =
      QPushButton *equalButton = createButton("=",SLOT(onEqualsClicked()));
      equalButton-> setStyleSheet("QPushButton { font-size: 16px; padding: 10px; background-color:#2196F3; color: white;}"); // задний фон синий, цвет белый
      operatorLayout->addWidget(equalButton);
      
      // очистка =
      QPushButton *clearButton = createButton("C",SLOT(onClearClicked()));
      clearButton-> setStyleSheet("QPushButton { font-size: 16px; padding: 10px; background-color:#f44336; color: white;}"); // задний фон красный, цвет белый
      operatorLayout->addWidget(clearButton);
      
      //backsapce
      QPushButton *backspaceButton = createButton("⌫",SLOT(onBackspaceClicked()));
      operatorLayout->addWidget(backspaceButton);
      
      QHBoxLayout *buttonLayout = new QHBoxLayout;
      buttonLayout->addLayout(digitLayout);
      buttonLayout->addLayout(operatorLayout);
      
      QVBoxLayout *calculatorLayout = new QVBoxLayout(calculatorWidget);
      calculatorLayout->addLayout(inputLayout);
      calculatorLayout->addLayout(buttonLayout);
      
      connect(historyButton, &QPushButton::clicked,this,&Calculator::showHistory);
      
    }
  
    void setupHistoryUI()
    {
  
    }
  
    QPushButton *createButton(const QString &text, const char *member)
    {
      QPushButton *button = new QPushButton(text);
      button -> setStyleSheet("QPushButton { font-size: 16px; padding: 10px; background-color: #e0e0e0; }" "QPushButton:hover { background-color:#d0d0d0; }"); // серый цвет у кнопки,чуть темнее серый при наведении мышкой
      connect(button,SIGNAL(clicked()),this,member);
      return button;
    }
  
    void add_to_History(const QString &operation, const QString &result)
    {
      operationHistory.push_back(qMakePair(operation,result));
    }
    
    // основа
    QStackedWidget *switcherWidget;
    QWidget *calculatorWidget;
    QWidget *historyWidget;
    
    // элементы калькулятора
    QLineEdit *inputLineEdit;
    QLineEdit *resultLineEdit;
    QPushButton *historyButton;
  
    // элементы истории
    QTextEdit *historyTextEdit;
    QPushButton *backButton;
    QPushButton *clearHistoryButton;
    
    vector<QPair<QString, QString>> operationHistory;

};







int main(int argc, char *argv[]) {
  QApplication app(argc,argv);
  
  Calculator calculator;
  calculator.show();
  
  return app.exec();
}

#include "Calculator.moc"
