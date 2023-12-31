#include "PlayScene.hpp"
#include "ForlornUtils.hpp"
#include "JsonUtils.hpp"
#include "AnimatedSprite.hpp"
#include <fmt/format.h>
#include <json.hpp>
#include "Nodes/Block.hpp"
#include <ImGuiPresenter.h>


USING_NS_AX;

PlayScene* PlayScene::create(std::string_view levelfile) {
    auto level = new PlayScene();
    if (level && level->initWithFile(levelfile))
    {
        level->autorelease();
        return level;
    }
    return nullptr;
}

bool PlayScene::init() {
    if (!Node::init())
        return false;

    //scheduleUpdate();
    return true;
}

void PlayScene::update(float dt)
{
    auto cam = Camera::getDefaultCamera();
    constexpr float moveSpeed = 7.0f;

    if (upPressed) cam->setPositionY(cam->getPositionY() + moveSpeed);
    if (downPressed) cam->setPositionY(cam->getPositionY() - moveSpeed);
    if (leftPressed) cam->setPositionX(cam->getPositionX() - moveSpeed);
    if (rightPressed) cam->setPositionX(cam->getPositionX() + moveSpeed);

    if (_parallax) {
        _parallax->setPosition(Vec2(-465 + (-cam->getPosition().x * -0.25), -430 + (-cam->getPosition().y * -0.25)));
    }
    if (_bg)
    {
        _bg->setPosition(Vec2(-465 + (-cam->getPosition().x * -0.75), 430 + (-cam->getPosition().y * -0.75)));
    }
}



bool PlayScene::initWithFile(std::string_view filename) {

    tempStatusLabel = Label::createWithBMFont("bigFont.fnt", "Loading");
    ForlornUtils::setCenter(tempStatusLabel);
    addChild(tempStatusLabel);



    scheduleOnce([this, filename](float)
    {
        this->initialLoading(filename);
        tempStatusLabel->setVisible(false);
        extension::ImGuiPresenter::getInstance()->addRenderLoop("__PLAYSCENE__", AX_CALLBACK_0(PlayScene::drawImgui, this), Director::getInstance()->getRunningScene());

        auto keyboardListener = EventListenerKeyboard::create();
        keyboardListener->onKeyPressed = AX_CALLBACK_2(PlayScene::onKeyPressed, this);
        keyboardListener->onKeyReleased = AX_CALLBACK_2(PlayScene::onKeyReleased, this);
        _eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, 11);

    }, 0.0f, "playlayer_load");

    scheduleUpdate();

    return true;
}

void PlayScene::initialLoading(std::string_view levelFileName)
{
    std::string levelData = FileUtils::getInstance()->getStringFromFile(levelFileName);
    try
    {
        readPlist(json::parse(levelData));
    }
    catch (std::exception& e)
    {
        fmt::println("exception while loading level: {}", e.what());
    }
    
}

void PlayScene::readPlist(const json::Value& level)
{
    if (auto playerSpawn = JsonUtils::Vec2FromArrayString(level["playerSpawn"].as_string()))
    {
        auto cam = Camera::getDefaultCamera();
        cam->setPosition(playerSpawn.value());
        cam->setZoom(cam->getZoom() * 0.5f);
    }

    fmt::println("loading spritesheets");
    for (const auto& sheet : level["sheetContainer"].as_object())
    {
        if (auto sheetStr = JsonUtils::fromObject<std::string>(sheet.second.as_object(), "texture"))
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fmt::format("{}.plist", sheetStr.value()));
    }

    fmt::println("loading finished");
    createBackground(level["settings"].as_object());
    createParallax(level["bgContainer"].as_object());
    loadBlocks(level["blockContainer"].as_object());
}

void PlayScene::drawImgui()
{
    static bool showLabels = true;
    ImGui::Begin("PlayScene Debug");
    if (ImGui::Checkbox("Block ID Labels", &showLabels))
    {
        setBlockLabelsVisible(showLabels);
    }

    static bool showParallax = true;
    if (ImGui::Checkbox("Show parallax node", &showParallax))
    {
        if (_parallax) _parallax->setVisible(showParallax);
    }

    ImGui::End();

}

void PlayScene::setBlockLabelsVisible(bool on)
{
    for (const auto& node : getChildren())
    {
        if (dynamic_cast<ax::Label*>(node))
        {
            node->setVisible(on);
        }
    }
}

void PlayScene::initVars()
{
}

void PlayScene::createPlayer()
{
}

void PlayScene::createUI()
{
}

void PlayScene::loadBlocks(const json::Object& blockContainer)
{
    for (const auto& obj : blockContainer)
    {

        if (auto block = Block::create(obj.second.as_object(), false))
        {
            auto label = Label::createWithBMFont("bigFont.fnt", obj.first);
            label->setPosition(block->getPosition());
            label->setScale(.2f);
            addChild(label, 9999);
            addChild(block, block->_p_uID);
        }
    }
}

void PlayScene::createBackground(const json::Object& bgSettings)
{
    _bg = Sprite::create(JsonUtils::fromObject<std::string>(bgSettings, "bgImage").value()+".png");
    if(_bg)
    {
        _bg->setPosition(Camera::getDefaultCamera()->getPosition());
        addChild(_bg);
    }
}

void PlayScene::createParallax(const json::Object& bgContainer)
{
    _parallax = ParallaxNode::create();
    addChild(_parallax);

    for (const auto& obj : bgContainer)
    {
        if(auto block = Block::create(obj.second.as_object(), true))
            _parallax->addChild(block, block->_p_uID, Vec2(1, 1), block->getPosition());
    }
}

void PlayScene::onKeyPressed(ax::EventKeyboard::KeyCode code, ax::Event* event) {

    switch (code) {
    case EventKeyboard::KeyCode::KEY_W: upPressed = true; break;
    case EventKeyboard::KeyCode::KEY_A: leftPressed = true; break;
    case EventKeyboard::KeyCode::KEY_S: downPressed = true; break;
    case EventKeyboard::KeyCode::KEY_D: rightPressed = true; break;
    default: break;
    }
}

void PlayScene::onKeyReleased(ax::EventKeyboard::KeyCode code, ax::Event* event) {
    switch (code) {
    case EventKeyboard::KeyCode::KEY_W: upPressed = false; break;
    case EventKeyboard::KeyCode::KEY_A: leftPressed = false; break;
    case EventKeyboard::KeyCode::KEY_S: downPressed = false; break;
    case EventKeyboard::KeyCode::KEY_D: rightPressed = false; break;
    default: break;
    }

}
