/*
 *  RenderQueue.h
 *  Nebula Horizon
 *
 *  Created by Zachary Murray on 11/15/09.
 *
 */

#ifndef NHZ_RENDERQUEUE_H
#define NHZ_RENDERQUEUE_H

#include <vector>
#include "Renderable.h"

class RenderQueue : public Renderable {
  public:
    RenderQueue();
    static RenderQueue* getInstance();
    void enqueue(Renderable& spr);
    void dequeue(Renderable& spr);
    void render() const;
    void idle(const int elapsed);
    void handleMouseEvent(int button, int state, int x, int y);
    void handleMouseDrag(int x, int y);
    void handleMouseMotion(int x, int y);
    void clear();
    void postQueueClobbered();
    std::vector<Renderable*>::size_type size();
  private:
    static RenderQueue* _singleton;
    std::vector<Renderable*> _sprites;
    bool _isClobbered;
};

#endif
