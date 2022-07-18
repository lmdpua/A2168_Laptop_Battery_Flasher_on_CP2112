#include "progress.h"

Progress::Progress()
{
    m_progress = 0;
}

quint8 Progress::getProgress() const
{
    return m_progress;
}

void Progress::setProgress(int value)
 {
     if (value != m_progress) {
         m_progress = value;
         emit progressChanged(value);
     }
 }
