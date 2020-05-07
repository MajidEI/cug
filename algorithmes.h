/*#pragma once

#include <queue>

#include "graphe.h"
#include <QGraphicsItem>
#include <QColor>

namespace CUG
{

class Algorithmes
{
public:

    using GraphElement = std::pair <int, Arete::Index>;
    using ElementQueue = std::queue <GraphElement>;
    //static std::list<int> colorationWP(Graphe graphe,int source=0);
    //std::list<int> displayColorationWP(Graphe graphe,int source=0);

    class Arguments
    {
    public:
        Arguments();
        ~Arguments();

        Graphe* graphe;
        Sommet::Index Sommetsrc;
    };

    class Result
    {
    public:
        Result();
        ~Result();

        void setBool(bool result);
        void setInt(int result);
        void setGraphe(Graphe* result);

        bool estBool() const;
        bool estInt() const;
        bool estGraphe() const;

        bool getBool() const;
        int getInt() const;
        Graphe* getGraphe() const;

    private:
        bool boolResult;
        bool aBool;

        int intResult;
        bool aInt;

        Graphe* grapheResult;
        bool aGraphe;
    };


    Algorithmes();

    ElementQueue getSequence() const;
    bool emptySequence() const;
    Algorithmes::GraphElement getNextElement();
    Algorithmes::GraphElement getLastElement() const;

    void pushSommet(Sommet::Index sommetIndex);
    void pushArete(Arete::Index areteIndex);

    virtual Result execute(Arguments args) = 0;

private:
    Algorithmes::ElementQueue sequence;
};

}
*/
