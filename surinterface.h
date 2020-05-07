#pragma once

#include <QGraphicsView>
#include <QTimer>
#include <vector>
#include <list>
#include "graphe.h"
#include "arete.h"
#include "algorithmes.h"
#include "mainwindow.h"
#include "gestionnairedealgos.h"
#include "aretegraphics.h"
#include "sommetgraphics.h"
#include "sommet.h"


class SurInterface : public QGraphicsView
{
    Q_OBJECT
public:
    using SommetPair = QPair<SommetGraphics *, SommetGraphics *>;

    enum
    {
        defaultMode = 0,
        renommerMode,
        sommetCreationMode,
        pondererMode,
        areteCreationMode,
        arcCreationMode,
        deletionMode
    };

    SurInterface(QWidget* parent = 0);

    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent* event);
    void manageAreteCreation(const QPoint& location);
    void setElementContent(QGraphicsItem *item);
    void creerSommet(const QPoint& pos, const QString &idtf = "");
    void deleteElementAtPosition(const QPoint& pos);
    void deleteElement(QGraphicsItem* item);
    void deleteSelectedElements();

    SommetGraphics* findSommetItem(CUG::Sommet::const_ref grapheSommet);
    AreteGraphics* findAreteItem(CUG::Arete::const_ref grapheArete);

    void clearSelection();

    void toggleMode(int mode, bool toggled);

    bool saveGraphToFile(const QString &saveFileName);
    bool loadGraphFromFile(const QString &loadFileName);

    void deleteGraph();

private slots:
    void creerArete(SommetGraphics *firstSommet, SommetGraphics *secondSommet, int poids = 0, bool oriented = false);
    void creerArete(int firstSommetIndex, int secondSommetIndex, int poids = 0, bool oriented = false);

    void toggleSelectionMode(bool isToggled);
    void toggleRenommerMode(bool isToggled);
    void toggleSommetCreationMode(bool isToggled);
    void togglePondererMode(bool isToggled);
    void toggleArcCreationMode(bool isToggled);
    void toggleAreteCreationMode(bool isToggled);
    void toggleDeletionMode(bool isToggled);

    void colorationGlouton();
    void colorationDsatur();
    void colorationWelshPowell();
    void resolutionSudoku();
    void generateur();

private:

    void clearDrawingLine();

    SommetGraphics* getTopmostSommetItem(QList<QGraphicsItem*> items);
    QGraphicsItem* getSelectedItem();
    QList<SommetGraphics *> getSommets() const;
    QList<AreteGraphics *> getAretes() const;

    std::function<SommetGraphics *(QGraphicsItem *)> aSommet;
    std::function<AreteGraphics *(QGraphicsItem *)> aArete;

    const int WIDTH = 1024;
    const int HEIGHT = 768;

    CUG::Graphe* graphe;
    CUG::Sommet* sommet;
    QGraphicsLineItem* drawingLine;
    SommetPair selectedSommets;
    int toggledMode;

    QVector<int> couleurs;
    SommetGraphics* sommetg;
    std::set<int> indexConstants;



};
