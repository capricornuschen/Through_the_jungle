#ifndef EVILSPRITE__H
#define EVILSPRITE__H
#include <string>
#include "drawable.h"

class CollisionStrategy;
class EvilSprite : public Drawable {
public:
  EvilSprite(const std::string&);
  EvilSprite(const std::string&, const Vector2f& pos, const Vector2f& vel);
  EvilSprite(const std::string&, 
         const Vector2f& pos, const Vector2f& vel, const Frame*);
  EvilSprite(const EvilSprite& s);
  virtual ~EvilSprite() { } 
  EvilSprite& operator=(const EvilSprite&);

  virtual const Frame* getFrame() const { return frame; }
  virtual void draw() const;

  virtual void update(Uint32 ticks);
  bool collidedWith(const Drawable* d) const;

private:
  const Frame * frame;
  int frameWidth;
  int frameHeight;
  int worldWidth;
  int worldHeight;
  CollisionStrategy* strategy;
  //Viewport& viewport;
  int getDistance(const EvilSprite*) const;
};
#endif
