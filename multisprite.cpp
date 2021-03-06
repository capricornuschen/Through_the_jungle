#include <cmath>
#include "multisprite.h"
#include "gamedata.h"
#include "frameFactory.h"
#include "SDL/SDL_rotozoom.h"

using namespace std; 

void MultiSprite::advanceFrame(Uint32 ticks) {
  float ms = 1000.0/frameInterval;
  timeSinceLastFrame += ticks;
  int df = timeSinceLastFrame / ms;
  timeSinceLastFrame -= df * ms;
  currentFrame = (currentFrame + df) % numberOfFrames;
if(shieldflag){
  shieldTime += ticks;
  if(shieldTime>3000){
      shieldflag = false;
      shieldTime = 0;
  }
}
if(explodingflag){
  if (currentexplodingFrame < 20){
    timeSinceLastExplodingFrame += ticks;
    int ef = timeSinceLastExplodingFrame / ms;
    timeSinceLastExplodingFrame -= ef * ms;
    currentexplodingFrame = (currentexplodingFrame + ef) % 21;
    //cout<<"currentexplodingFrame is :"<<currentexplodingFrame<<endl;
  }
  else {
    gone = true;
    explodingflag = false;
    currentexplodingFrame = 0;
  }
}
}
MultiSprite::MultiSprite( const std::string& name) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  frames( FrameFactory::getInstance().getFrames(name) ),
  shieldframe( FrameFactory::getInstance().getFrames("halo") ),
  explodingframe( FrameFactory::getInstance().getFrames("leopardexp") ),
  // anson debug
  booRight(true),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  shieldTime(0),
  waitTime(0),
  explodingTime(0),
  currentFrame(0),
  currentexplodingFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  timeSinceLastFrame(0),
  timeSinceLastExplodingFrame(0),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight()),
  shieldWidth(shieldframe[0]->getWidth()),
  shieldHeight(shieldframe[0]->getHeight()),
  explodingWidth(explodingframe[0]->getWidth()),
  explodingHeight(explodingframe[0]->getHeight()),
  frameName(name),
  shieldflag(false),
  shieldgodflag(false),
  explodingflag(false),
  gone(false)
{ }

MultiSprite::MultiSprite(const MultiSprite& s) :
  Drawable(s), 
  frames(s.frames),
  shieldframe(s.shieldframe),
  explodingframe(s.explodingframe),
  booRight(s.booRight),
  worldWidth( s.worldWidth ),
  worldHeight( s.worldHeight ),
  shieldTime(s.shieldTime),
  waitTime(s.waitTime),
  explodingTime(s.explodingTime),
  currentFrame(s.currentFrame),
  currentexplodingFrame(s.currentexplodingFrame),
  numberOfFrames( s.numberOfFrames ),
  frameInterval( s.frameInterval ),
  timeSinceLastFrame( s.timeSinceLastFrame ),
  timeSinceLastExplodingFrame(s.timeSinceLastExplodingFrame),
  frameWidth( s.frameWidth ),
  frameHeight( s.frameHeight ),
  shieldWidth( s.shieldWidth ),
  shieldHeight( s.shieldHeight ),
  explodingWidth( s.explodingWidth ),
  explodingHeight( s.explodingHeight ),
  frameName(s.frameName),
  shieldflag(s.shieldflag),
  shieldgodflag(s.shieldgodflag),
  explodingflag(s.explodingflag),
  gone(s.gone)
  { }

void MultiSprite::resetplayer(){
  setPosition(Vector2f(Gamedata::getInstance().getXmlInt(frameName+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(frameName+"/startLoc/y")));
  //cout<<"frameName is :"<< Vector2f(Gamedata::getInstance().getXmlInt(frameName+"/startLoc/x"), Gamedata::getInstance().getXmlInt(frameName+"/startLoc/y"))<<endl;
}

void MultiSprite::draw() const { 
  extern double angle;
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());

if (!gone){
  if (explodingflag){
    explodingframe[currentexplodingFrame]->draw(x-(explodingWidth/2-frameWidth/2),y-(explodingHeight/2-frameHeight/2));
  }
  else {  
    if (shieldflag){
      shieldframe[0]->draw(x-(shieldWidth/2-frameWidth/2),y-(shieldHeight/2-frameHeight/2));
    }
    if (shieldgodflag){
      shieldframe[0]->draw(x-(shieldWidth/2-frameWidth/2),y-(shieldHeight/2-frameHeight/2));
    }
    frames[currentFrame]->draw(x, y,angle);
  }
}
}
void MultiSprite::update(Uint32 ticks) { 
  extern double zoom;
  extern bool scan;

  if(!gone){
    advanceFrame(ticks);
  }
  else {
    waitTime += ticks;
    //cout<<"waitTime is:"<<waitTime<<endl;
    if(waitTime>1000){
      //cout<<"waitTime>3000!!!!!"<<endl;
      gone = false;
      waitTime = 0;
      explodingflag = false;
    }
  }
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
    setPosition(getPosition() + incr);

  if ( Y() < 0) {
    velocityY( abs( velocityY() ) );
  }
  if ( Y() > worldHeight-frameHeight) {
    velocityY( -abs( velocityY() ) );
  }

  if ( X() < 0) {
    velocityX( abs( velocityX() ) );
  }
  if ( X() > worldWidth-frameWidth) {
    velocityX( -abs( velocityX() ) );
  }  
  if ( velocityX() < 0 && booRight ) {
    booRight = false;
    //SDL_Surface* surface = frame->getSurface();
    for(unsigned int i = 0; i < numberOfFrames; i++){
      SDL_Surface* surface = frames[i]->getSurface();
      // Zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.'zoomx' and 'zoomy' are scaling factors for width and height. If 'smooth' is 1 then the destination 32bit surface is anti-aliased. If the surface is not 8bit or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.
      // and zoomx = -1 means not scale but reverse the frame by 180 angle. 

      SDL_Surface* pic = zoomSurface(surface, -1, 1, SMOOTHING_ON);
      if ( pic != NULL ) {
        //anson check if this lead to a double free, maybe the pointer is shallow copy.
        SDL_FreeSurface(surface);
        frames[i]->setSurface(pic);
      }
      else throw "zoomSurface failed in TwowayEffectsSprite";
      //SDL_FreeSurface( pic );
    }
  }
  if ( velocityX() > 0 && !booRight) {
    booRight = true;
    //SDL_Surface* surface = frame->getSurface();
    for(unsigned int j = 0; j < numberOfFrames; j++){
      SDL_Surface* surface = frames[j]->getSurface();
      SDL_Surface* pic = zoomSurface(surface, -1, 1, SMOOTHING_ON);
      if ( pic != NULL ) {
        //anson check if this lead to a double free, maybe the pointer is shallow copy.
        SDL_FreeSurface(surface);
        frames[j]->setSurface(pic);
      }
      else throw "zoomSurface failed in TwowayEffectsSprite";
    }
  }
  /*
  if (scan){
      //std::cout << "current zoom is:" << zoom << std::endl;
      //std::cout << "current scan is:" << scan << std::endl;
      //frames = FrameFactory::getInstance().modifyFrames(frameName,zoom); 
   for(unsigned int n = 0; n < numberOfFrames; n++){
      SDL_Surface* surface = frames[n]->getSurface();
      SDL_Surface* pic2 = zoomSurface(surface, zoom, zoom, SMOOTHING_ON);
      if ( pic2 != NULL ) {
        //anson check if this lead to a double free, maybe the pointer is shallow copy.
        //SDL_FreeSurface(surface);
        frames[n]->setSurface(pic2);
      }
      else throw "zoomSurface failed in TwowayEffectsSprite";
    }
  }
  */
}
