/*
 *  RenderQueue.cpp
 *  Nebula Horizon
 *
 *  Created by Zachary Murray on 11/15/09.
 *
 */

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "RenderQueue.h"

RenderQueue* RenderQueue::_singleton = 0;

RenderQueue::RenderQueue() {
  _isClobbered = false;
}

RenderQueue* RenderQueue::getInstance() {
  if(_singleton == 0) {
    _singleton = new RenderQueue;
  }
  return _singleton;
}

void RenderQueue::enqueue(Renderable& spr) {
  _sprites.push_back(&spr);
}

void RenderQueue::dequeue(Renderable& spr) {
  std::vector<Renderable*>::reverse_iterator vector_it = _sprites.rbegin();
  bool found_it = false;
  while(vector_it != _sprites.rend() && !found_it) {
    if(&spr == *vector_it) {
      // note that this converts to a plain iterator for erase()
      _sprites.erase((++vector_it).base());
      found_it = true;
    }
    vector_it++;
  }
}

void RenderQueue::render() const {
  std::vector<Renderable*>::const_iterator vector_it = _sprites.begin();
  while(vector_it != _sprites.end()) {
    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    (*vector_it)->render();
    glPopAttrib();
    glPopMatrix();
    vector_it++;
  }
}

void RenderQueue::idle(const int elapsed) {
  std::vector<Renderable*>::iterator vector_it = _sprites.begin();
  while(vector_it != _sprites.end()) {
    (*vector_it)->idle(elapsed);
    if(_isClobbered) {
      _isClobbered = false;
      break;
    } else {
      vector_it++;
    }
  }
}

void RenderQueue::handleMouseEvent(int button, int state, int x, int y) {
  std::vector<Renderable*>::iterator vector_it = _sprites.begin();
  while(vector_it != _sprites.end()) {
    (*vector_it)->handleMouseEvent(button, state, x, y);
    if(_isClobbered) {
      _isClobbered = false;
      break;
    } else {
      vector_it++;
    }
  }
}

void RenderQueue::handleMouseDrag(int x, int y) {
  std::vector<Renderable*>::iterator vector_it = _sprites.begin();
  while(vector_it != _sprites.end()) {
    (*vector_it)->handleMouseDrag(x, y);
    if(_isClobbered) {
      _isClobbered = false;
      break;
    } else {
      vector_it++;
    }
  }
}

void RenderQueue::handleMouseMotion(int x, int y) {
  std::vector<Renderable*>::iterator vector_it = _sprites.begin();
  while(vector_it != _sprites.end()) {
    (*vector_it)->handleMouseMotion(x, y);
    if(_isClobbered) {
      _isClobbered = false;
      break;
    } else {
      vector_it++;
    }
  }
}

void RenderQueue::clear() {
  _sprites.clear();
}

void RenderQueue::postQueueClobbered() {
  // should be used when something in the queue modifies it during execution
  _isClobbered = true;
}

std::vector<Renderable*>::size_type RenderQueue::size() {
  return _sprites.size();
}
