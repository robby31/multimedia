#ifndef ACOUSTIDANSWER_H
#define ACOUSTIDANSWER_H

#include <QDomDocument>
#include <QDebug>

class AcoustIdAnswer : public QDomDocument
{

public:
    AcoustIdAnswer() = default;

    bool isValid() const;

    // acoustid information
    int countResults() const;
    double score() const;
    QString trackId() const;

    // get musicbrainz information
    int duration() const;
    QString title() const;

private:
    QHash<QString, QDomNode> resultsByScore() const;
    QDomNode bestResult() const;
    QDomNode recording() const;
    double scoreFromNode(const QDomNode &node) const;
};

#endif // ACOUSTIDANSWER_H
