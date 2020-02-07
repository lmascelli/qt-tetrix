#ifndef TETRIX_H
#define TETRIX_H

#include <QtWidgets>
#include "tetrixboard.h"

class Tetrix : public QWidget {
  Q_OBJECT

public:
  Tetrix(QWidget *parent = nullptr);
  ~Tetrix();

private:
  QLabel *createLabel(const QString &text);

  TetrixBoard *board;
  QLabel *nextPieceLabel;
  QLCDNumber *scoreLcd;
  QLCDNumber *levelLcd;
  QLCDNumber *linesLcd;
  QPushButton *startButton;
  QPushButton *quitButton;
  QPushButton *pauseButton;
};
#endif // TETRIX_H
