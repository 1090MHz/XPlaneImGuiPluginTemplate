#ifndef MENUHANDLER_H
#define MENUHANDLER_H

#include <string>
#include <map>
#include <functional>

// Forward declaration of XPLMMenuID
typedef void *XPLMMenuID;

class MenuItem
{
public:
    // Constructor and Destructor
    MenuItem(const std::string &title);
    ~MenuItem();

    // Member functions
    void addSubItem(const std::string &title, std::function<void()> action);

private:
    // Member variables
    XPLMMenuID m_menu_id;
    int m_item_id;
    std::map<int, std::function<void()>> m_actions;
    int m_next_item_id = 0;
};

#endif // MENUHANDLER_H