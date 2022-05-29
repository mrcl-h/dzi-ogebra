#include"Geoapp.h"
#include<SFML/Graphics.hpp>

constexpr double epsilon = 2;
constexpr int antialias = 4;

Geoapp::Geoapp(){
    //height=300.0;
    //width=300.0;
    //uiwidth=200.0;
    uiBarrier = 0.6;
    sf::ContextSettings settings;
    settings.antialiasingLevel = antialias;
    window.create(sf::VideoMode(500, 300), "Geo", sf::Style::Default, settings);
    scalingFactor=1.0;
    centerX = centerY = 0;
    currentConditions.reset();
    //currentConditions.segmentCount = 1;
    resetConstructionElements (hulledElements);

    uiOptionConditions cond;

    //segment mid point
    uiObject segMidObject;
    segMidObject.creator = makeConstruction<segmentMiddle>;
    segMidObject.image.loadFromFile ("resources/segmentMid.png");
    segMidObject.image.setSmooth(true);

    cond.reset();
    cond.segmentCount = 1;
    registerUiOption (segMidObject, cond);

    //points mid point
    uiObject pointMidObject;
    pointMidObject.creator = makeConstruction<pointsMiddle>;
    pointMidObject.image.loadFromFile ("resources/pointsMid.png");
    pointMidObject.image.setSmooth(true);

    cond.reset();
    cond.pointCount = 2;
    registerUiOption (pointMidObject, cond);

    //parallel line
    uiObject parallelLineObject;
    parallelLineObject.creator = makeConstruction<parallelLine>;
    parallelLineObject.image.loadFromFile ("resources/parallelLine.png");
    parallelLineObject.image.setSmooth(true);

    cond.reset();
    cond.pointCount = 1;
    cond.lineCount = 1;
    registerUiOption (parallelLineObject, cond);

    //orthogonal line
    uiObject orthogonalLineObject;
    orthogonalLineObject.creator = makeConstruction<orthogonalLine>;
    orthogonalLineObject.image.loadFromFile ("resources/orthogonalLine.png");
    orthogonalLineObject.image.setSmooth (true);

    cond.reset();
    cond.pointCount = 1;
    cond.lineCount = 1;
    registerUiOption (orthogonalLineObject, cond);

    uiObject lineThroughPointsObject;
    lineThroughPointsObject.creator = makeConstruction<lineThroughPoints>;
    lineThroughPointsObject.image.loadFromFile("resources/lineThroughPoints.png");
    lineThroughPointsObject.image.setSmooth(true);
    cond.reset();
    cond.pointCount = 2;
    registerUiOption (lineThroughPointsObject, cond);

    uiObject circleWithCenterObject;
    circleWithCenterObject.creator = makeConstruction<circleWithCenter>;
    circleWithCenterObject.image.loadFromFile("resources/circleWithCenter.png");
    lineThroughPointsObject.image.setSmooth(true);
    cond.reset();
    cond.pointCount = 2;
    registerUiOption (circleWithCenterObject, cond);

    uiObject segmentFromPointsObject;
    segmentFromPointsObject.creator = makeConstruction<segmentFromPoints>;
    segmentFromPointsObject.image.loadFromFile("resources/segmentFromPoints.png");
    segmentFromPointsObject.image.setSmooth(true);
    cond.reset();
    cond.pointCount = 2;
    registerUiOption (segmentFromPointsObject, cond);

    loop();
}

void Geoapp::loop(){
    while (window.isOpen()){
        sf::Event event;
        events(event);
        update();

        window.display();
        window.clear(sf::Color(255,255,255,255));
    }
}

void Geoapp::events(sf::Event event){
    while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed){
                double x=(double)sf::Mouse::getPosition(window).x, y=(double)sf::Mouse::getPosition(window).y;
                Point mysz (x,y);
                if(x>uiBarrier*window.getSize().x){
                    UIhandling(mysz);
                } else {
                    whenClick(x,y);
                }
            } else if (event.type== sf::Event::Resized){

            } else if(event.type == sf::Event::KeyPressed){

                float xMovePoints = 0, yMovePoints = 0;
                float pointsMoveDist = 10;
            
                if (event.key.code == sf::Keyboard::Left) {
                    leftKeyDown=true;
                } else if (event.key.code == sf::Keyboard::Right) {
                    rightKeyDown=true;
                } else if (event.key.code == sf::Keyboard::Up) {
                    upKeyDown=true;
                } else if (event.key.code == sf::Keyboard::Down) {
                    downKeyDown=true;
                } else if (event.key.code == sf::Keyboard::H) {
                    xMovePoints -= pointsMoveDist;
                } else if (event.key.code == sf::Keyboard::J) {
                    yMovePoints += pointsMoveDist;
                } else if (event.key.code == sf::Keyboard::K) {
                    yMovePoints -= pointsMoveDist;
                } else if (event.key.code == sf::Keyboard::L) {
                    xMovePoints += pointsMoveDist;
                }
                for (auto i : hulledShapes) {
                    if (i->what_is() == shapeTypeId<Point>::typeId && i->isDependent == false) {
                        Point *pt = static_cast<Point*> (i);
                        pt->x += xMovePoints;
                        pt->y += yMovePoints;
                    }
                }
                for (auto i : constructions) {
                    i->adjust();
                }
                changeMode(event);
            }
            else if(event.type == sf::Event::KeyReleased){
                if (event.key.code == sf::Keyboard::Left) {
                    leftKeyDown=false;
                } else if (event.key.code == sf::Keyboard::Right) {
                    rightKeyDown=false;
                } else if (event.key.code == sf::Keyboard::Up) {
                    upKeyDown=false;
                } else if (event.key.code == sf::Keyboard::Down) {
                    downKeyDown=false;
                }
                changeMode(event);
            }
        }
}

void Geoapp::update(){
    drawObjects();
    drawUI();
    if (leftKeyDown) {
        centerX -= step;
    }
    if (rightKeyDown) {
        centerX += step;
    }
    if (upKeyDown) {
        centerY -= step;
    }
    if (downKeyDown) {
        centerY += step;
    }
}

void Geoapp::drawUI() const {
    unsigned int windowWidth = window.getSize().x, windowHeight = window.getSize().y;

    float uiWidth = windowWidth*(1-uiBarrier);
    float uiLeft = windowWidth * uiBarrier;

    window.setView (sf::View(sf::FloatRect(0,0,windowWidth, windowHeight)));
    sf::RectangleShape rect (sf::Vector2f(uiWidth,windowHeight));
    rect.move(sf::Vector2f(uiLeft,0));
    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(uiLeft, 0)),
        sf::Vertex(sf::Vector2f(uiLeft, windowHeight))
    };
    line[0].color=sf::Color(0,0,0);
    line[1].color=sf::Color(0,0,0);
    rect.setFillColor (sf::Color(0,255,255));
    window.draw(rect);
    window.draw(line, 2, sf::Lines);

    const std::vector<uiObject>& currentObjects = uiPages.find(uiMapId (currentConditions))->second;
    float top = 0;
    float objectHeight = uiWidth/2;

    sf::Sprite currentIcon;
    currentIcon.setPosition (uiLeft+uiWidth*0.3, top+objectHeight*0.1);

    sf::Vector2f leftSeparator (uiLeft, top+objectHeight);
    sf::Vector2f rightSeparator (windowWidth, top+objectHeight);
    for (auto& i : currentObjects) {
        currentIcon.setTexture (i.image);
        float texWidth = i.image.getSize().x, texHeight = i.image.getSize().y;
        currentIcon.setScale (objectHeight*0.8/texWidth, objectHeight*0.8/texHeight);
        window.draw (currentIcon);
        sf::Vertex separator [] = { sf::Vertex (leftSeparator), sf::Vertex (rightSeparator) };
        separator[0].color=sf::Color(0,0,0);
        separator[1].color=sf::Color(0,0,0);
        window.draw (separator, 2, sf::Lines);

        leftSeparator.y += objectHeight;
        rightSeparator.y += objectHeight;
        currentIcon.move(0,objectHeight);
    }
}

void Geoapp::drawObjects() const{
    float windowWidth = window.getSize().x, windowHeight = window.getSize().y;
    sf::FloatRect visible (centerX - uiBarrier*windowWidth/2, centerY-windowHeight/2,uiBarrier*windowWidth,windowHeight);
    sf::FloatRect box (0,0,window.getSize().x*uiBarrier,window.getSize().y);
    for(unsigned int i=0;i<hulledShapes.size();i++){
        hulledShapes[i]->hull_draw(&window, visible, box);
    }
    for(unsigned int i=0;i<shapes.size();i++){
        shapes[i]->draw(&window, visible, box);
    }
}

void Geoapp::UIhandling(Point mysz){
    unsigned int windowWidth = window.getSize().x;
    float uiWidth = windowWidth*(1-uiBarrier);
    float top = 0;
    float objectHeight = uiWidth/2;
    int clickedOption = (mysz.y-top)/objectHeight;
    std::vector<uiObject>& currentPage = uiPages[uiMapId (currentConditions)];
    if (clickedOption >= (int)currentPage.size()) {
        return;
    }
    if (clickedOption < 0) return;
    //Construction* constructionMade = currentPage[clickedOption].creator (hulledShapes, shapes);
    Construction *constructionMade = currentPage[clickedOption].creator (hulledElements, shapes);
    if (constructionMade != NULL) {
        constructions.push_back(constructionMade);
    }
    if (hulledShapes.size() > 0) {
        hulledShapes.back()->isCurrent = false;
    }
    for (auto i : hulledShapes) {
        i->isActive = false;
    }
    hulledShapes.clear();
    currentConditions.reset();
    resetConstructionElements (hulledElements);
}

void Geoapp::whenClick(double x, double y){
    Point clickPosition (centerX+x-float(window.getSize().x*uiBarrier)/2,centerY+y-float(window.getSize().y)/2);
    if(mode==0){
        Shape *S = new Point (clickPosition);;
        shapes.push_back(S);
        /*
        if(shapes.size()>1){
            if(shapes[shapes.size()-2]->what_is()=="Point"){
                Point* a=static_cast<Point*>(shapes[shapes.size()-2]);
                Line *l= new Line(*a,clickPosition);
                //Shape &L=*l;
                shapes.push_back(l);
            }
        }
        */
    } else if(mode==1){
        int a=FTCO(clickPosition);
        //std::cout<<a;
        if(a>-1){
            int selectCount;
            if (shapes[a]->isActive) {
                shapes[a]->isActive = false;
                shapes[a]->isCurrent = false;

                hulledShapes.erase (std::find(hulledShapes.begin(), hulledShapes.end(), shapes[a]));

                if (shapes[a]->what_is() == shapeTypeId<Point>::typeId) {
                    std::vector<Point*>& vec = hulledElements.points;
                    vec.erase (std::find(vec.begin(), vec.end(), shapes[a]));
                } else if (shapes[a]->what_is() == shapeTypeId<Segment>::typeId) {
                    std::vector<Segment*>& vec = hulledElements.segments;
                    vec.erase (std::find(vec.begin(), vec.end(), shapes[a]));
                } else if (shapes[a]->what_is() == shapeTypeId<Circle>::typeId) {
                    std::vector<Circle*>& vec = hulledElements.circles;
                    vec.erase (std::find(vec.begin(), vec.end(), shapes[a]));
                } else if (shapes[a]->what_is() == shapeTypeId<Line>::typeId) {
                    std::vector<Line*>& vec = hulledElements.lines;
                    vec.erase (std::find(vec.begin(), vec.end(), shapes[a]));
                }


                if (hulledShapes.size() > 0) {
                    hulledShapes.back()->isCurrent = true;
                }
                selectCount = -1;
            } else {
                shapes[a]->isActive = true;
                if (hulledShapes.size() > 0) 
                    hulledShapes.back()->isCurrent = false;

                hulledShapes.push_back(shapes[a]);
                hulledShapes.back()->isCurrent = true;

                if (shapes[a]->what_is() == shapeTypeId<Point>::typeId) {
                    hulledElements.points.push_back (static_cast<Point*>(shapes[a]));
                } else if (shapes[a]->what_is() == shapeTypeId<Segment>::typeId) {
                    hulledElements.segments.push_back (static_cast<Segment*>(shapes[a]));
                } else if (shapes[a]->what_is() == shapeTypeId<Circle>::typeId) {
                    hulledElements.circles.push_back (static_cast<Circle*>(shapes[a]));
                } else if (shapes[a]->what_is() == shapeTypeId<Line>::typeId) {
                    hulledElements.lines.push_back (static_cast<Line*>(shapes[a]));
                }
                selectCount = 1;
            }
            if (shapes[a]->what_is() == shapeTypeId<Point>::typeId) {
                currentConditions.pointCount += selectCount;
            } else if (shapes[a]->what_is() == shapeTypeId<Line>::typeId) {
                currentConditions.lineCount += selectCount;
            } else if (shapes[a]->what_is() == shapeTypeId<Segment>::typeId) {
                currentConditions.segmentCount += selectCount;
            } else if (shapes[a]->what_is() == shapeTypeId<Circle>::typeId) {
                currentConditions.circleCount += selectCount;
            }
        }
        /*Shape &s=shapes[a];
        if(findInObjects(&s)>-1){
            hulledShapes.erase(findInObjects(s));
        } else{
            hulledShapes.push_back(&s);
        }*/
    }

}

void Geoapp::changeMode(sf::Event e){
    if(e.key.code==sf::Keyboard::Q){
        mode=0;
    } else if(e.key.code==sf::Keyboard::W){
        mode=1;
    }
    //cout<<mode<<endl;
}



int Geoapp::FTCP(Point A) const {
    for(unsigned int i=0;i<shapes.size();i++){
        if(shapes[i]->what_is()==shapeTypeId<Point>::typeId && shapes[i]->dist(A)<20){ return i; }
    }
    return -1;
}
int Geoapp::FTCL(Point A) const{
    for(unsigned int i=0;i<shapes.size();i++){
        if(shapes[i]->what_is()==shapeTypeId<Line>::typeId && shapes[i]->dist(A)<epsilon){ return i; }
    }
    return -1;
}
int Geoapp::FTCS(Point A) const{
    for(unsigned int i=0;i<shapes.size();i++){
        if(shapes[i]->what_is()==shapeTypeId<Segment>::typeId && shapes[i]->dist(A)<epsilon){ return i; }
    }
    return -1;
}
int Geoapp::FTCC(Point A) const{
    for(unsigned int i=0;i<shapes.size();i++){
        if(shapes[i]->what_is()==shapeTypeId<Point>::typeId && shapes[i]->dist(A)<epsilon){ return i; }
    }
    return -1;
}
int Geoapp::FTCT(Point A) const { return -1; }

int Geoapp::FTCO(Point A) const {
    int temp;
    if((temp=FTCP(A))>-1){ return temp; }
    if((temp=FTCS(A))>-1){ return temp; }
    if((temp=FTCL(A))>-1){ return temp; }
    if((temp=FTCC(A))>-1){ return temp; }
    return FTCT(A);
}
