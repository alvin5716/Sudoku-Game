#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QItemSelectionModel>
#include <ctime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //prevent dangling pointer
    blinkTimer=NULL;
    CustomFinish=NULL;
    cover=NULL;
    isShowingAns=false;
    ui->Pages->setCurrentIndex(0); //page 1
    //buttons
    connect(ui->QuitButton,SIGNAL(clicked()),qApp,SLOT(quit()));
    connect(ui->StartButton,SIGNAL(clicked()),this,SLOT(gameStart()));
    connect(ui->AnswerButton,SIGNAL(clicked()),this,SLOT(showAnswer()));
    connect(ui->CheckButton,SIGNAL(clicked()),this,SLOT(checkAnswer()));
    connect(ui->MenuButton,SIGNAL(clicked()),this,SLOT(back2Menu()));
    connect(ui->MenuButton_2,SIGNAL(clicked()),this,SLOT(back2Menu()));
    connect(ui->NewGameButton,SIGNAL(clicked()),this,SLOT(newGameStart()));
    connect(ui->RandomButton,SIGNAL(clicked()),this,SLOT(gameStart_Random()));
    connect(ui->CustomButton,SIGNAL(clicked()),this,SLOT(gameStart_Custom()));
    connect(ui->SettingsButton,SIGNAL(clicked()),this,SLOT(gameSetting()));
    connect(ui->RandomMin,SIGNAL(valueChanged(int)),this,SLOT(setRandomRange()));
    connect(ui->RandomMax,SIGNAL(valueChanged(int)),this,SLOT(setRandomRange()));
    connect(ui->ClearButton,SIGNAL(clicked()),this,SLOT(clearAnswer()));
    connect(ui->HelpButton,SIGNAL(clicked()),this,SLOT(help()));
    connect(ui->HelpBackButton,SIGNAL(clicked()),this,SLOT(back2Game()));
    connect(ui->ColorButton,SIGNAL(clicked()),this,SLOT(changeColorTheme()));
    //set range of quantity of numbers in random mode
    ui->RandomMin->setMinimum(1);
    ui->RandomMax->setMaximum(40);
    ui->RandomMax->setValue(33);
    ui->RandomMin->setValue(28);
    emit setRandomRange();
}
MainWindow::~MainWindow()
{
    delete ui;
}
/*
void MainWindow::rUSure(void (*slot)()) {
    emit slot();
    return;
}*/
void MainWindow::gameStart() {
    ui->Pages->setCurrentIndex(2); //page 3
    ui->centralWidget->setFocus(); //de-focus this button
    return;
}
void MainWindow::gameStart_Random() {
    ui->Pages->setCurrentIndex(1); //page 2
    srand(time(NULL));
    randomGenerate(random_min+rand()%(random_max-random_min+1)); //random generate from min to max
    mode = 1; //random mode is "mode 1"
    spawnCells();
    redNumberBlanks();
    return;
}
void MainWindow::gameStart_Custom() {
    ui->Pages->setCurrentIndex(1); //page 2
    customSet();
    mode = 2; //random mode is "mode 2"
    spawnCells();
    return;
}
void MainWindow::newGameStart() {
    clearPage2();
    ui->centralWidget->setFocus(); //de-focus this button
    srand(time(NULL));
    if(mode==1) {  //start a new game with random mode
        randomGenerate(random_min+rand()%(random_max-random_min+1));
        spawnCells();
        redNumberBlanks();
    }
    else if(mode==2) { //start a new game with custom mode
        customSet();
        spawnCells();
    }
    return;
}
void MainWindow::gameSetting() {
    ui->Pages->setCurrentIndex(3);
    ui->centralWidget->setFocus();
    return;
}
void MainWindow::setRandomRange() {
    random_min=ui->RandomMin->value();
    random_max=ui->RandomMax->value();
    ui->RandomMin->setMaximum(ui->RandomMax->value());
    ui->RandomMax->setMinimum(ui->RandomMin->value());
    return;
}
void MainWindow::showAnswer() {
    int i, j;
    static int x, y;
    QString str;
    ui->centralWidget->setFocus(); //de-focus answer button
    if(!isShowingAns) { //show answer
        cover = coverCells(); //make a QFrame, let it cover cells, make cells unclickable
        for(i=0;i<9;++i) { //save user's answer
            for(j=0;j<9;++j) {
                str = cell[i][j]->text();
                if(str.endsWith("?")) str.chop(1); //remove question mark
                if(str!="") {
                    userSudoku[i][j]=str.toInt();
                } else userSudoku[i][j]=0;
            }
        }
        for(i=0;i<9;++i) { //display real answer
            for(j=0;j<9;++j) {
                cell[i][j]->setText(str.setNum(ansSudoku[i][j]));
            }
        }
        for(i=0;i<9;++i) { //save the axis of the selected cell
            for(j=0;j<9;++j) {
                if(cell[i][j]->beingSelected()) {
                    x=i; //save axis
                    y=j;
                    cell[i][j]->deselected(); //deselect it
                    break;
                }
            }
            if(j!=9) break;
        }
        if(j==9) { //if no cell is selected
            x=87; //this is to mark that "no cells is selected"
        }
        ui->CheckButton->hide(); // hide other buttons
        ui->NewGameButton->hide();
        ui->MenuButton->hide();
        ui->Result->hide();
        ui->HelpButton->hide();
        ui->ClearButton->hide();
        ui->AnswerButton->setText("Resume"); // change string
        isShowingAns = true;
    } else { //show user's answer
        for(i=0;i<9;i++) { //display user's answer
            for(j=0;j<9;j++) {
                if(userSudoku[i][j]!=0) {
                    cell[i][j]->setText(str.setNum(userSudoku[i][j]));
                } else cell[i][j]->setText("");
                if(cell[i][j]->beingQuested()) cell[i][j]->setText(cell[i][j]->text().append("?"));
            }
        }
        ui->CheckButton->show(); // show other buttons
        ui->NewGameButton->show();
        ui->MenuButton->show();
        ui->Result->show();
        ui->HelpButton->show();
        ui->ClearButton->show();
        ui->AnswerButton->setText("ShowAnswer"); // change string
        if(x!=87) cell[x][y]->selected(); //reselect
        if(cover!=NULL) delete cover;
        cover=NULL; //prevent dangling pointer
        isShowingAns = false;
    }
}
void MainWindow::checkAnswer() {
    int i, j;
    QString str;
    bool doesUserWin = true;
    for(i=0;i<9;++i) {
        for(j=0;j<9;++j) {
            str = cell[i][j]->text();
            if(str.endsWith("?")) str.chop(1); //remove question mark
            if(str!="") {
                userSudoku[i][j]=str.toInt();
            } else userSudoku[i][j]=0;
        }
    }
    for(i=0;i<9;++i) {
        for(j=0;j<9;++j) {
            if(userSudoku[i][j]==0||isInvalid(i,j,userSudoku)) {
                doesUserWin = false;
                break;
            }
        }
        if(!doesUserWin) break;
    }
    if(doesUserWin) {
        greenNumberCells();
        //focus back to selected button
        ui->centralWidget->setFocus(); //de-focus answer button
        ui->Result->setStyleSheet("color: #76E023;");
        ui->Result->setText("You win!");
        cover = coverCells(); //make a QFrame, let it cover cells, make cells unclickable
        ui->AnswerButton->hide();
        ui->CheckButton->hide();
        ui->HelpButton->hide();
        ui->ClearButton->hide();
        blinkTimer = new QTimer(this);
        connect(blinkTimer, SIGNAL(timeout()), this, SLOT(blinkResult()));
        blinkTimer->start(250);
        for(i=0;i<9;++i) for(j=0;j<9;++j) cell[i][j]->deselected();
    } else {
        for(i=0;i<9;++i) for(j=0;j<9;++j) if(cell[i][j]->beingSelected()) cell[i][j]->setFocus();
        ui->Result->setStyleSheet("color: red;");
        ui->Result->setText("Incorrect!");
        //make result blink
        disconnect(ui->CheckButton,0,0,0);
        blinkTimer = new QTimer(this);
        connect(blinkTimer, SIGNAL(timeout()), this, SLOT(blinkResult()));
        blinkTimer->start(250);
    }
}
void MainWindow::help() {
    ui->Pages->setCurrentIndex(4);
    ui->centralWidget->setFocus();
    return;
}
void MainWindow::back2Game() {
    ui->Pages->setCurrentIndex(1);
    for(int i=0;i<9;++i) for(int j=0;j<9;++j) if(cell[i][j]->beingSelected()) cell[i][j]->setFocus();
    return;
}
void MainWindow::blinkResult() {
    static int x=0;
    if(x>5) {
        if(blinkTimer!=NULL) delete blinkTimer;
        blinkTimer=NULL;
        x=0;
        connect(ui->CheckButton,SIGNAL(clicked(bool)),this,SLOT(checkAnswer()));
        return;
    }
    if(x%2==0) ui->Result->hide();
    else ui->Result->show();
    ++x;
    return;
}
QFrame* MainWindow::coverCells() {
    QFrame* frame = new QFrame(ui->page_2);
    frame->setGeometry(QRect(QPoint(100,100),QSize(450,450)));
    frame->setStyleSheet("background-color: transparent");
    frame->show();
    return frame;
}
void MainWindow::randomGenerate(int total) { // Random Generate question
    int i, j, k, num;
    do {
        for(i=0;i<9;i++) for(j=0;j<9;j++) sudoku[i][j]=0;
        for(num=0;num<total;++num) {
            do {
                i=rand()%9;
                j=rand()%9;
            } while(sudoku[i][j]!=0);
            sudoku[i][j]=rand()%9+1;
            for(k=0;k<9;k++) {
                sudoku[i][j]=(sudoku[i][j]==9)?1:sudoku[i][j]+1;
                if(!isInvalid(i, j, sudoku)) break;
            }
            if(k==9) break; //sudoku[i][j] can'e be filled by any 1~9 number
        }
        if(k==9) continue;
        for(i=0;i<9;i++) for(j=0;j<9;j++) ansSudoku[i][j]=sudoku[i][j];
    } while(k==9||isNotSolvable(total));
    for(i=0;i<9;i++) for(j=0;j<9;j++) userSudoku[i][j]=sudoku[i][j];
    return;
}
void MainWindow::customSet() { // Random Generate question
    int i, j;
    for(i=0;i<9;i++) for(j=0;j<9;j++) sudoku[i][j]=0;
    //spawn a finish button
    CustomFinish = new QPushButton(ui->page_2);
    CustomFinish->setGeometry(QRect(QPoint(620,290),QSize(340,70)));
    CustomFinish->setText("Finish");
    CustomFinish->setFont(QFont("Consolas",32));
    CustomFinish->setCursor(QCursor(Qt::PointingHandCursor));
    connect(CustomFinish,SIGNAL(clicked(bool)),this,SLOT(customGenerate()));
    CustomFinish->show();
    ui->NewGameButton->hide();
    ui->MenuButton->hide();
    ui->AnswerButton->hide();
    ui->CheckButton->hide();
    ui->HelpButton->hide();
    ui->ClearButton->hide();
    return;
}
void MainWindow::customGenerate() {
    int i, j, total=0;
    QString str;
    //calculate total amount of numbers
    for(i=0;i<9;++i) {
        for(j=0;j<9;++j) {
            str = cell[i][j]->text();
            if(str.endsWith("?")) str.chop(1); //remove question mark
            if(str!="") {
                ansSudoku[i][j]=sudoku[i][j]=str.toInt();
                ++total;
            } else {
                ansSudoku[i][j]=sudoku[i][j]=0;
            }
        }
    }
    //test if it is solvable or not
    if(isNotSolvable(total)) {
        qDebug()<<"nope";
        return;
    }
    for(i=0;i<9;i++) for(j=0;j<9;j++) userSudoku[i][j]=sudoku[i][j];
    //kill finish button and show other button
    if(CustomFinish!=NULL) delete CustomFinish;
    CustomFinish=NULL;
    ui->NewGameButton->show();
    ui->MenuButton->show();
    ui->AnswerButton->show();
    ui->CheckButton->show();
    ui->HelpButton->show();
    ui->ClearButton->show();
    ui->ClearButton->show();
    redNumberBlanks();
    return;
}
void MainWindow::redNumberBlanks() {
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            if(sudoku[i][j]==0) cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("color: #FF0F0F;"));
    return;
}
void MainWindow::greenNumberCells() {
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("color: #76E023;"));
    return;
}
void MainWindow::spawnCells() { // Spawn cells of sudoku
    int i, j, a, b;
    QString str;
    for(i=0;i<9;i++) {
        for(j=0;j<9;j++) {
            cell[i][j] = new CellButton(ui->sudokuBox);
            if(sudoku[i][j]!=0) {
                cell[i][j]->setText(str.setNum(sudoku[i][j]));
            } else {
                cell[i][j]->setText("");
            }
            if (((i>=3&&i<6)||(j>=3&&j<6))&&!((i>=3&&i<6)&&(j>=3&&j<6))) {
                cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("background-color: #F4F3D3;"));
            }
            else {
                cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("background-color: #FFFFFF;"));
            }
            cell[i][j]->setGeometry(QRect(QPoint(2+50*i,2+50*j),QSize(50,50)));
        }
    }
    //connect click and select/deselect
    for(i=0;i<9;i++) {
        for(j=0;j<9;j++) {
            if(sudoku[i][j]==0) {
                connect(cell[i][j],SIGNAL(clicked(bool)),cell[i][j],SLOT(selected()));
            } else {
                cell[i][j]->setChangeable(false);
            }
            for(a=0;a<9;a++) {
                for(b=0;b<9;b++) {
                    if(!(i==a&&j==b)) {
                        connect(cell[i][j],SIGNAL(clicked(bool)),cell[a][b],SLOT(deselected()));
                    }
                }
            }
            if(i!=0) {
                connect(cell[i][j],SIGNAL(selectLeft()),cell[i][j],SLOT(deselected()));
                connect(cell[i][j],SIGNAL(selectLeft()),cell[i-1][j],SLOT(selected()));
            }
            if(i!=8) {
                connect(cell[i][j],SIGNAL(selectRight()),cell[i][j],SLOT(deselected()));
                connect(cell[i][j],SIGNAL(selectRight()),cell[i+1][j],SLOT(selected()));
            }
            if(j!=0) {
                connect(cell[i][j],SIGNAL(selectUp()),cell[i][j],SLOT(deselected()));
                connect(cell[i][j],SIGNAL(selectUp()),cell[i][j-1],SLOT(selected()));
            }
            if(j!=8) {
                connect(cell[i][j],SIGNAL(selectDown()),cell[i][j],SLOT(deselected()));
                connect(cell[i][j],SIGNAL(selectDown()),cell[i][j+1],SLOT(selected()));
            }
        }
    }
    return;
}
void MainWindow::back2Menu() {
    if(ui->Pages->currentIndex()==1) clearPage2();
    ui->Pages->setCurrentIndex(0);
    ui->centralWidget->setFocus(); //de-focus this button
}
void MainWindow::clearPage2() {
    for (int i=0;i<9;++i) {
        for(int j=0;j<9;++j) {
            if(cell[i][j]!=NULL) delete cell[i][j];
            cell[i][j]=NULL; //prevent dangling pointer
            sudoku[i][j]=ansSudoku[i][j]=userSudoku[i][j]=0;
        }
    }
    isShowingAns=false;
    if(cover!=NULL) delete cover;
    //if(blinkTimer!=NULL) delete blinkTimer;
    ui->Result->setText("");
    ui->MenuButton->show();
    ui->NewGameButton->show();
    ui->CheckButton->show();
    ui->AnswerButton->show();
}
void MainWindow::clearAnswer() {
    for(int i=0;i<9;++i) {
        for(int j=0;j<9;++j) {
            if(sudoku[i][j]==0) cell[i][j]->setText("");
            if(cell[i][j]->beingSelected()) cell[i][j]->deselected();
            if(cell[i][j]->beingQuested()) cell[i][j]->quest();
        }
    }
    ui->centralWidget->setFocus();
    return;
}
bool MainWindow::isInvalid(int a, int b, int su[][9]) {
    int i, j, x, y;
    for(i=0;i<9;i++) {
        if(i!=a && su[a][b]==su[i][b]) return true;
        else if(i!=b && su[a][b]==su[a][i]) return true;
    }
    for(i=0;i<3;i++) {
        for(j=0;j<3;j++) {
            x=a/3*3+i;
            y=b/3*3+j;
            if(x!=a && y!=b && su[a][b]==su[x][y]) return true;
        }
    }
    return false;
}
bool MainWindow::isNotSolvable(int total) {
    int amountOfBlank[27];
    int *blank[81-total];
    createBlankPointers(blank, amountOfBlank, total);
    return !canBeFilled(blank, 0, total);
}
bool MainWindow::canBeFilled(int *blank[], int n, int total) {
    int a, b, *address=&ansSudoku[0][0];
    if(n==81-total) return true;
    for(int num=1;num<=9;num++) {
        *blank[n]=num;
        a=(blank[n]-address)/9;
        b=(blank[n]-address)%9;
        if(!isInvalid(a, b, ansSudoku))
            if(canBeFilled(blank, n+1, total)) return true;
    }
    *blank[n]=10;
    return false;
}
void MainWindow::createBlankPointers(int *blank[], int amountOfBlank[], int total) {
    int i, x, y, n=0, k;
    while(n<81-total) {
        blankCalculate(amountOfBlank);
        k=smallest(amountOfBlank,27);
        if(k<9) { //smallest amoutOfBlank is col
            for(i=0;i<9;++i) {
                if(ansSudoku[k][i]==0) {
                    blank[n]=&ansSudoku[k][i];
                    *blank[n]=10; //this is to mark this blank
                    ++n;
                }
            }
        } else if(k<18) {
            for(i=0;i<9;++i) {
                if(ansSudoku[i][k-9]==0) {
                    blank[n]=&ansSudoku[i][k-9];
                    *blank[n]=10; //this is to mark this blank
                    ++n;
                }
            }
        } else if(k<21) {
            for(x=3*(k-18);x<3*(k-18)+3;++x) {
                for(y=0;y<3;++y) {
                    if(ansSudoku[x][y]==0) {
                        blank[n]=&ansSudoku[x][y];
                        *blank[n]=10; //this is to mark this blank
                        ++n;
                    }
                }
            }
        } else if(k<24) {
            for(x=3*(k-21);x<3*(k-21)+3;++x) {
                for(y=3;y<6;++y) {
                    if(ansSudoku[x][y]==0) {
                        blank[n]=&ansSudoku[x][y];
                        *blank[n]=10; //this is to mark this blank
                        ++n;
                    }
                }
            }
        } else {
            for(x=3*(k-24);x<3*(k-24)+3;++x) {
                for(y=6;y<9;++y) {
                    if(ansSudoku[x][y]==0) {
                        blank[n]=&ansSudoku[x][y];
                        *blank[n]=10; //this is to mark this blank
                        ++n;
                    }
                }
            }
        }
    }
}
void MainWindow::blankCalculate(int amountOfBlank[]) {
    int i, j, x, y;
    for(i=0;i<27;i++) {
        amountOfBlank[i]=0;
        if(i<9) { //col
            for(j=0;j<9;j++) {
                if(ansSudoku[i][j]==0) amountOfBlank[i]++;
            }
        } else if(i<18) { //row
            for(j=0;j<9;j++) {
                if(ansSudoku[j][i-9]==0) amountOfBlank[i]++;
            }
        } else if(i<21){ //block
            for(x=3*(i-18);x<3*(i-18)+3;x++) {
                for(y=0;y<3;y++) {
                    if(ansSudoku[x][y]==0) amountOfBlank[i]++;
                }
            }
        } else if(i<24){ //block
            for(x=3*(i-21);x<3*(i-21)+3;x++) {
                for(y=3;y<6;y++) {
                    if(ansSudoku[x][y]==0) amountOfBlank[i]++;
                }
            }
        } else { //block
            for(x=3*(i-24);x<3*(i-24)+3;x++) {
                for(y=6;y<9;y++) {
                    if(ansSudoku[x][y]==0) amountOfBlank[i]++;
                }
            }
        }
    }
    return;
}
int MainWindow::smallest(int array[], int x) {
    int index=0, value=9;
    for(int i=0;i<x;i++) {
        if(array[i]>0 && array[i]<value) {
            value = array[i];
            index = i;
        }
    }
    return index;
}
void MainWindow::changeColorTheme() {
    static int theme_color=0;
    ++theme_color;
    switch (theme_color) {
    case 1:
        ui->ColorButton->setText("Yellow");
        ui->centralWidget->setStyleSheet("QWidget{background-color: #EFF28A;}"
                                         "QPushButton,QSpinBox{"
                                         "color: #BABD6A;"
                                         "background: transparent;"
                                         "border: 5px solid #BABD6A;"
                                         "}"
                                         "QPushButton:hover,QSpinBox:hover {background: #F3F5B6;}"
                                         "QGroupBox {border: 2px solid #BABD6A;}"
                                         "QLabel {"
                                         "color: #A4A75B;"
                                         "background:#EFF28A;"
                                         "}"
                                         "QTextBrowser {"
                                         "color: #A4A75B;"
                                         "background: transparent;"
                                         "border: 5px solid #A4A75B;"
                                         "}");
        break;
    case 2:
        ui->ColorButton->setText("Green");
        ui->centralWidget->setStyleSheet("QWidget{background-color: #8AF2A7;}"
                                         "QPushButton,QSpinBox{"
                                         "color: #6EC085;"
                                         "background: transparent;"
                                         "border: 5px solid #6EC085;"
                                         "}"
                                         "QPushButton:hover,QSpinBox:hover {background: #B5F5C7;}"
                                         "QGroupBox {border: 2px solid #6EC085;}"
                                         "QLabel {"
                                         "color: #5AA770;"
                                         "background:#8AF2A7;"
                                         "}"
                                         "QTextBrowser {"
                                         "color: #5AA770;"
                                         "background: transparent;"
                                         "border: 5px solid #5AA770;"
                                         "}");
        break;
    case 3:
        ui->ColorButton->setText("Purple");
        ui->centralWidget->setStyleSheet("QWidget{background-color: #D88AF2;}"
                                         "QPushButton,QSpinBox{"
                                         "color: #AB6FBF;"
                                         "background: transparent;"
                                         "border: 5px solid #AB6FBF;"
                                         "}"
                                         "QPushButton:hover,QSpinBox:hover {background: #E5B5F5;}"
                                         "QGroupBox {border: 2px solid #AB6FBF;}"
                                         "QLabel {"
                                         "color: #8E5C9E;"
                                         "background:#D88AF2;"
                                         "}"
                                         "QTextBrowser {"
                                         "color: #8E5C9E;"
                                         "background: transparent;"
                                         "border: 5px solid #8E5C9E;"
                                         "}");
        break;
    case 4:
        ui->ColorButton->setText("Blue");
        ui->centralWidget->setStyleSheet("QWidget{background-color: #8AE4F2;}"
                                         "QPushButton,QSpinBox{"
                                         "color: #6EB4BF;"
                                         "background: transparent;"
                                         "border: 5px solid #6EB4BF;"
                                         "}"
                                         "QPushButton:hover,QSpinBox:hover {background: #B6EDF5;}"
                                         "QGroupBox {border: 2px solid #6EB4BF;}"
                                         "QLabel {"
                                         "color: #5C959E;"
                                         "background:#8AE4F2;"
                                         "}"
                                         "QTextBrowser {"
                                         "color: #5C959E;"
                                         "background: transparent;"
                                         "border: 5px solid #5C959E;"
                                         "}");
        theme_color = 0;
        break;
    }
}
