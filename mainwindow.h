#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFont>
#include <QPalette>
#include <QString>
#include <QFrame>
#include "cellbutton.h"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void randomGenerate(int total);
    //random generate numbers in sudoku[][], and generate its answer in ansSudoku[][]
    void customSet(); //make user custom their question
    void spawnCells(); //spawn cells in page 2
    bool isInvalid(int a, int b, int su[][9]); //return if su[a][b] is invalid or not
    bool isNotSolvable(int total); //return if ansSudoku[][] is solvable or not
    bool canBeFilled(int *blank[], int n, int total);
    //use recursive method to try to fill numbers in ansSudoku[][]
    void createBlankPointers(int *blank[], int amountOfBlank[], int total);
    //make every elements of *blank[] point to blank cells
    void blankCalculate(int amountOfBlank[]); //calculate quantity of blanks and save result in array
    int smallest(int array[], int x); //select the smallest number in array and return its index
    void clearPage2(); //kill all cells and reset all things in page 2
    void redNumberBlanks(); //make blank cells have red numbers
    void greenNumberCells(); //make all cells have green numbers
    QFrame *coverCells();

private slots:
    void gameStart(); //turn to page 3, make user choose game mode
    void gameStart_Random(); //turn to page 2, start game with random mode
    void gameStart_Custom(); //turn to page 2, start game with custom mode
    void newGameStart(); //start a new game with the same game mode
    void back2Menu(); //turn to page 1, clear page 2(if user is in page 2)
    void customGenerate();
    //generate numbers in sudoku[][] as user's input, and generate its answer in ansSudoku[][]
    void showAnswer(); //show the ansSudoku[][] in cells, reclick ShowAnswer button to resume
    void checkAnswer(); //check if user's answer is correct or not
    void blinkResult(); //make result blink
    void gameSetting();  //turn to page 4, make user adjust settings
    void setRandomRange(); //reset random_min and random_max
    //void rUSure(void (*slot)()); //show a confirm window before call function
    void clearAnswer();
    void help();
    void back2Game();
    void changeColorTheme();

private:
    Ui::MainWindow *ui;
    CellButton *cell[9][9];
    int sudoku[9][9], ansSudoku[9][9], userSudoku[9][9];
    bool isShowingAns;
    QFrame *cover;
    QTimer *blinkTimer;
    QPushButton *CustomFinish;
    int mode, random_min, random_max;
};


#endif // MAINWINDOW_H
