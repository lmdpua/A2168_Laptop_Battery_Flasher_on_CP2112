#ifndef PROGRESS_H
#define PROGRESS_H

#include <QObject>

class Progress : public QObject
{
    Q_OBJECT

public:
    Progress(); //Конструктор по умолчанию

    quint8 getProgress() const;

public slots:
    void setProgress(int value);

signals:
    void progressChanged (int percent);

private:
    int m_progress;
};

#endif // PROGRESS_H
