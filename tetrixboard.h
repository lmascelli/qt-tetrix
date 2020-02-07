#ifndef TETRIXBOARD_H
#define TETRIXBOARD_H

#include "tetrixpiece.h"




#include <QPainter>
#include <QPointer>
#include <QBasicTimer>
#include <QLabel>

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
#endif // TETRIXBOARD_H
