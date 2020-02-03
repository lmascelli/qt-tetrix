#include "tetrix.h"

enum TetrixShape {
  NoShape,
  ZShape,
  SShape,
  LineShape,
  TShape,
  ShareShape,
  LShape,
  MirroredLShape
};

class TetrixPiece {
public:
  TetrixPiece() { setShape(NoShape); }

  void setRandomShape(){
      setShape(TetrixShape(QRandomGenerator::global()->bounded(7)+1));
  }
  void setShape(TetrixShape shape);

  TetrixShape shape() const { return pieceShape; }
  int x(int index) const { return coords[index][0]; }
  int y(int index) const { return coords[index][1]; }
  int minX() const;
  int minY() const;
  int maxX() const;
  int maxY() const;
  TetrixPiece rotatedLeft() const;
  TetrixPiece rotatedRight() const;

private:
  void setX(int index, int x) { coords[index][0] = x; }
  void setY(int index, int y) { coords[index][1] = y; }
  TetrixShape pieceShape;
  int coords[4][2];
};

Tetrix::Tetrix(QWidget *parent) : QWidget(parent) {
  scoreLcd = new QLCDNumber(5);
  levelLcd = new QLCDNumber(QLCDNumber::Filled);
  linesLcd = new QLCDNumber;
  QGridLayout *layout = new QGridLayout;
  startButton = new QPushButton("&Start");
  quitButton = new QPushButton("&Quit");
  pauseButton = new QPushButton("&Pause");

  layout->addWidget(createLabel("NEXT"), 0, 0);
  //  layout->addWidget(nextPieceLabel, 1, 0);
  layout->addWidget(createLabel("LEVEL"));
  layout->addWidget(levelLcd, 3, 0);
  layout->addWidget(startButton, 4, 0);
  layout->addWidget(createLabel("SCORE"), 0, 2);
  layout->addWidget(scoreLcd, 1, 2);
  layout->addWidget(createLabel("LINES REMOVED"), 2, 2);
  layout->addWidget(linesLcd, 3, 2);
  layout->addWidget(quitButton, 4, 2);
  layout->addWidget(pauseButton, 5, 2);
  setLayout(layout);

  setWindowTitle("Tetrix");
  resize(550, 370);
}

Tetrix::~Tetrix() {}

QLabel *Tetrix::createLabel(const QString &text) {
  QLabel *label = new QLabel(text);
  //    label->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  return label;
}
