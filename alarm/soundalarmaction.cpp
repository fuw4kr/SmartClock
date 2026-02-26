/**
 * @file soundalarmaction.cpp
 * @brief Definitions for soundalarmaction.
 * @details Implements logic declared in the corresponding header for soundalarmaction.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "soundalarmaction.h"
#include <QObject>
#include <QSoundEffect>
#include <QTimer>
#include <QUrl>

static const char* kDefaultSound = "qrc:/s/resources/sounds/soundalarm.wav";

SoundAlarmAction::SoundAlarmAction(const QString &soundPath)
    : soundPath(soundPath)
{
}

void SoundAlarmAction::setSoundPath(const QString &soundPath)
{
    this->soundPath = soundPath;
}

void SoundAlarmAction::execute()
{
    auto *effect = new QSoundEffect;
    const QString src = soundPath.isEmpty()
                            ? QString::fromUtf8(kDefaultSound)
                            : soundPath;
    effect->setSource(QUrl(src));
    effect->setVolume(1.0);
    effect->play();
    QTimer::singleShot(3000, effect, &QObject::deleteLater);
}
