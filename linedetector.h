#ifndef LINEDETECTOR_H
#define LINEDETECTOR_H

#include <QObject>

class LineDetector : public QObject
{
    Q_OBJECT
public:
    explicit LineDetector(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LINEDETECTOR_H
