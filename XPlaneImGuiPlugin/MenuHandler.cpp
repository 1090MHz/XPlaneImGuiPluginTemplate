#include <XPLMDisplay.h>
#include <XPLMMenus.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class MenuItem
{
public:
    MenuItem(const std::string &title);
    ~MenuItem();

    void addSubItem(const std::string &title, std::function<void()> action);

private:
    static void menuHandler(void *mRef, void *iRef);

    XPLMMenuID m_menu_id;
    int m_item_id;
    std::map<int, std::function<void()>> m_actions;
    int m_next_item_id = 0;
};

MenuItem::MenuItem(const std::string &title)
{
    m_item_id = XPLMAppendMenuItem(XPLMFindPluginsMenu(), title.c_str(), nullptr, 1);
    m_menu_id = XPLMCreateMenu(title.c_str(), XPLMFindPluginsMenu(), m_item_id, menuHandler, this);
}

MenuItem::~MenuItem()
{
    XPLMClearAllMenuItems(m_menu_id);
    XPLMRemoveMenuItem(XPLMFindPluginsMenu(), m_item_id);
    XPLMDestroyMenu(m_menu_id);
}

void MenuItem::addSubItem(const std::string &title, std::function<void()> action)
{
    int item_id = m_next_item_id++;
    m_actions[item_id] = action;
    XPLMAppendMenuItem(m_menu_id, title.c_str(), reinterpret_cast<void *>(static_cast<intptr_t>(item_id)), 1);
}

void MenuItem::menuHandler(void *mRef, void *iRef)
{
    MenuItem *menu = static_cast<MenuItem *>(mRef);
    intptr_t item_id = reinterpret_cast<intptr_t>(iRef);
    auto it = menu->m_actions.find(static_cast<int>(item_id));
    if (it != menu->m_actions.end())
    {
        it->second();
    }
}