#include "Geoapp.h"
#include "keyStates.h"
#include <SFML/Graphics.hpp>

constexpr double epsilon = 2;
constexpr int antialias = 4;

//Geoapp::Geoapp() : inManager (), inWrapper (inManager), mainGeoView (window, &world, uiTracker), testPtr (new int) {
Geoapp::Geoapp() : inManager (), inWrapper (inManager), mainGeoView (&world, uiTracker), sfmlDrawing (&window), testPtr (new int) {

    uiBarrier = 0.6;
    sf::ContextSettings settings;
    settings.antialiasingLevel = antialias;
    window.create(sf::VideoMode(500, 300), "Geo", sf::Style::Default, settings);
    scalingFactor=1.0;
    centerX = centerY = 0;
    //resetUiOptionConditions (currentConditions);

    //rightMoving = false;

    makeUiOption<segmentMiddle> (uiTracker, "resources/segmentMid.png", uiSegmentObject (1));
    makeUiOption<pointsMiddle> (uiTracker, "resources/pointsMid.png", uiPointObject (2));
    makeUiOption<parallelLine> (uiTracker, "resources/parallelLine.png", uiLineObject (1), uiPointObject (1));
    makeUiOption<orthogonalLine> (uiTracker, "resources/orthogonalLine.png", uiLineObject (1), uiPointObject (1));
    makeUiOption<lineThroughPoints> (uiTracker, "resources/lineThroughPoints.png", uiPointObject (2));
    makeUiOption<circleWithCenter> (uiTracker, "resources/circleWithCenter.png", uiPointObject (2));
    makeUiOption<segmentFromPoints> (uiTracker, "resources/segmentFromPoints.png", uiPointObject (2));
    makeUiOption<centerOfMass> (uiTracker, "resources/centerOfMass.png", uiPointObject (3));
    makeUiOption<circleThreePoints> (uiTracker, "resources/circleThreePoints.png", uiPointObject (3));
    makeUiOption<powerLine> (uiTracker, "resources/powerLine.png", uiCircleObject (2));
    makeUiOption<symmetricalOfPoints> (uiTracker, "resources/symmetricalOfPoints.png", uiPointObject (2));
    makeUiOption<symmetricalOfSegment> (uiTracker, "resources/symmetricalOfSegment.png", uiSegmentObject (1));
    makeUiOption<lineCircleIntersection> (uiTracker, "resources/lineCircleIntersection.png", uiLineObject(1), uiCircleObject(1));
    makeUiOption<circlesIntersection> (uiTracker, "resources/circleCircleIntersection.png", uiCircleObject (2));
    makeUiOption<bisectorThreePoints> (uiTracker, "resources/bisectorThreePoints.png", uiPointObject (3));
    makeUiOption<tangentCirclePoint> (uiTracker, "resources/tangentsThroughPoint.png", uiPointObject(1), uiCircleObject(1));
    makeUiOption<circleConstraint> (uiTracker, "resources/circleConstraint.png", uiPointObject (1), uiCircleObject (1));
    makeUiOption<lineConstraint> (uiTracker, "resources/lineConstraint.png", uiPointObject (1), uiLineObject (1));
    makeUiOption<Triangle> (uiTracker, "resources/segmentMid.png", uiPointObject (3));

    junctionInputState *mainState = new junctionInputState (&inManager);
    mainState->addState (inputManager::Key::Left,   new inputCameraMovementState (&inManager, &mainGeoView, -10,   0));
    mainState->addState (inputManager::Key::Right,  new inputCameraMovementState (&inManager, &mainGeoView,  10,   0));
    mainState->addState (inputManager::Key::Up,     new inputCameraMovementState (&inManager, &mainGeoView,   0, -10));
    mainState->addState (inputManager::Key::Down,   new inputCameraMovementState (&inManager, &mainGeoView,   0,  10));

    mainState->addState (inputManager::Key::Left,   new inputCameraMovementState (&inManager, &mainGeoView, -100,   0), inputManager::ctrlMod);
    mainState->addState (inputManager::Key::Right,  new inputCameraMovementState (&inManager, &mainGeoView,  100,   0), inputManager::ctrlMod);
    mainState->addState (inputManager::Key::Up,     new inputCameraMovementState (&inManager, &mainGeoView,   0, -100), inputManager::ctrlMod);
    mainState->addState (inputManager::Key::Down,   new inputCameraMovementState (&inManager, &mainGeoView,   0,  100), inputManager::ctrlMod);

    mainState->addState (inputManager::Key::H, new inputPointMovementState (&inManager, this, -10,   0));
    mainState->addState (inputManager::Key::J, new inputPointMovementState (&inManager, this,   0,  10));
    mainState->addState (inputManager::Key::K, new inputPointMovementState (&inManager, this,   0, -10));
    mainState->addState (inputManager::Key::L, new inputPointMovementState (&inManager, this,  10,   0));

    mainState->addState (inputManager::Key::H, new inputPointMovementState (&inManager, this, -100,   0), inputManager::ctrlMod);
    mainState->addState (inputManager::Key::J, new inputPointMovementState (&inManager, this,   0,  100), inputManager::ctrlMod);
    mainState->addState (inputManager::Key::K, new inputPointMovementState (&inManager, this,   0, -100), inputManager::ctrlMod);
    mainState->addState (inputManager::Key::L, new inputPointMovementState (&inManager, this,  100,   0), inputManager::ctrlMod);

    mainState->addState (inputManager::Key::Q, new inputPointCreationState  (&inManager, &mainGeoView));
    mainState->addState (inputManager::Key::W, new inputPointSelectionState (&inManager, &mainGeoView));

    mainState->addState (inputManager::Key::M,      new inputSetMarkState  (&inManager, this));
    mainState->addState (inputManager::Key::Quote,  new inputGoToMarkState (&inManager, this));

    mainState->addState (inputManager::Key::U, new inputUIScrollState (&inManager, this,  10));
    mainState->addState (inputManager::Key::D, new inputUIScrollState (&inManager, this, -10));

    mainState->addState (inputManager::Key::Hyphen, new inputScalingState (&inManager, &mainGeoView, 2));
    mainState->addState (inputManager::Key::Equal, new inputScalingState (&inManager, &mainGeoView, 0.5), inputManager::shiftMod);

    mainState->addState (inputManager::Key::S, new inputSaveState (&inManager, &mainGeoView, "output.svg", &svgDrawing));

    inManager.setMainState (mainState);
    inManager.goToMainState();

    loop();
}

void Geoapp::changeScale (double rat) {
    scalingFactor *= rat;
}

float Geoapp::findUIScrollMin () const {
    unsigned int windowWidth = window.getSize().x, windowHeight = window.getSize().y;
    float uiWidth = windowWidth*(1-uiBarrier);
    std::unordered_map<uint32_t, std::vector<uiObject> >::const_iterator it;
    //it = uiPages.find(uiMapId (currentConditions));
    //if (it == uiPages.end()) return 0;
    //float objectHeight = uiWidth/2;
    //return -(objectHeight*it->second.size()-windowHeight);
    float objectHeight = uiWidth/2;
    return -(objectHeight*uiTracker.size()-windowHeight);
}

//Shape* Geoapp::findObjectHit (const Point& p) const {
//    Shape *shapeHit = NULL;
//    for (auto& i : shapes) {
//        if (i->isHit (p, scalingFactor)) {
//            if (shapeHit == NULL || shapeHit->getHitPriority() < i->getHitPriority()) {
//                shapeHit = i.get();
//            }
//        }
//    }
//    return shapeHit;
//}

const Point * Geoapp::getMark (char c) const {
    decltype(markMap)::const_iterator it = markMap.find (c);
    if (it == markMap.end()) { return NULL; }
    return &it->second;
}

void Geoapp::setMark (char c, const Point& p) {
    markMap[c] = p;
}

void Geoapp::moveCamera (double x, double y) {
    centerX += x;
    centerY += y;
}
void Geoapp::setCamera (const Point& p) {
    centerX = p.x;
    centerY = p.y;
}
const Point Geoapp::getCamera () {
    Point camera;
    camera.x = centerX;
    camera.y = centerY;
    return camera;
}

void Geoapp::scrollUI (double s) {
    uiTop += s;
    float min = findUIScrollMin();
    if (uiTop > 0 || min > 0) {
        uiTop = 0;
        return;
    }
    if (uiTop < min) {
        uiTop = min;
    }
}

void Geoapp::resetUIPosition () {
    uiTop = 0;
}

class shapeMovingVisitor : public ShapeVisitor {
    private:
        float x, y;
    public:
        void setMovement (float _x, float _y) {
            x = _x; y = _y;
        }
        virtual void visitSegment (SegmentShape* ss) {
            ss->moveShape (x,y);
        }
        virtual void visitTriangle (TriangleShape* ts) {
            ts->moveShape (x,y);
        }
        virtual void visitLine (LineShape* ls) {
            ls->moveShape (x,y);
        }
        virtual void visitCircle (CircleShape* cs) {
            cs->moveShape (x,y);
        }
        virtual void visitPoint (PointShape* ps) {
            ps->moveShape (x,y);
        }
};

void Geoapp::moveHulledPoints (double x, double y) {
    shapeMovingVisitor mv;
    mv.setMovement (x, y);
    world.visitHulledShapes (&mv);

    world.refreshConstructions ();
}

void Geoapp::loop(){
    sf::Event event;
    while (window.waitEvent(event)){
        events (event);

        window.clear(sf::Color(255,255,255,255));
        drawApp ();
        window.display();
    }
}

void Geoapp::events(const sf::Event& event){
    if (event.type == sf::Event::Closed){
        window.close();
    } else if (event.type == sf::Event::MouseButtonPressed){
        Point mysz;
        //mysz.x = sf::Mouse::getPosition(window).x; mysz.y=sf::Mouse::getPosition(window).y;
        mysz.x = event.mouseButton.x; mysz.y = event.mouseButton.y;
        if(mysz.x>uiBarrier*window.getSize().x){
            if (event.mouseButton.button == sf::Mouse::Left) {
                UIhandling(mysz);
            }
        } else {
            if (event.mouseButton.button == sf::Mouse::Left) {
                whenClick(mysz.x,mysz.y);
            } else if (event.mouseButton.button == sf::Mouse::Right) {
                //lastMouseRightPosition = mysz;
                //rightMoving = true;
                mainGeoView.startRightDragging (mysz.x, mysz.y);
            }
        }
    } else if (event.type == sf::Event::MouseButtonReleased){

        if (event.mouseButton.button == sf::Mouse::Right) {
            //rightMoving = false;
            mainGeoView.stopRightDragging ();

        }

    } else if (event.type== sf::Event::Resized){
        floatRect box;
        box.left = box.top = 0;
        box.height = getWindowHeight ();
        box.width = getWindowWidth () * uiBarrier;
        mainGeoView.setBox (box);
        resetUIPosition();
    } else if(event.type == sf::Event::KeyPressed){
        inWrapper.onKeyEvent (event);
    } else if(event.type == sf::Event::KeyReleased){
        inWrapper.onKeyEvent (event);
    } else if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            if (event.mouseWheelScroll.x > uiBarrier*window.getSize().x) {
                scrollUI (10*event.mouseWheelScroll.delta);
            } else {
                //scalingFactor *= (1.25-event.mouseWheelScroll.delta*0.75);
                mainGeoView.changeScale (1.25-event.mouseWheelScroll.delta*0.75);
            }
        }
    } else if (event.type == sf::Event::MouseMoved) {
        Point mysz;
        mysz.x = sf::Mouse::getPosition(window).x; mysz.y=sf::Mouse::getPosition(window).y;
        mainGeoView.continueRightDragging (mysz.x, mysz.y);
        //if (rightMoving) {
        //    Point mysz;
        //    mysz.x = sf::Mouse::getPosition(window).x; mysz.y=sf::Mouse::getPosition(window).y;
        //    centerX -= (mysz.x-lastMouseRightPosition.x)*scalingFactor;
        //    centerY -= (mysz.y-lastMouseRightPosition.y)*scalingFactor;

        //    lastMouseRightPosition = mysz;
        //}
    }
}

void Geoapp::drawApp(){
    drawObjects();
    drawUI();
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

    //const std::vector<uiObject>& currentObjects = uiPages.find(uiMapId (currentConditions))->second;
    float objectHeight = uiWidth/2;

    sf::Sprite currentIcon;
    currentIcon.setPosition (uiLeft+uiWidth*0.3, uiTop+objectHeight*0.1);

    sf::Vector2f leftSeparator (uiLeft, uiTop+objectHeight);
    sf::Vector2f rightSeparator (windowWidth, uiTop+objectHeight);
    //for (auto& i : currentObjects) {
    for (auto& i : uiTracker) {
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

void Geoapp::drawObjects() {
    /*
    float windowWidth = getWindowWidth(), windowHeight = getWindowHeight();

    floatRect visible (centerX - uiBarrier*windowWidth/2*scalingFactor, centerY-windowHeight/2*scalingFactor,uiBarrier*windowWidth*scalingFactor,windowHeight*scalingFactor);
    floatRect box (0,0,windowWidth*uiBarrier,windowHeight);

    sfmlDrawing.setVisible (visible);
    sfmlDrawing.setBox (box);

    hullDrawingShapeVisitor hdv;
    hdv.setDrawer (&sfmlDrawing);
    for(unsigned int i=0;i<hulledShapes.size();i++){
        hulledShapes[i]->acceptVisitor (&hdv);
    }

    drawShapes (&sfmlDrawing);
    */
    drawingClass * oldDrawer = mainGeoView.setDrawer (&sfmlDrawing);
    mainGeoView.setRects();
    mainGeoView.draw();
    mainGeoView.setDrawer (oldDrawer);
}

void Geoapp::UIhandling(const Point& mysz){
    unsigned int windowWidth = window.getSize().x;
    float uiWidth = windowWidth*(1-uiBarrier);
    float top = 0;
    float objectHeight = uiWidth/2;
    if (mysz.y < uiTop) {
        return;
    }
    unsigned int clickedOption = (mysz.y-uiTop)/objectHeight;
    //std::vector<uiObject>& currentPage = uiPages[uiMapId (currentConditions)];
    //if (clickedOption >= currentPage.size()) {
    if (clickedOption >= uiTracker.size()) {
        return;
    }
    //Construction *constructionMade = currentPage[clickedOption].creator (hulledElements, shapes);
    //Construction *constructionMade = uiTracker.getNthOption(clickedOption).creator (hulledElements, shapes);
    

    //Construction *constructionMade = uiTracker.getNthOption(clickedOption).creator (hulledShapes, shapes);
    //constructions.emplace_back (constructionMade);
    //if (hulledShapes.size() > 0) {
    //    hulledShapes.back()->setCurrent (false);
    //}
    //for (auto i : hulledShapes) {
    //    i->setActivity (false);
    //}
    //hulledShapes.clear();
    world.createConstruction (uiTracker.getNthOption(clickedOption).creator);


    resetUIPosition();
    //resetUiOptionConditions (currentConditions);
    uiTracker.resetConditions();

    //resetConstructionElements (hulledElements);
    //hulledElements.clear();
}

void Geoapp::whenClick(double x, double y){
    mainGeoView.click (x, y);
    /*
       Point clickPosition;
       clickPosition.x = centerX+(x-float(window.getSize().x*uiBarrier)/2)*scalingFactor;
       clickPosition.y = centerY+(y-float(window.getSize().y)/2)*scalingFactor;
       if(currentMode == mode::pointCreation){
    //std::unique_ptr<Shape> S (makePointShape(clickPosition.x, clickPosition.y));
    //shapes.push_back(std::move(S));
    } else if(currentMode == mode::selection){
    Shape *hitShape = findObjectHit (clickPosition);
    if(hitShape){
    int selectCount;
    if (hitShape->getActivity()) {
    hitShape->setActivity (false);
    hitShape->setCurrent (false);

    hulledShapes.erase (std::find(hulledShapes.begin(), hulledShapes.end(), hitShape));

    //hitShape->removeFromConstructionElements (hulledElements);
    //constructionElementsRemovingShapeVisitor rvs;
    //rvs.setElements (&hulledElements);
    //hitShape->acceptVisitor (&rvs);

    if (hulledShapes.size() > 0) {
    hulledShapes.back()->setCurrent (true);
    }
    selectCount = -1;
    } else {
    hitShape->setActivity (true);
    if (hulledShapes.size() > 0)
    hulledShapes.back()->setCurrent (false);

    hulledShapes.push_back(hitShape);
    hulledShapes.back()->setCurrent (true);

    //hitShape->addToConstructionElements (hulledElements);
    //constructionElementsAddingShapeVisitor avs;
    //avs.setElements (&hulledElements);
    //hitShape->acceptVisitor (&avs);

    selectCount = 1;
    }
    //hitShape->addToCurrentConditions (currentConditions, selectCount);
    uiOptionConditionsAdjusterShapeVisitor ocasv;
    //ocasv.setConditions (&currentConditions);
    ocasv.setTracker (&uiTracker);
    ocasv.setCount (selectCount);
    hitShape->acceptVisitor (&ocasv);
    resetUIPosition();
    }
    //uiPages[uiMapId(currentConditions)];
    }
    */

}
