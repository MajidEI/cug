/*#include "gestionnairedealgos.h"
#include "surinterface.h"

using namespace CUG;


GestionnaireAlgos::GestionnaireAlgos(SurInterface *w)
{
    surinterface = w;
    timer = new QTimer();
    algorithme = NULL;
}

void GestionnaireAlgos::gerAlgorithme(Algorithmes* algo)
{
    algorithme = algo;

    Algorithmes::Arguments args;
    args.graphe = surinterface->graphe;
    algorithme->execute(args);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(highlightElement()));
    timer->start(1000);
}

void GestionnaireAlgos::stopAlgorithme()
{
    if (timer->isActive())
        timer->stop();
}

void GestionnaireAlgos::highlightElement()
{
    if (!algorithme->emptySequence())
    {
        Algorithmes::GraphElement element = algorithme->getNextElement();
        if (element.first != -1)
        {
            SommetGraphics* SommetItem;
            SommetItem = surinterface->findSommetItem(surinterface->graphe->recupererSommet(element.first));
            QPen pen = SommetItem->pen();
            pen.setColor(highlightPenColor);
            SommetItem->setBrush(QBrush(QColor(highlightBrushColor)));
            SommetItem->setPen(pen);
        }
        else
        {
            AreteGraphics* areteItem;
            areteItem = surinterface->findAreteItem(surinterface->graphe->recupererArete(element.second));
            QPen pen = areteItem->pen();
            pen.setColor(highlightPenColor);
            areteItem->setPen(pen);
        }
    }
    else
        timer->stop();
}

void GestionnaireAlgos::removeHighlight()
{
    foreach (SommetGraphics* sommet, surinterface->getSommets())
    {
        sommet->setPen(sommet->getDefaultPen());
        sommet->setBrush(sommet->getDefaultBrush());
    }
    foreach (AreteGraphics* arete, surinterface->getAretes())
    {
        arete->setPen(arete->getDefaultPen());
    }
}



*/
