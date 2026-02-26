#ifndef SOUNDALARMACTION_H
#define SOUNDALARMACTION_H

#include "ialarmaction.h"
#include <QString>

class SoundAlarmAction : public IAlarmAction
{
public:
    explicit SoundAlarmAction(const QString &soundPath = QString());

    void execute() override;
    void setSoundPath(const QString &soundPath);

private:
    QString soundPath;
};

#endif // SOUNDALARMACTION_H
