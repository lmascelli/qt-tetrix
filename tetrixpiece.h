#ifndef TETRIXPIECE_H
#define TETRIXPIECE_H

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
#endif // TETRIXPIECE_H
