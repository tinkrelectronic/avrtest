#ifndef AVRDUDE_CONF_PARSER_H
#define AVRDUDE_CONF_PARSER_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QByteArray>

struct Programmer {
    QString id;
    QString desc;
    QString type;
    QString connection_type;
};

struct Memory {
    QString type;
    int size = 0;
    int page_size = 0;
};

struct MCU {
    QString id;
    QString desc;
    QByteArray signature; // 3 bytes
    QMap<QString, Memory> memories;
};

class AvrdudeConfParser {
public:
    bool parse(const QString& filePath);

    const QVector<Programmer>& programmers() const { return m_programmers; }
    const QVector<MCU>& mcus() const { return m_mcus; }
    const MCU* mcuBySignature(const QByteArray& sig) const;

private:
    QVector<Programmer> m_programmers;
    QVector<MCU> m_mcus;

    void resetCurrent();
    Programmer currentProg;
    MCU currentMcu;
    Memory currentMem;
    bool inProgrammer = false;
    bool inPart = false;
    bool inMemory = false;
};

#endif // AVRDUDE_CONF_PARSER_H