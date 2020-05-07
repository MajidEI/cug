/*#pragma once

#include <QTimer>

#include "algorithmes.h"

class SurInterface;


namespace CUG
{

class GestionnaireAlgos : public QObject
{
    Q_OBJECT
public:
    GestionnaireAlgos(SurInterface* w);

    void gerAlgorithme(Algorithmes *algo);
    void stopAlgorithme();

public slots:
    void removeHighlight();

private slots:
    void highlightElement();

private:
    SurInterface* surinterface;
    Algorithmes* algorithme;

    QTimer* timer;

    QString highlightPenColor = "green";
    QString highlightBrushColor = "light green";
};

}
*/
