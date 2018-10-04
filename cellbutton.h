#ifndef CELLBUTTON_H
#define CELLBUTTON_H

#include <QPushButton>


class CellButton : public QPushButton
{
    Q_OBJECT
public:
    CellButton(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *e);
    void setChangeable(bool can);
    bool beingSelected() const;
    bool beingQuested() const;
signals:
    void selectLeft();
    void selectRight();
    void selectUp();
    void selectDown();
public slots:
    void selected();
    void deselected();
    void quest();
private:
    bool isSelected;
    bool isChangeable;
    bool isQuested;
};

#endif // CELLBUTTON_H
