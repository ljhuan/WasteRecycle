#ifndef CPRICESET_H
#define CPRICESET_H

#include <QObject>
#include <QWidget>

class CPriceSet : public QWidget
{
    Q_OBJECT
public:
    explicit CPriceSet(QWidget *parent = nullptr);

    void view();

signals:

public slots:
};

#endif // CPRICESET_H
