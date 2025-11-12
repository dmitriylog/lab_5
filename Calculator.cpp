#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>

using namespace std;

// класс калькулятора с т.з. графической составляющей
class Calculator 
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
      mainLayot->addWidget(switcherWidget); // занимает всё место в окне
      
      switcherWidget->setCurrentIndex(0); // по умолчанию показывается калькулятор
      
    }
  
  private slots:
    void onDigitClicked()
    {
    
    }
    
    void on_Operator_Clicked()
    {
    
    }
    
    void onEqualsClicked()
    {
    
    }
    
    void onClearClicked()
    {
    
    }
    
    void onBackspaceClicked()
    {
    
    }
    
    void on_ClearHistory_Clicked()
    {
    
    }
    
    void showHistory()
    {
    
    }
    
    void showCalculator()
    {
    
    }
    
  private:
    void setupCalculatorUI()
    {
     
    
    
    
    }
  
    void setupHistoryUI()
    {
  
    }
  
    void add_to_History()
    {
  
    }
  
  

}







int main() {

}

#include "Calculator.moc"
