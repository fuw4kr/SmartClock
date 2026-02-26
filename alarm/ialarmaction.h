#ifndef IALARMACTION_H
#define IALARMACTION_H

class IAlarmAction
{
public:
    virtual ~IAlarmAction() = default;
    virtual void execute() = 0;
};

#endif // IALARMACTION_H
