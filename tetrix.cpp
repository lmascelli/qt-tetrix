#include "tetrix.h"

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
  layout->addWidget(nextPieceLabel, 1, 0);
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


Tetrix::~Tetrix() {}

QLabel *Tetrix::createLabel(const QString &text) {
    QLabel *label = new QLabel(text);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    return label;
}
