#include "alarmmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>

static const char* kDefaultSound = "qrc:/s/resources/sounds/soundalarm.wav";

AlarmManager::AlarmManager(QObject *parent)
    : QObject(parent)
{
    connect(&checkTimer, &QTimer::timeout, this, &AlarmManager::checkAlarms);
    checkTimer.start(1000);
}

void AlarmManager::addAlarm(const AlarmData &data)
{
    AlarmData a = data;
    a.nextTrigger = computeInitialTrigger(a.time);
    alarms.append(a);
    emit alarmsUpdated();
}

void AlarmManager::removeAlarm(int index)
{
    if (index >= 0 && index < alarms.size()) {
        alarms.removeAt(index);
        emit alarmsUpdated();
    }
}

void AlarmManager::toggleAlarm(int index)
{
    if (index >= 0 && index < alarms.size()) {
        alarms[index].enabled = !alarms[index].enabled;
        emit alarmsUpdated();
    }
}

QList<AlarmData> AlarmManager::getAlarms() const
{
    return alarms;
}

QDateTime AlarmManager::computeInitialTrigger(const QTime &t)
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime trigger(now.date(), t);
    if (trigger <= now)
        trigger = trigger.addDays(1);
    return trigger;
}

QDateTime AlarmManager::computeNextTrigger(const AlarmData &a, const QDateTime &after)
{
    QDate d = after.date();
    for (int i = 0; i < 7; ++i) {
        d = d.addDays(1);
        int dow = d.dayOfWeek();
        QString day;
        switch (dow) {
        case 1: day = "Mon"; break;
        case 2: day = "Tue"; break;
        case 3: day = "Wed"; break;
        case 4: day = "Thu"; break;
        case 5: day = "Fri"; break;
        case 6: day = "Sat"; break;
        case 7: day = "Sun"; break;
        }
        if (a.repeatMode == "Every day" ||
            (a.repeatMode == "Weekdays" && dow <= 5) ||
            (a.repeatMode == "Weekends" && dow >= 6) ||
            (a.repeatMode == "Specific days" && a.days.contains(day)))
            return QDateTime(d, a.time);
    }
    return QDateTime(after.date().addDays(1), a.time);
}

void AlarmManager::checkAlarms()
{
    const QDateTime now = QDateTime::currentDateTime();

    for (AlarmData &a : alarms) {
        if (!a.enabled)
            continue;

        if (a.time.hour() == now.time().hour() &&
            a.time.minute() == now.time().minute() &&
            now.time().second() == 0)
        {
            QSoundEffect *effect = new QSoundEffect(this);
            const QString src = a.soundPath.isEmpty()
                                    ? "qrc:/s/resources/sounds/soundalarm.wav"
                                    : a.soundPath;
            effect->setSource(QUrl(src));
            effect->setVolume(1.0);
            effect->play();

            QMessageBox msg;
            msg.setWindowTitle("Alarm");
            msg.setText(QString("⏰ %1").arg(a.name.isEmpty() ? "Alarm" : a.name));

            if (a.snooze) {
                msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msg.setButtonText(QMessageBox::Yes, "Snooze 5 min");
                msg.setButtonText(QMessageBox::No, "Stop");
            } else {
                msg.setStandardButtons(QMessageBox::Ok);
                msg.setButtonText(QMessageBox::Ok, "Stop");
            }

            int res = msg.exec();

            if (effect->isPlaying()) {
                effect->stop();
            }
            effect->deleteLater();

            if (a.snooze && res == QMessageBox::Yes) {
                a.time = QTime::currentTime().addSecs(5 * 60);
                a.enabled = true;
            }
            else {
                if (a.repeatMode == "Never" || a.repeatMode.toLower() == "once") {
                    a.enabled = false;
                } else {
                    QTime t = a.time.addSecs(24 * 60 * 60);
                    if (t.isValid())
                        a.time = t;
                    a.enabled = true;
                }
            }

            emit alarmsUpdated();
        }
    }
}

void AlarmManager::saveToFile(const QString &path)
{
    QJsonArray arr;
    for (const auto &a : alarms) {
        QJsonObject o;
        o["name"] = a.name;
        o["time"] = a.time.toString("HH:mm:ss");
        o["repeatMode"] = a.repeatMode;
        o["days"] = QJsonArray::fromStringList(a.days);
        o["soundPath"] = a.soundPath;
        o["snooze"] = a.snooze;
        o["enabled"] = a.enabled;
        o["nextTrigger"] = a.nextTrigger.toString(Qt::ISODate);
        arr.append(o);
    }
    QJsonDocument doc(arr);
    QFile f(path);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(doc.toJson());
        f.close();
    }
}

void AlarmManager::loadFromFile(const QString &path)
{
    QFile f(path);
    if (!f.exists() || !f.open(QIODevice::ReadOnly)) return;
    const auto doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    alarms.clear();
    if (!doc.isArray()) return;

    for (const auto &v : doc.array()) {
        QJsonObject o = v.toObject();
        AlarmData a;
        a.name = o["name"].toString();
        a.time = QTime::fromString(o["time"].toString(), "HH:mm:ss");
        a.repeatMode = o["repeatMode"].toString();
        for (const auto &d : o["days"].toArray())
            a.days << d.toString();
        a.soundPath = o["soundPath"].toString();
        a.snooze = o["snooze"].toBool();
        a.enabled = o["enabled"].toBool();
        a.nextTrigger = QDateTime::fromString(o["nextTrigger"].toString(), Qt::ISODate);
        if (!a.nextTrigger.isValid())
            a.nextTrigger = computeInitialTrigger(a.time);
        alarms.append(a);
    }
    emit alarmsUpdated();
}
