#ifndef TETRIX_H
#define TETRIX_H

#include <QtWidgets>

class Tetrix : public QWidget
{
    Q_OBJECT

public:
    Tetrix(QWidget *parent = nullptr);
    ~Tetrix();

private:
    QLabel *title;
};
#endif // TETRIX_H
