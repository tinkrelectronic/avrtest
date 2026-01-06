#include "avrdude_conf_parser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

bool AvrdudeConfParser::parse(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open avrdude.conf:" << filePath;
        return false;
    }

    QTextStream in(&file);
    QRegularExpression reAssignment(R"(^\s*(\w+)\s*=\s*"([^"]*)"\s*;?$)");
    QRegularExpression reMemory(R"(^memory\s+"([^"]+)"\s*$)");

    resetCurrent();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;

        if (line == "programmer") {
            inProgrammer = true;
            inPart = false;
            continue;
        }
        if (line.startsWith("part")) {
            if (inPart && !currentMcu.id.isEmpty())
                m_mcus.append(currentMcu);
            resetCurrent();
            inPart = true;
            inProgrammer = false;
            continue;
        }
        if (line == ";") {
            if (inProgrammer) {
                if (!currentProg.id.isEmpty())
                    m_programmers.append(currentProg);
                inProgrammer = false;
            } else if (inPart) {
                if (inMemory) {
                    currentMcu.memories[currentMem.type] = currentMem;
                    inMemory = false;
                }
                if (!currentMcu.id.isEmpty())
                    m_mcus.append(currentMcu);
                inPart = false;
            }
            resetCurrent();
            continue;
        }

        auto memMatch = reMemory.match(line);
        if (inPart && memMatch.hasMatch()) {
            if (inMemory)
                currentMcu.memories[currentMem.type] = currentMem;
            currentMem = Memory();
            currentMem.type = memMatch.captured(1);
            inMemory = true;
            continue;
        }

        auto match = reAssignment.match(line);
        if (match.hasMatch()) {
            QString key = match.captured(1);
            QString value = match.captured(2);

            if (inProgrammer) {
                if (key == "id") currentProg.id = value;
                else if (key == "desc") currentProg.desc = value;
                else if (key == "type") currentProg.type = value;
                else if (key == "connection_type") currentProg.connection_type = value;
            } else if (inPart) {
                if (inMemory) {
                    if (key == "size") currentMem.size = value.toInt(nullptr, 0);
                    else if (key == "page_size") currentMem.page_size = value.toInt(nullptr, 0);
                } else {
                    if (key == "id") currentMcu.id = value;
                    else if (key == "desc") currentMcu.desc = value;
                    else if (key == "signature") {
                        QStringList bytes = value.split(' ', Qt::SkipEmptyParts);
                        QByteArray sig;
                        for (const QString& b : bytes)
                            sig.append(static_cast<char>(b.toInt(nullptr, 0)));
                        if (sig.size() == 3) currentMcu.signature = sig;
                    }
                }
            }
        }
    }

    // Save last entry
    if (inProgrammer && !currentProg.id.isEmpty())
        m_programmers.append(currentProg);
    if (inPart) {
        if (inMemory)
            currentMcu.memories[currentMem.type] = currentMem;
        if (!currentMcu.id.isEmpty())
            m_mcus.append(currentMcu);
    }

    file.close();
    return true;
}

void AvrdudeConfParser::resetCurrent()
{
    currentProg = Programmer();
    currentMcu = MCU();
    currentMem = Memory();
    inMemory = false;
}

const MCU* AvrdudeConfParser::mcuBySignature(const QByteArray& sig) const
{
    for (const MCU& mcu : m_mcus) {
        if (mcu.signature.size() == 3 && mcu.signature == sig)
            return &mcu;
    }
    return nullptr;
}