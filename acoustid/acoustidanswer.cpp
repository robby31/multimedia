#include "acoustidanswer.h"

AcoustIdAnswer::AcoustIdAnswer()
{
}

bool AcoustIdAnswer::isValid() const
{
    QDomNodeList l_nodes = elementsByTagName("status");
    if (l_nodes.count() == 1) {
        return l_nodes.at(0).toElement().text() == "ok";
    }

    return false;
}

int AcoustIdAnswer::countResults() const
{
    QDomNodeList l_nodes = elementsByTagName("result");
    return l_nodes.count();
}

QHash<QString, QDomNode> AcoustIdAnswer::resultsByScore() const
{
    QHash<QString, QDomNode> res;

    if (isValid()) {
        QDomNodeList l_nodes = elementsByTagName("result");
        for (int i=0; i<l_nodes.count(); ++i) {
            double score = scoreFromNode(l_nodes.at(i));
            res[QString("%1").arg(score)] = l_nodes.at(i);
        }
    }
    return res;
}

QDomNode AcoustIdAnswer::bestResult() const
{
    if (isValid()) {
        QHash<QString, QDomNode> results = resultsByScore();
        QList<double> scores;
        foreach (const QString &key, results.keys())
            scores.append(key.toDouble());
        std::sort(scores.begin(), scores.end(), qGreater<double>());
        QString bestScore = QString("%1").arg(scores.at(0));

        if (results.contains(bestScore))
            return results[bestScore];
    }

    // default value
    return QDomNode();
}

QDomNode AcoustIdAnswer::recording() const
{
    if (isValid()) {
        QDomNode node = bestResult();

        QDomNodeList l_nodes = node.toElement().elementsByTagName("recording");
        if (l_nodes.count() >= 1) {
            if (l_nodes.count() > 1)
                qWarning() << QString("%1 recordings found, returns only first one").arg(l_nodes.count());

            return l_nodes.at(0);
        } else {
            qWarning() << "No recording found";
            return QDomNode();
        }
    } else {
        // invalid answer
        return QDomNode();
    }
}

double AcoustIdAnswer::scoreFromNode(const QDomNode &node) const
{
    // search attribute id in direct children
    QDomNodeList l_children = node.toElement().childNodes();
    int nbChildren = l_children.count();
    for (int index=0; index<nbChildren; ++index)
        if (l_children.at(index).toElement().tagName()=="score")
            return l_children.at(index).toElement().text().toDouble();

    // default result
    return 0.0;
}

double AcoustIdAnswer::score() const
{
    if (isValid()) {
            return scoreFromNode(bestResult());
    } else {
        // invalid answer
        return 0.0;
    }
}

QString AcoustIdAnswer::trackId() const
{
    if (isValid()) {
            QDomNode node = bestResult();

            // search attribute id in direct children
            QDomNodeList l_children = node.toElement().childNodes();
            int nbChildren = l_children.count();
            for (int index=0;index<nbChildren;++index)
                if (l_children.at(index).toElement().tagName()=="id")
                    return l_children.at(index).toElement().text();

            return QString();
    } else {
        // invalid answer
        return QString();
    }
}

int AcoustIdAnswer::duration() const
{
    if (isValid()) {
            QDomNode node = recording();

            // search attribute id in direct children
            QDomNodeList l_children = node.toElement().childNodes();
            int nbChildren = l_children.count();
            for (int index=0;index<nbChildren;++index)
                if (l_children.at(index).toElement().tagName()=="duration")
                    return l_children.at(index).toElement().text().toInt();

            return 0;
    } else {
        // invalid answer
        return 0;
    }
}

QString AcoustIdAnswer::title() const
{
    if (isValid()) {
            QDomNode node = recording();

            // search attribute id in direct children
            QDomNodeList l_children = node.toElement().childNodes();
            int nbChildren = l_children.count();
            for (int index=0;index<nbChildren;++index)
                if (l_children.at(index).toElement().tagName()=="title")
                    return l_children.at(index).toElement().text();

            return QString();
    } else {
        // invalid answer
        return QString();
    }
}
