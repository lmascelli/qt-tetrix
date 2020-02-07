#include "tetrix.h"

void TetrixPiece::setRandomShape() {
  setShape(TetrixShape(QRandomGenerator::global()->bounded(7) + 1));
}

void TetrixPiece::setShape(TetrixShape shape) {
  static constexpr int coordsTable[8][4][2] = {
      // clang-format off
        {{0,  0},  {0,  0},  {0, 0},  {0,  0}},
        {{0, -1},  {0,  0},  {-1, 0}, {-1, 1}},
        {{0, -1},  {0,  0},  {1, 0},  {1,  1}},
        {{0, -1},  {0,  0},  {0, 1},  {0,  2}},
        {{-1, 0},  {0,  0},  {1, 0},  {0,  1}},
        {{0,  0},  {1,  0},  {0, 1},  {1,  1}},
        {{-1, -1}, {0, -1},  {0, 0},  {0,  1}},
        {{1, -1},  {0, -1},  {0, 0},  {0,  1}}};

  // clang-format on

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; ++j)
      coords[i][j] = coordsTable[shape][i][j];
  }
  pieceShape = shape;
}

int TetrixPiece::x(int index) const { return coords[index][0]; }

int TetrixPiece::y(int index) const { return coords[index][1]; }

int TetrixPiece::minX() const {
  int min = coords[0][0];
  for (int i = 1; i < 4; ++i)
    min = qMin(min, coords[i][0]);
  return min;
}

int TetrixPiece::minY() const {
  int min = coords[0][1];
  for (int i = 1; i < 4; ++i)
    min = qMin(min, coords[i][1]);
  return min;
}

int TetrixPiece::maxX() const {
  int max = coords[0][0];
  for (int i = 1; i < 4; ++i)
    max = qMax(max, coords[i][0]);
  return max;
}

int TetrixPiece::maxY() const {
  int max = coords[0][1];
  for (int i = 1; i < 4; ++i) {
    max = qMax(max, coords[i][1]);
  }
  return max;
}

TetrixPiece TetrixPiece::rotatedLeft() const {
  if (pieceShape == SquareShape)
    return *this;

  TetrixPiece result;
  result.pieceShape = pieceShape;
  for (int i = 0; i < 4; ++i) {
    result.setX(i, y(i));
    result.setY(i, -x(i));
  }
  return result;
}

TetrixPiece TetrixPiece::rotatedRight() const {
  if (pieceShape == SquareShape)
    return *this;

  TetrixPiece result;
  result.pieceShape = pieceShape;
  for (int i = 0; i < 4; ++i) {
    result.setX(i, -y(i));
    result.setY(i, x(i));
  }
  return result;
}

TetrixBoard::TetrixBoard(QWidget *parent)
    : QFrame(parent), isStarted(false), isPaused(false) {
  setFrameStyle(QFrame::Panel | QFrame::Sunken);
  setFocusPolicy(Qt::StrongFocus);
  clearBoard();

  nextPiece.setRandomShape();
}

void TetrixBoard::setNewPieceLabel(QLabel *piece) { nextPieceLabel = piece; }

QSize TetrixBoard::sizeHint() const {
  return QSize(BoardWidth * 15 + frameWidth() * 2,
               BoardHeight * 15 + frameWidth() * 2);
}

QSize TetrixBoard::minimumSizeHint() const {
  return QSize(BoardWidth * 5 + frameWidth() * 2,
               BoardHeight * 5 + frameWidth() * 2);
}

void TetrixBoard::start() {
  if (isPaused)
    return;
  isStarted = true;
  isWaitingAfterLine = false;
  numLinesRemoved = 0;
  numPiecesDropped = 0;
  score = 0;
  level = 1;
  clearBoard();

  emit linesRemovedChanged(numLinesRemoved);
  emit scoreChanged(score);
  emit levelChanged(level);

  newPiece();
  timer.start(timeoutTime(), this);
}

void TetrixBoard::pause() {
  if (!isStarted)
    return;
  isPaused = !isPaused;
  if (isPaused) {
    timer.stop();
  } else {
    timer.start(timeoutTime(), this);
  }
  update();
}

void TetrixBoard::paintEvent(QPaintEvent *event) {
  QFrame::paintEvent(event);

  QPainter painter(this);
  QRect rect = contentsRect();
  int boardTop = rect.bottom() - BoardHeight * squareHeight();

  for (int i = 0; i < BoardHeight; ++i) {
    for (int j = 0; j < BoardWidth; ++j) {
      TetrixShape shape = shapeAt(j, BoardHeight - i - 1);
      if (shape != NoShape) {
        drawSquare(painter, rect.left() + j * squareWidth(),
                   boardTop + i * squareHeight(), shape);
      }
    }
  }
  if (curPiece.shape() != NoShape) {
    for (int i = 0; i < 4; ++i) {
      int x = curX + curPiece.x(i);
      int y = curY - curPiece.y(i);
      drawSquare(painter, rect.left() + x * squareWidth(),
                 boardTop + (BoardHeight - y - 1) * squareHeight(),
                 curPiece.shape());
    }
  }
}

void TetrixBoard::clearBoard() {
  for (int i = 0; i < BoardWidth * BoardHeight; ++i)
    board[i] = NoShape;
}

void TetrixBoard::newPiece() {
  curPiece = nextPiece;
  nextPiece.setRandomShape();
  showNextPiece();
  curX = BoardWidth / 2 + 1;
  curY = BoardHeight - 1 + curPiece.minY();

  if (!tryMove(curPiece, curX, curY)) {
    curPiece.setShape(NoShape);
    timer.stop();
    isStarted = false;
  }
}

void TetrixBoard::showNextPiece() {
  if (!nextPieceLabel)
    return;
  int dx = nextPiece.maxX() - nextPiece.minX() + 1;
  int dy = nextPiece.maxY() - nextPiece.minY() + 1;

  QPixmap pixmap(dx * squareWidth(), dy * squareHeight());
  QPainter painter(&pixmap);
  painter.fillRect(pixmap.rect(), nextPieceLabel->palette().window());

  for (int i = 0; i < 4; ++i) {
    int x = nextPiece.x(i) - nextPiece.minX();
    int y = nextPiece.y(i) - nextPiece.minY();
    drawSquare(painter, x * squareWidth(), y * squareHeight(),
               nextPiece.shape());
  }
  nextPieceLabel->setPixmap(pixmap);
}

bool TetrixBoard::tryMove(const TetrixPiece &newPiece, int newX, int newY) {
  for (int i = 0; i < 4; ++i) {
    int x = newX + newPiece.x(i);
    int y = newY - newPiece.y(i);
    if (x < 0 || x >= BoardWidth || y < 0 || y >= BoardHeight) {
      return false;
    }
    if (shapeAt(x, y) != NoShape)
      return false;
  }
  curPiece = newPiece;
  curX = newX;
  curY = newY;
  update();
  return true;
}

void TetrixBoard::drawSquare(QPainter &painter, int x, int y,
                             TetrixShape shape) {
  static constexpr QRgb colorTable[8] = {0x000000, 0xCC3333, 0x33CC33,
                                         0x3333CC, 0xCCCC33, 0xCC33CC,
                                         0x33CCCC, 0xDAAA00};

  QColor color = colorTable[int(shape)];
  painter.fillRect(x + 1, y + 1, squareWidth() - 2, squareHeight() - 2, color);
  painter.setPen(color.lighter());
  painter.drawLine(x, y + squareHeight() - 1, x, y);
  painter.drawLine(x, y, x + squareWidth() - 1, y);
  painter.setPen(color.darker());
  painter.drawLine(x + 1, y + squareHeight() - 1, x + squareWidth() - 1,
                   y + squareHeight() - 1);
  painter.drawLine(x + squareWidth() - 1, y + squareHeight() - 1,
                   x + squareWidth() - 1, y + 1);
  ;
}

Tetrix::Tetrix(QWidget *parent) : QWidget(parent), board(new TetrixBoard) {
  nextPieceLabel = new QLabel;
  nextPieceLabel->setFrameStyle(QFrame::Box | QFrame::Raised);
  nextPieceLabel->setAlignment(Qt::AlignCenter);
  board->setNewPieceLabel(nextPieceLabel);
  scoreLcd = new QLCDNumber(5);
  levelLcd = new QLCDNumber(QLCDNumber::Filled);
  linesLcd = new QLCDNumber;
  startButton = new QPushButton("&Start");
  quitButton = new QPushButton("&Quit");
  pauseButton = new QPushButton("&Pause");

  connect(startButton, &QPushButton::clicked, board, &TetrixBoard::start);
  connect(quitButton, &QPushButton::clicked, qApp, &QCoreApplication::quit);
  connect(pauseButton, &QPushButton::clicked, board, &TetrixBoard::pause);
  connect(board, &TetrixBoard::scoreChanged, scoreLcd,
          qOverload<int>(&QLCDNumber::display));
  connect(board, &TetrixBoard::levelChanged, levelLcd,
          qOverload<int>(&QLCDNumber::display));
  connect(board, &TetrixBoard::linesRemovedChanged, linesLcd,
          qOverload<int>(&QLCDNumber::display));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createLabel("NEXT"), 0, 0);
  //  layout->addWidget(nextPieceLabel, 1, 0);
  layout->addWidget(createLabel("LEVEL"));
  layout->addWidget(levelLcd, 3, 0);
  layout->addWidget(startButton, 4, 0);
  layout->addWidget(board, 0, 1, 6, 1);
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

void TetrixBoard::keyPressEvent(QKeyEvent *event) {
  if (!isStarted || isPaused || curPiece.shape() == NoShape) {
    QFrame::keyPressEvent(event);
    return;
  }
  switch (event->key()) {
  case Qt::Key_Left:
    tryMove(curPiece, curX - 1, curY);
    break;
  case Qt::Key_Right:
    tryMove(curPiece, curX + 1, curY);
    break;
  case Qt::Key_Down:
    tryMove(curPiece.rotatedRight(), curX, curY);
    break;
  case Qt::Key_Up:
    tryMove(curPiece.rotatedLeft(), curX, curY);
    break;
  case Qt::Key_Space:
    dropDown();
    break;
  default:
    QFrame::keyPressEvent(event);
  }
}

void TetrixBoard::dropDown() {
  int dropHeight = 0;
  int newY = curY;
  while (newY > 0) {
    if (!tryMove(curPiece, curX, newY - 1))
      break;
    --newY;
    ++dropHeight;
  }
  pieceDropped(dropHeight);
}

void TetrixBoard::pieceDropped(int dropHeight) {
  for (int i = 0; i < 4; ++i) {
    int x = curX + curPiece.x(i);
    int y = curY - curPiece.y(i);
    shapeAt(x, y) = curPiece.shape();

    ++numPiecesDropped;
    if (numPiecesDropped % 25 == 0) {
      ++level;
      timer.start(timeoutTime(), this);
      emit levelChanged(level);
    }

    score += dropHeight + 7;
    emit scoreChanged(score);
    removeFullLines();

    if (!isWaitingAfterLine)
      newPiece();
  }
}

void TetrixBoard::removeFullLines() {
  int numFullLines = 0;

  for (int i = BoardHeight - 1; i >= 0; i--) {
    bool lineIsFull = true;

    for (int j = 0; j < BoardWidth; ++j) {
      if (shapeAt(j, i) == NoShape) {
        lineIsFull = false;
        break;
      }
    }

    if (lineIsFull) {
      ++numFullLines;
      for (int k = i; k < BoardHeight - 1; ++k) {
        for (int j = 0; j < BoardWidth; ++j) {
          shapeAt(j, k) = shapeAt(j, k + 1);
        }
      }
      for (int j = 0; j < BoardWidth; ++j)
        shapeAt(j, BoardHeight - 1) = NoShape;
    }
  }
  if (numFullLines > 0) {
    numLinesRemoved += numFullLines;
    score += 10 * numFullLines;
    emit linesRemovedChanged(numLinesRemoved);
    emit scoreChanged(score);

    timer.start(500, this);
    isWaitingAfterLine = true;
    curPiece.setShape(NoShape);
    update();
  }
}

void TetrixBoard::timerEvent(QTimerEvent *event) {
  if (event->timerId() == timer.timerId()) {
    if (isWaitingAfterLine) {
      isWaitingAfterLine = false;
      newPiece();
      timer.start(timeoutTime(), this);
    } else {
      oneLineDown();
      QMessageBox::warning(this->parentWidget(), "Warning", "One line down.");
    }
  } else {
    QFrame::timerEvent(event);
  }
}

void TetrixBoard::oneLineDown() {
  if (!tryMove(curPiece, curX, curY - 1))
    pieceDropped(0);
}

Tetrix::~Tetrix() {}

QLabel *Tetrix::createLabel(const QString &text) {
  QLabel *label = new QLabel(text);
  //    label->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  return label;
}
