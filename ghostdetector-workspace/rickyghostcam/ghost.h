#ifndef GHOST_H
#define GHOST_H

#include <QWidget>

class GhostPhoto : public QWidget
{
    Q_OBJECT

public:
    GhostPhoto(QWidget *parent = nullptr);
    ~GhostPhoto();
};
#endif // GHOST_H
