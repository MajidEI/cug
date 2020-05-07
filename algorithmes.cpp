
/*#include "algorithmes.h"
#include <vector>

using namespace CUG;


Algorithmes::Arguments::Arguments()
    : graphe(nullptr), Sommetsrc(-1)
{
}

Algorithmes::Arguments::~Arguments()
{
}


Algorithmes::Result::Result()
    : boolResult(false), aBool(false),
      intResult(0), aInt(false),
      grapheResult(nullptr), aGraphe(false)
{
}

Algorithmes::Result::~Result()
{
}

void Algorithmes::Result::setBool(bool result)
{
    boolResult = result;
    aBool = true;
}

void Algorithmes::Result::setInt(int result)
{
    intResult = result;
    aInt = true;
}

void Algorithmes::Result::setGraphe(Graphe* result)
{
    if (aGraphe)
        delete grapheResult;

    grapheResult = result;
    aGraphe = true;
}

bool Algorithmes::Result::estBool() const
{
    return aBool;
}

bool Algorithmes::Result::estInt() const
{
    return aInt;
}

bool Algorithmes::Result::estGraphe() const
{
    return aGraphe;
}

bool Algorithmes::Result::getBool() const
{
    return boolResult;
}

int Algorithmes::Result::getInt() const
{
    return intResult;
}

Graphe* Algorithmes::Result::getGraphe() const
{
    return grapheResult;
}


Algorithmes::Algorithmes()
{

}

Algorithmes::ElementQueue Algorithmes::getSequence() const
{
    return sequence;
}

bool Algorithmes::emptySequence() const
{
    return sequence.empty();
}

Algorithmes::GraphElement Algorithmes::getNextElement()
{
    if (!emptySequence())
    {
        Algorithmes::GraphElement next = sequence.front();
        sequence.pop();
        return next;
    }
    else
        return Algorithmes::GraphElement(std::make_pair(-1, Arete::Index(-1, -1)));
}

Algorithmes::GraphElement Algorithmes::getLastElement() const
{
    return sequence.back();
}

void Algorithmes::pushSommet(Sommet::Index sommetIndex)
{
    GraphElement sommet(std::make_pair(sommetIndex, Arete::Index(-1, -1)));
    sequence.push(sommet);
}

void Algorithmes::pushArete(Arete::Index areteIndex)
{
    GraphElement arete(std::make_pair(-1, areteIndex));
    sequence.push(arete);
}

std::list<int> colorationWP(Graphe graphe,int source){


    int nbrsommets=graphe.compterSommets();

    std::vector<bool> available(nbrsommets, false);
    std::vector<int> res(nbrsommets, -1);
    res[source] = source;
    for (int v = 1; v < nbrsommets; v++) {
        for (int adj = 0; adj < nbrsommets; adj++) {
            if (graphe.contientArete(graphe.recupererSommet(v), graphe.recupererSommet(adj))) {
                if (res[adj] != -1)
                    available[res[adj]] = true;
            }
        }

        int cr;
        for (cr = 0; cr < nbrsommets; cr++)
            if (available[cr] == false)
                break;

        res[v] = cr;
        for (int adj = 0; adj < nbrsommets; adj++) {
            if (graphe.contientArete(graphe.recupererSommet(v), graphe.recupererSommet(adj))) {
                if (res[adj] != -1)
                    available[res[adj]] = false;
            }
        }
    }
    std::list<int> res_list;
    std::copy(res.begin(), res.end(), std::back_inserter(res_list));
    return res_list;
}
*/


