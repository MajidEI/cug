#include <QInputDialog>
#include <QXmlStreamWriter>
#include <QMouseEvent>
#include <algorithm>
#include <functional>
#include "algorithmes.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include "inputdata.h"
#include <QTextStream>

#include "surinterface.h"


using namespace CUG;


SurInterface::SurInterface(QWidget* parent)
    : QGraphicsView(parent)
{
    QGraphicsScene* newScene = new QGraphicsScene(0, 0, WIDTH, HEIGHT);
    setScene(newScene);
    setMinimumSize(667,600);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    graphe = new Graphe();
    drawingLine = NULL;
    aSommet = [] (QGraphicsItem *item) {
        return qgraphicsitem_cast<SommetGraphics *>(item);
    };
    aArete = [] (QGraphicsItem *item) {
        return qgraphicsitem_cast<AreteGraphics *>(item);
    };
}

void SurInterface::mouseMoveEvent(QMouseEvent *event)
{
    switch (toggledMode)
    {
        case defaultMode:
        {
            SommetGraphics* sommet = aSommet(scene()->itemAt(mapToScene(event->pos()), QTransform()));
            if (sommet)
                ensureVisible(sommet, 40, 40);
            break;
        }
        case renommerMode:
        {
            setElementContent(getSelectedItem());
            break;
        }
        case pondererMode:
        {
            setElementContent(getSelectedItem());
            break;
        }
        case areteCreationMode:
        case arcCreationMode:
        {
            if (drawingLine)
            {
                drawingLine->setLine(QLineF(selectedSommets.first->getCenterPos(), mapToScene(event->pos())));
                ensureVisible(drawingLine);
            }
            break;
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void SurInterface::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (toggledMode == defaultMode)
        creerSommet(event->pos());
    else
        QGraphicsView::mouseDoubleClickEvent(event);
}

void SurInterface::mousePressEvent(QMouseEvent *event)
{
    switch (toggledMode)
    {
        case defaultMode:
        {
            QGraphicsView::mousePressEvent(event);
            break;
        }
        case sommetCreationMode:
        {
            creerSommet(event->pos());
            break;
        }
        case areteCreationMode:
        case arcCreationMode:
        {
            manageAreteCreation(event->pos());
            break;
        }
        case deletionMode:
        {
            deleteElementAtPosition(event->pos());
            break;
        }
    }
}

void SurInterface::keyPressEvent(QKeyEvent *event)
{
    clearDrawingLine();
    switch (event->key())
    {
        case Qt::Key_Delete:
        {
            deleteSelectedElements();
            break;
        }
        case Qt::Key_Space:
        {
            setElementContent(getSelectedItem());
            break;
        }
        default:
        {
            clearDrawingLine();
            QWidget::keyPressEvent(event);
        }
    }
}

void SurInterface::focusOutEvent(QFocusEvent *event)
{
    clearDrawingLine();
    QGraphicsView::focusOutEvent(event);
}

void SurInterface::creerSommet(const QPoint& pos, const QString& idtf)
{
    Sommet::const_ref newGraphSommet = graphe->addSommet(idtf);
    SommetGraphics* newSommetItem = new SommetGraphics(mapToScene(pos), newGraphSommet);
    scene()->addItem(newSommetItem);
}

void SurInterface::deleteElementAtPosition(const QPoint &pos)
{
    QGraphicsItem* item = scene()->itemAt(QPointF(mapToScene(pos)), QTransform());
    deleteElement(item);
}

void SurInterface::deleteElement(QGraphicsItem *item)
{
    if (SommetGraphics* sommet = aSommet(item))
    {
        foreach (AreteGraphics* arete, sommet->getAreteItems())
            deleteElement(arete);
        Sommet::const_ref graphSommet = sommet->getGraphSommet();
        delete sommet;
        graphe->delSommet(graphSommet);
    }
    else if (AreteGraphics* arete = aArete(item))
    {
        Arete::const_ref grapheArete = arete->getGrapheArete();
        delete arete;
        graphe->delArete(grapheArete);
    }
}

void SurInterface::deleteSelectedElements()
{
    QList<QGraphicsItem *> items = scene()->selectedItems();
    std::partition(items.begin(), items.end(), aArete);
    foreach (QGraphicsItem* item, items)
        deleteElement(item);
}

SommetGraphics *SurInterface::findSommetItem(Sommet::const_ref grapheSommet)
{
    QList<SommetGraphics *> sommets = getSommets();
    foreach (SommetGraphics *item, sommets)
    {
        if (item->getGraphSommet() == grapheSommet)
            return item;
    }
    return NULL;
}

AreteGraphics *SurInterface::findAreteItem(Arete::const_ref grapheArete)
{
    QList<AreteGraphics *> aretes = getAretes();
    foreach (AreteGraphics *arete, aretes)
    {
        if (arete->getGrapheArete() == grapheArete)
            return arete;
    }
    return NULL;
}

void SurInterface::manageAreteCreation(const QPoint &location)
{
    SommetGraphics* topmostSommet = getTopmostSommetItem(scene()->items(QPointF(mapToScene(location))));
    if (topmostSommet)
    {
        topmostSommet->setSelected(true);
        if (!selectedSommets.first)
        {
            selectedSommets.first = topmostSommet;
            drawingLine = scene()->addLine(QLine(location, location));
        }
        else if (selectedSommets.first && !selectedSommets.second)
        {
            selectedSommets.second = topmostSommet;
            creerArete(selectedSommets.first, selectedSommets.second);
            clearDrawingLine();
            clearSelection();
        }
    }
    else
    {
        clearDrawingLine();
        clearSelection();
    }
}

void SurInterface::setElementContent(QGraphicsItem* item)
{
    if (SommetGraphics* sommet = aSommet(item))
    {
        QString idtf = QInputDialog::getText(this,tr("Renommer le sommet"), tr("Nouvel identifiant:"), QLineEdit::Normal,sommet->getGraphSommet().getText());
        graphe->setSommetIdtf(sommet->getGraphSommet(), idtf);
    }
    else if (AreteGraphics* arete = aArete(item))
    {
        int poids = QInputDialog::getInt(this,tr("Changer le poids d'une Arete"),tr("Nouveau poids:"));
        graphe->setAretePoids(arete->getGrapheArete(), poids);
    }
}

void SurInterface::creerArete(SommetGraphics* firstSommet, SommetGraphics* secondSommet, int poids, bool oriented)
{
    if (firstSommet && secondSommet)
    {
        try
        {
            bool isOriented = (toggledMode == arcCreationMode) || oriented;
            Arete::const_ref newGraphArete = graphe->addArete(firstSommet->getGraphSommet(),secondSommet->getGraphSommet(),poids,isOriented);
            AreteGraphics* nouveauArete = new AreteGraphics(firstSommet,secondSommet,newGraphArete);
            scene()->addItem(nouveauArete);
            clearSelection();
        }
        catch (const mauvais_arete& e)
        {
            clearSelection();
            return;
        }
    }
}

void SurInterface::creerArete(int firstSommetIndex, int secondSommetIndex, int poids, bool oriented)
{
    SommetGraphics* firstSommet = NULL;
    SommetGraphics* secondSommet = NULL;
    foreach (SommetGraphics* sommet, getSommets())
    {
        if (sommet->getGraphSommet().getIndex() == firstSommetIndex)
            firstSommet = sommet;
        if (sommet->getGraphSommet().getIndex() == secondSommetIndex)
            secondSommet = sommet;
    }
    creerArete(firstSommet, secondSommet, poids, oriented);
}

void SurInterface::toggleSelectionMode(bool isToggled)
{
    setMouseTracking(false);
    toggleMode(defaultMode, isToggled);
}

void SurInterface::toggleRenommerMode(bool isToggled)
{
    setMouseTracking(false);
    toggleMode(renommerMode, isToggled);
}

void  SurInterface::togglePondererMode(bool isToggled)
{
    setMouseTracking(false);
    toggleMode(pondererMode, isToggled);
}
void SurInterface::toggleMode(int mode, bool toggled)
{
    if (toggled)
    {
        toggledMode = mode;
        scene()->clearSelection();
    }
    else
        toggledMode = defaultMode;
}

bool SurInterface::saveGraphToFile(const QString& saveFileName)
{
    QFile file(saveFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("graphe");

    foreach (SommetGraphics* sommet, getSommets()) {
        stream.writeStartElement("sommet");
        QXmlStreamAttributes attribs;
        attribs.append("x", QString::number(sommet->getCenterPos().x()));
        attribs.append("y", QString::number(sommet->getCenterPos().y()));
        attribs.append("Index", QString::number(sommet->getGraphSommet().getIndex()));
        attribs.append("contenu", sommet->getGraphSommet().getText());
        stream.writeAttributes(attribs);
        stream.writeEndElement();
    }
    foreach (AreteGraphics *arete, getAretes())
    {
        stream.writeStartElement("arete");
        QXmlStreamAttributes attribs;
        attribs.append("source", QString::number(arete->getGrapheArete().getIndex().first));
        attribs.append("destination", QString::number(arete->getGrapheArete().getIndex().second));
        attribs.append("poids", QString::number(arete->getGrapheArete().getPoids()));
        attribs.append("oriente", arete->isOriented() ? "oui" : "non");
        stream.writeAttributes(attribs);
        stream.writeEndElement();
    }

    stream.writeEndElement();
    stream.writeEndDocument();
    file.close();
    return true;
}

bool SurInterface::loadGraphFromFile(const QString& loadFileName)
{
    deleteGraph();

    QFile file(loadFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QXmlStreamReader stream(&file);

    while (!stream.atEnd() && !stream.hasError())
    {
        QXmlStreamReader::TokenType token = stream.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement)
        {
            if (stream.name() == "sommet")
            {
                QXmlStreamAttributes attribs = stream.attributes();

                QPoint pos(attribs.value("x").toInt(), attribs.value("y").toInt());
                QString text = attribs.value("contenu").toString();

                creerSommet(pos, text);
            }
            if (stream.name() == "arete")
            {
                QXmlStreamAttributes attribs = stream.attributes();

                int beginSommetIndex = attribs.value("source").toInt();
                int endSommetIndex = attribs.value("desitination").toInt();
                int poids = attribs.value("poids").toInt();
                bool oriented = attribs.value("oriente").toString() == "oui";

                creerArete(beginSommetIndex, endSommetIndex, poids, oriented);
            }
        }
    }
    file.close();
    return true;
}

void SurInterface::deleteGraph()
{
    scene()->clear();
    delete drawingLine;
    delete graphe;

    graphe = new Graphe();
}

void SurInterface::toggleSommetCreationMode(bool isToggled)
{
    toggleMode(sommetCreationMode, isToggled);
}


void SurInterface::toggleArcCreationMode(bool isToggled)
{
    setMouseTracking(true);
    toggleMode(arcCreationMode, isToggled);
}

void SurInterface::toggleAreteCreationMode(bool isToggled)
{
    setMouseTracking(true);
    toggleMode(areteCreationMode, isToggled);
}

void SurInterface::toggleDeletionMode(bool isToggled)
{
    deleteSelectedElements();
    toggleMode(deletionMode, isToggled);
}

void SurInterface::colorationGlouton()
{

    int nbSommets = graphe->compterSommets();

        QList<QColor> colors;

        QList<SommetGraphics*> sommetsGraphics = getSommets();

        QList<QColor> sommetsColorier;

        for (int i = 0 ; i < nbSommets; i++) {
            int x = 0;
            int y = 0;
            int z = 0;

            // While x == y == z
            // Generate another color
            while(x == y && y == z)
            {
                x =  rand()%255;
                y =  rand()%255;
                z =  rand()%255;
            }

            QColor m(x, y, z);

            colors.append(m);

            sommetsColorier.append(QColor("#000"));
        }

        foreach(Sommet *sommet, graphe->lista())
        {
            foreach(QColor color, colors)
            {
                // Compter le nombre des adjancents qui non utilise pas (color)
                int compter = 0;

                // Nombre des adjacents
                int nbAdjacentes = 0;

                QPair<int, Sommet::Index> sommetPaire;

                foreach(sommetPaire, sommet->getSommetsAdjacents())
                {
                    nbAdjacentes++;
                    if (sommetsColorier.at(sommetPaire.second).rgb() != color.rgb())
                    {
                        // Si oui incrementer le nb des successuer qu'ont pas la couleur i
                        compter++;
                    }
                }

                if (nbAdjacentes == compter)
                {
                    QTimer tmp;
                    QEventLoop boc;
                    tmp.setInterval(500);
                    QObject::connect(&tmp,&QTimer::timeout,&boc,&QEventLoop::quit);
                    tmp.start();
                    boc.exec();
                    sommetsColorier.replace(sommet->getIndex(), color);
                    sommetsGraphics.at(sommet->getIndex())->setBrush(color);
                    break;
                }
            }
        }

        //compter le nbr de couleur dans la table couleur


        int s=0;
         for(int c=0;c<nbSommets;c++){
             for(int a=c+1;a<nbSommets;a++){
                 if (colors[c]==colors[a]){
                     s++;
                     //break;
                 }
             }
         }
        QTextEdit *txt =new QTextEdit();
        txt->setText("Le nombre chromatqiue de ce graphe est :" );
        txt->append(QString::number(s));
        //txt->setText(QString::number(s));
        txt->setTextColor(Qt::darkCyan);
        txt->setFontFamily("monospace");
        txt->setReadOnly(true);
        txt->setMaximumSize(350,40);
        txt->setMinimumSize(350,40);
        txt->setStyleSheet("background:rgb(44,44,44);font-size:10pt;font-weight:bold;color:Cyan;");
        txt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        txt->setWindowIcon(QPixmap(":/icons/cuglogo1.png"));
        txt->show();
        QVBoxLayout* lay= new QVBoxLayout();
        lay->addWidget(txt,0,Qt::AlignBottom |Qt::AlignLeft);
        lay->setMargin(0);
        this->setLayout(lay);



}

void SurInterface::colorationDsatur()
{
    int nbrdeSommets=graphe->compterSommets();


    QVector<int> couleur(nbrdeSommets,-1);
    QVector<int> Degree(nbrdeSommets,0);
    QVector<int> Dsat(nbrdeSommets,0);

    //initialisation
    for(int sommet=0; sommet<nbrdeSommets-1;sommet++){
        Degree.append(graphe->recupererSommet(sommet).getDegree());
        Dsat.append(graphe->recupererSommet(sommet).getDegree());
    }
    //Boucle principale
    for(int i=0;i<nbrdeSommets-1;i++){
        int x=0;
        //Recherche du sommet de Dsat max;
        for(int y=0;y<nbrdeSommets-1;y++){
            if((couleur[x]==(-1)) && Dsat[y]>Dsat[x]){
                x=y;
            }
        }
        //en cas de conflit, prendre celui de Degre max
        for(int y=0;y<nbrdeSommets-1;y++){
            if((couleur[x]==(-1)) && Dsat[y]==Dsat[x] && Degree[y]>Degree[x] ){
                x=y;
            }
        }

        //Recherche de la plus petite couleur non utilisee dans N(x)
        QVector<bool> Libre;
        int z=Libre.size();

        for (int f=0;f<z;f++) {
            Libre[f]=true;
        }

        QPair<int,Sommet::Index> pair;
        foreach(pair, graphe->recupererSommet(x).getSommetsAdjacents()){
            if (couleur[pair.second] != -1)
                 Libre[couleur[pair.second]] = false;

        }

        int index=0;
        while (Libre[index]==false) {
            index=index+1;
        }

        //Mettre a jour Dsat pour les voisins de x

        foreach(pair,graphe->recupererSommet(x).getSommetsAdjacents()){
            if(Dsat[pair.second]==Degree[pair.second]){
                Dsat[pair.second]=1;

            QPair<int,Sommet::Index> pairy;
            foreach(pairy,graphe->recupererSommet(pair.second).getSommetsAdjacents()){
                if (index!=couleur[pairy.second]){
                    Dsat[pair.second]=Dsat[pair.second]+1;

              }

            }

         }

        }
        //Affecter la couleur donee par index a x
        couleur[x]=index;



    }

    std::map<int, QColor> colorMap;

    for (int i:couleur){

        int x= rand() % 256;
        int y= rand() % 256;
        int z= rand() % 256;

        if (x!=y and y!=z){
            colorMap[i] = QColor(x, y, z);
        }

    }

    //colorier
    int k=0;
    foreach(SommetGraphics* sommet,getSommets()){
            std::map<int, QColor>::const_iterator pos = colorMap.find(couleur[k]);
            sommet->setBrush(pos->second);
            k++;
        }

    //compter le nbr de couleur dans la table couleur


     int s=0;
     for(int c=0;c<nbrdeSommets;c++){
         for(int a=c+1;a<nbrdeSommets;a++){
             if (couleur[c]==couleur[a]){
                 s++;
                 //break;
             }
         }
     }

    QTextEdit *txt =new QTextEdit();
    txt->setText("Le nombre chromatqiue de ce graphe est :" );
    txt->append(QString::number(s));
    txt->setTextColor(Qt::darkCyan);
    txt->setFontFamily("monospace");
    txt->setReadOnly(true);
    txt->setMaximumSize(350,40);
    txt->setMinimumSize(350,40);
    txt->setStyleSheet("background:rgb(44,44,44);font-size:10pt;color:Cyan;font-weight:bold;");
    txt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    txt->setWindowIcon(QPixmap(":/icons/cuglogo1.png"));
    txt->show();
    QVBoxLayout* lay= new QVBoxLayout();
    lay->addWidget(txt,0,Qt::AlignBottom |Qt::AlignLeft);
    lay->setMargin(0);
    this->setLayout(lay);




}
void SurInterface::colorationWelshPowell()
{
    int nbrsommets=graphe->compterSommets();

    std::vector<bool> disponible(nbrsommets, true);
    std::vector<int> couleur(nbrsommets, -1);
    std::vector<int> Deg(nbrsommets,0);

    QList<QColor> colors;
    QList<QColor> colorsto;

    for (int i = 0 ; i < nbrsommets; i++) {
        int x = 0;
        int y = 0;
        int z = 0;

        // While x == y == z
        // Generate another color
        while(x == y && y == z)
        {
            x =  rand()%255;
            y =  rand()%255;
            z =  rand()%255;
        }

        QColor m(x, y, z);

        colors.append(m);
    }

    QList<SommetGraphics*> sommetscolor=getSommets();
    QLinkedList<Sommet*> sommm=graphe->lista();
    QList<Sommet*> sommets;
    foreach(Sommet* k,sommm){
        sommets.append(k);
    }
    std::sort(sommets.begin(),sommets.end(),Graphe::compare);
    for (int j=0;j<sommets.size();j++) {
        Deg[j]=sommets[j]->getDegree();
    }
    for (int i=0;i<nbrsommets-1;i++) {
        int x=sommets[i]->getIndex();
        QPair<int,Sommet::Index> pair;
        foreach(pair, graphe->recupererSommet(x).getSommetsAdjacents()){
            if(couleur[pair.second]!=-1){
                disponible[couleur[pair.second]]=false;
            }
        }
        int index=0;
        while(disponible[index]==false){
            index=index+1;
        }
        couleur[x]=index;
        colorsto.append(colors[couleur[x]]);


    }

    for (int c=0;c<colorsto.size();c++){
        int t=sommets[c]->getIndex();
        sommetscolor[t]->setBrush(colorsto[c]);
    }





   /* int nbrsommets=graphe->compterSommets();

    std::vector<bool> disponible(nbrsommets, false);
    std::vector<int> couleur(nbrsommets, -1);
    std::vector<int> couleursint;


    QLinkedList<Sommet*> sommm=graphe->lista();
    QList<Sommet*> so;
    foreach(Sommet* k,sommm){
        so.append(k);
    }
    std::sort(so.begin(),so.end(),Graphe::compare);

    couleur[so[0]->getIndex()]=0;
    couleursint.push_back(so[0]->getIndex());

    for(int i=1;i<nbrsommets;i++){

            int is=so[i]->getIndex();


            QPair<int,Sommet::Index> pair;
            foreach(pair, graphe->recupererSommet(is).getSommetsAdjacents()){
                if(couleur[pair.second]!=-1){
                    disponible[couleur[pair.second]]=true;
                }
            }

            int j;
            for(j=0;j<nbrsommets;j++)
                if(disponible[j]==false)
                       break;
            couleur[is]=j;
            couleursint.push_back(is);
            foreach(pair, graphe->recupererSommet(is).getSommetsAdjacents()){

                if(couleur[pair.second]!=-1){
                      disponible[couleur[pair.second]]=false;
                 }
            }

        }
    std::map<int, QColor> colorMap;

    for (int color_i: couleur){

        int x= rand() % 256;
        int y= rand() % 256;
        int z= rand() % 256;

        if (x!=y and y!=z){
            colorMap[color_i] = QColor(x, y, z);
        }

    }
   //compter le nbr de couleur dans la table couleur
    int s=0;
    for(int c=0;c<nbrsommets;c++){
        for(int a=c+1;a<nbrsommets;a++){
            if (couleur[c]==couleur[a]){
                s++;
                //break;
            }
        }
    }


    QList<SommetGraphics*> sg=getSommets();

    //int m=0;
    //foreach(SommetGraphics* sommet,getSommets()){
    for(int m=0;m<nbrsommets;m++){
        std::map<int, QColor>::const_iterator pos = colorMap.find(couleur[m]);
        sg[couleursint[m]]->setBrush(pos->second);
        //m++;
    }
    int k=0;
    foreach(SommetGraphics* sommet,getSommets()){
            std::map<int, QColor>::const_iterator pos = colorMap.find(couleur[k]);
            sommet->setBrush(pos->second);
            k++;
        }
     QTextEdit *txt =new QTextEdit();
     txt->setText("Le nombre chromatqiue de ce graphe est :" );
     txt->append(QString::number(s));
     txt->setTextColor(Qt::darkCyan);
     txt->setFontFamily("monospace");
     txt->setReadOnly(true);
     txt->setMaximumSize(350,40);
     txt->setMinimumSize(350,40);
     txt->setStyleSheet("background:rgb(44,44,44);font-size:10pt;color:Cyan;font-weight:bold;");
     txt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     txt->setWindowIcon(QPixmap(":/icons/cuglogo1.png"));
     txt->show();
     QVBoxLayout* lay= new QVBoxLayout();
     lay->addWidget(txt,0,Qt::AlignBottom |Qt::AlignLeft);
     lay->setMargin(0);
     this->setLayout(lay);
*/
}


void SurInterface::resolutionSudoku()
{

    for(int i=0;i<81;i++){
        int size=sqrt(81-0.1);
        int dx=6600/(81+1);
        int dy=5000/(81+1);

        QPoint pos(i%(size+1)*dx+120, i/(size+1)*dy+50);

        creerSommet(pos);
    }

    for(int i=0;i<81;i++){
           static int j;

            // Connections
            static int quadrant_row;
            static int quadrant_column;
            quadrant_row = (i % 9) / 3;
            quadrant_column = (i / 9) / 3;
            j = i + 1;
            while ( ((j / 9) / 3) == quadrant_column )
            {
                if ( ((j % 9) / 3)  == quadrant_row )
                {
                    creerArete(i,j);
                    j++;
                }
                else
                {
                    j += 6;
                }
            }

            // row Connections
            j = i+1;

            while ( (j % 9) / 3 == quadrant_row ) j++;
            while ( j % 9 > i % 9 )
            {
                creerArete(i,j);
                j++;
            }

             //column Connections
            j = i + 9;

            while ( ((j / 9) / 3) == quadrant_column ) j += 9;
            while ( j < 81 )
            {
                creerArete(i,j);
                j += 9;
            }


    }
}

void SurInterface::generateur()
{
    int type=QInputDialog::getInt(this,tr("oriente/non oriente"),tr("Taper 1 si le graphe est oriente et 0 sinon:"));
    int ponderation= QInputDialog::getInt(this,tr("Les donnes"),tr("Taper 1 si le graphe est ponderer et 0 sinon :"));
    if(type==0){
        int nbrdesommet = QInputDialog::getInt(this,tr("Les donnes"),tr("Donner le nombre sommets :"));
        int nbrdearete= QInputDialog::getInt(this,tr("Les donnes"),tr("Donner le nombre aretes :"));
        for(int i=0;i<nbrdesommet;i++){
            int size=rand()%nbrdesommet;
            int dx=rand()%nbrdesommet+50;
            int dy=rand()%nbrdesommet+40;
            QPoint pos(i%(size+1)*dx+120, i/(size+1)*dy+50);
            creerSommet(pos);
        }
        int k=nbrdearete+1;
        if(ponderation==0){
            while(k!=0){
            int a=rand()%nbrdearete;
            int b=rand()%nbrdearete;
              creerArete(a,b);
              k--;
            }}
        else{
            while(k!=0){
            int a=rand()%nbrdearete;
            int b=rand()%nbrdearete;
            int p=rand()%100;
              creerArete(a,b,p,false);
              k--;
            }
        }
    }
    else{
        int nbrdesommet = QInputDialog::getInt(this,tr("Les donnes"),tr("Donner le nombre sommets:"));
        int nbrdearete= QInputDialog::getInt(this,tr("Les donnes"),tr("Donner le nombre aretes:"));
        for(int i=0;i<nbrdesommet;i++){
            int size=rand()%nbrdesommet;
            int dx=rand()%nbrdesommet+50;
            int dy=rand()%nbrdesommet+40;
            QPoint pos(i%(size+1)*dx+120, i/(size+1)*dy+50);
            creerSommet(pos);
        }
        int k=nbrdearete+1;
        if(ponderation==0){
            while(k!=0){
            int a=rand()%nbrdearete;
            int b=rand()%nbrdearete;
              creerArete(a,b);
              k--;
            }}
        else{
            while(k!=0){
            int a=rand()%nbrdearete;
            int b=rand()%nbrdearete;
            int p=rand()%100;
              creerArete(a,b,p,false);
              k--;
            }
        }
    }

}
SommetGraphics *SurInterface::getTopmostSommetItem(QList<QGraphicsItem *> items)
{
    if (!items.isEmpty())
    {
        auto isNotSommet = [this](QGraphicsItem* item) -> bool
                        { return !aSommet(item); };
        items.erase(std::remove_if(items.begin(), items.end(), isNotSommet), items.end());
        if (!items.isEmpty())
        {
            auto sortByZ = [](QGraphicsItem* firstItem, QGraphicsItem* secondItem)
                            { return firstItem->zValue() < secondItem->zValue(); };
            std::sort(items.begin(), items.end(), sortByZ);

            return aSommet(items.first());
        }
    }
    return NULL;
}

QGraphicsItem* SurInterface::getSelectedItem()
{
    QList<QGraphicsItem*> selectedItems = scene()->selectedItems();
    if (selectedItems.length() == 1)
        return selectedItems.first();
    else
        return NULL;
}

QList<SommetGraphics *> SurInterface::getSommets() const
{
    QList<QGraphicsItem *> items = scene()->items();
    QList<SommetGraphics *> sommets;
    foreach (QGraphicsItem *item, items)
    {
        if (SommetGraphics *sommet = aSommet(item))
            sommets.append(sommet);
    }
    auto indexComp = [](SommetGraphics* first, SommetGraphics* second)
    {
        return first->getGraphSommet().getIndex() < second->getGraphSommet().getIndex();
    };

    std::sort(sommets.begin(), sommets.end(), indexComp);
    return sommets;
}

QList<AreteGraphics *> SurInterface::getAretes() const
{
    QList<QGraphicsItem *> items = scene()->items();
    QList<AreteGraphics *> aretes;
    foreach (QGraphicsItem *item, items)
    {
        if (AreteGraphics *arete = aArete(item))
            aretes.append(arete);
    }
    return aretes;
}

void SurInterface::clearDrawingLine()
{
    delete drawingLine;
    drawingLine = NULL;
}

void SurInterface::clearSelection()
{
    scene()->clearSelection();
    selectedSommets = SommetPair();
}
