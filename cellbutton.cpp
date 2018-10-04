#include "cellbutton.h"
#include <QString>
#include <QDebug>
#include <QKeyEvent>

CellButton::CellButton(QWidget *parent)
    : QPushButton(parent)
{
    setStyleSheet("border-width: 2px;"
                  "border-style: solid;");
    setFont(QFont("Consolas",24));
    setCheckable(false);
    setAutoExclusive(false);
    isSelected=false;
    isChangeable=true;
    isQuested = false;
    show();
}
void CellButton::selected() {
    if(!isSelected) {
        //add red border
        setStyleSheet(styleSheet().append("border-color: #FF0F0F;"));
        //remove border width setting
        int from = styleSheet().indexOf("border-width"), to;
        if(from!=-1) {
            to = styleSheet().indexOf(";",from);
            setStyleSheet(styleSheet().remove(from,to-from+1));
        }
        //set border width to 5px
        setStyleSheet(styleSheet().append("border-width: 5px;"));
        //mark it
        isSelected=true;
        setFocus();
    } else {
        emit quest();
    }
    return;
}
void CellButton::quest() {
    if(isChangeable && !isQuested) {
        setText(text().append("?"));
        isQuested = true;
    } else if(isChangeable && isQuested) {
        QString str = text();
        str.chop(1);
        setText(str);
        isQuested = false;
    }
    return;
}
void CellButton::deselected() {
    //remove border color setting
    int from = styleSheet().indexOf("border-color"), to;
    if(from!=-1) {
        to = styleSheet().indexOf(";",from);
        setStyleSheet(styleSheet().remove(from,to-from+1));
    }
    //remove border width setting
    from = styleSheet().indexOf("border-width");
    if(from!=-1) {
        to = styleSheet().indexOf(";",from);
        setStyleSheet(styleSheet().remove(from,to-from+1));
    }
    //set border width to 2px
    setStyleSheet(styleSheet().append("border-width: 2px;"));
    //de-mark it
    isSelected=false;
    return;
}
void CellButton::keyPressEvent(QKeyEvent *e) {
    if(isSelected) {
        switch (e->key()) {
        case Qt::Key_1:
            if(isChangeable) setText("1");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_2:
            if(isChangeable) setText("2");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_3:
            if(isChangeable) setText("3");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_4:
            if(isChangeable) setText("4");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_5:
            if(isChangeable) setText("5");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_6:
            if(isChangeable) setText("6");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_7:
            if(isChangeable) setText("7");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_8:
            if(isChangeable) setText("8");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_9:
            if(isChangeable) setText("9");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_0:
        case Qt::Key_Backspace:
            if(isChangeable) setText("");
            if(isQuested) isQuested = false;
            break;
        case Qt::Key_Space:
            emit quest();
            break;
        case Qt::Key_A:
        case Qt::Key_Left:
            emit selectLeft();
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            emit selectRight();
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            emit selectUp();
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            emit selectDown();
            break;
        }
    }
    return;
}
void CellButton::setChangeable(bool can) {
    if(can) isChangeable=true;
    else isChangeable=false;
    return;
}
bool CellButton::beingSelected() const {
    return isSelected;
}
bool CellButton::beingQuested() const {
    return isQuested;
}
