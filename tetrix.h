#ifndef TETRIX_H
#define TETRIX_H

#include <QtWidgets>

enum TetrixShape {
  NoShape,
  ZShape,
  SShape,
  LineShape,
  TShape,
  SquareShape,
  LShape,
  MirroredLShape
};

class TetrixPiece {
public:
  TetrixPiece() { setShape(NoShape); }

  void setRandomShape();
  void setShape(TetrixShape shape);

  TetrixShape shape() const { return pieceShape; }
  int x(int index) const;
  int y(int index) const;
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

class TetrixBoard : public QFrame {

  Q_OBJECT

public:
  TetrixBoard(QWidget *parent = nullptr);

  void setNewPieceLabel(QLabel *piece);
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

public slots:
  void start();
  void pause();

signals:
  void scoreChanged(int score);
  void levelChanged(int level);
  void linesRemovedChanged(int numLines);

protected:
  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void timerEvent(QTimerEvent *event) override;

private:
  enum { BoardWidth = 10, BoardHeight = 22 };

  TetrixShape &shapeAt(int x, int y) { return board[(y * BoardWidth) + x]; }
  int timeoutTime() { return 1000 / (1 + level); }
  int squareWidth() { return contentsRect().width() / BoardWidth; }
  int squareHeight() { return contentsRect().height() / BoardHeight; }
  void clearBoard();
  void dropDown();
  void pieceDropped(int dropHeight);
  void removeFullLines();
  void newPiece();
  void showNextPiece();
  void oneLineDown();

  bool tryMove(const TetrixPiece &newPiece, int newX, int newY);
  void drawSquare(QPainter &painter, int x, int y, TetrixShape shape);

  bool isStarted;
  bool isPaused;
  bool isWaitingAfterLine;
  QPointer<QLabel> nextPieceLabel;
  TetrixPiece curPiece;
  TetrixPiece nextPiece;
  int curX;
  int curY;
  int numLinesRemoved;
  int numPiecesDropped;
  int score;
  int level;
  QBasicTimer timer;
  TetrixShape board[BoardWidth * BoardHeight];
};

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
