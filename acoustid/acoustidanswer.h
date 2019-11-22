#ifndef ACOUSTIDANSWER_H
#define ACOUSTIDANSWER_H

#include <QDomDocument>
#include <QDebug>

class AcoustIdAnswer : public QObject
{
    Q_OBJECT

public:
    explicit AcoustIdAnswer(QObject *parent = Q_NULLPTR);

    bool isValid() const;

    bool setContent(const QByteArray &text);
    QString content() const;

    // acoustid information
    int countResults() const;
    double score() const;
    QString trackId() const;

    // get musicbrainz information
    int duration() const;
    QString title() const;

private:
    QDomDocument doc;
    QHash<QString, QDomNode> resultsByScore() const;
    QDomNode bestResult() const;
    QDomNode recording() const;
    double scoreFromNode(const QDomNode &node) const;
};

#endif // ACOUSTIDANSWER_H
