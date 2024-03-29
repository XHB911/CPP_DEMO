#ifndef PLAYSCENE_H
#define PLAYSCENE_H

#include <QMainWindow>
#include "mycoin.h"

class PlayScene : public QMainWindow
{
    Q_OBJECT
public:
//    explicit PlayScene(QWidget *parent = nullptr);

    PlayScene(int levelNum);

    int levelIndex;

    void paintEvent(QPaintEvent *e);

    int gameArrary[4][4];
    bool isWin;

    mycoin *coinBtn[4][4];

signals:
    void chooseSceneBack();
};

#endif // PLAYSCENE_H
