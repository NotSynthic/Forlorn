#ifndef _ANIMATEDSPRITE_H
#define _ANIMATEDSPRITE_H

#include <axmol.h>
#include "ForlornUtils.hpp"
 
class AnimatedSprite : public ax::Sprite
{ 
    public:
        static AnimatedSprite* createWithSpriteFrameName(const std::string& filename) {
            AnimatedSprite* sprite = new AnimatedSprite();
            
            if (sprite && sprite->initWithSpriteFrameName(filename)) {
                sprite->autorelease();
                return sprite;
            } else {
                delete sprite;
                return nullptr;
            }
        }
        ax::Action* currentAnim;
        void runAnimation(std::string animName, float fps, bool loop)
        {
            stopAction(currentAnim);
            ax::Vector<ax::SpriteFrame*> frames;

            std::string loopedAnim = animName;
            if (loopedAnim.find_last_of(".") != std::string::npos)
                loopedAnim.insert(loopedAnim.find_last_of("."), "_looped_001");

            std::string loopedAnimName = loopedAnim;
                
            ForlornUtils::replaceString(animName, "_01.", "_%02d.");
            ForlornUtils::replaceString(animName, "_001.", "_%03d.");
            ForlornUtils::replaceString(loopedAnimName, "_01.", "_%02d.");
            ForlornUtils::replaceString(loopedAnimName, "_001.", "_%03d.");

            int frameIndex = 1;
            while (true) 
            {
                std::string frameName = ax::StringUtils::format((loopedAnim.find("_looped_001.") != std::string::npos) ? loopedAnimName.c_str() : animName.c_str(), frameIndex);
                ax::SpriteFrame* frame = ax::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);

                if (frame) 
                {
                    frames.pushBack(frame);
                    frameIndex++;
                } 
                else {
                    break;
                }
            }

            ax::Animation* animation = ax::Animation::createWithSpriteFrames(frames, 1.0f / fps);
            ax::Animate* animate = ax::Animate::create(animation);
            currentAnim = loop ? ax::RepeatForever::create(animate) : static_cast<ax::Action*>(animate);
            runAction(currentAnim);
        };
};
#endif