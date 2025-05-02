#include <iostream>
#include <unordered_map>
#include <map>
#include <memory>
#include <stdexcept>

class DuplicateItemException : public std::runtime_error {
public:
    DuplicateItemException(const std::string& msg) : std::runtime_error(msg) {}
};

class ItemNotFoundException : public std::runtime_error {
public:
    ItemNotFoundException(const std::string& msg) : std::runtime_error(msg) {}
};

class StoredItem {
private:
    std::string id;
    std::string description;
    std::string location;

public:
    StoredItem(std::string id, std::string desc, std::string loc)
        : id(id), description(desc), location(loc) {    }

    std::string getId() const { return id; }
    std::string getDescription() const { return description; }
    std::string getLocation() const { return location; }
};

class StorageManager {
private:
    std::unordered_map<std::string, std::shared_ptr<StoredItem>> itemById;
    std::map<std::string, std::shared_ptr<StoredItem>> itemByDescription;

public:
    void addItem(const std::shared_ptr<StoredItem>& item) {

        // searches if item exists by comparing the itemByID
        if (itemById.find(item->getId()) != itemById.end()) {
            throw DuplicateItemException("Item with ID " + item->getId() + " already exists.");
        }

        //else...
        // Insert into both maps
        itemById[item->getId()] = item;
        itemByDescription[item->getDescription()] = item;

        std::cout << "~~~~~~~~~~~~~~~~~~~~~~\nAdding item: " << item->getId() << " - " << item->getDescription() << std::endl;
    }

    std::shared_ptr<StoredItem> findById(const std::string& id) const {

        auto it = itemById.find(id);

        if (it != itemById.end()) {
            return it->second;
        }
        else {
            throw ItemNotFoundException("No item with ID '" + id + "' found.");
        }
    }

    void removeItem(const std::string& id) {
        auto it = itemById.find(id);

        if (it == itemById.end()) {
            throw ItemNotFoundException("No item with ID '" + id + "' found.\nItem removal cancelled.");
        }

        //get description from ordered map to be removed
        std::string description = it->second->getDescription();

        // Display removal message before deletion
        std::cout << "Removing item: " << id << " (" << description << ")" << std::endl;  

        // Remove from both maps
        itemById.erase(it);
        itemByDescription.erase(description);
    }

    void listItemsByDescription() const {

        if (itemByDescription.empty()) {
            std::cout << "No items to display.\n";
            return;
        }
        // iterate over the map using a valid key-value pair extraction
        for (const auto& pair : itemByDescription) {
            std::shared_ptr<StoredItem> item = pair.second; // Access stored item
            std::cout << "- " << item->getDescription() << ": "
                << item->getLocation() << '\n';
        }
    }


    void testDuplicateAddition(const std::shared_ptr<StoredItem>& dupeItem) {

        try {
            addItem(dupeItem);
            std::cout << "Items added successfully.\n";
        }
        catch (const DuplicateItemException& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }

    std::shared_ptr<StoredItem> testItemNotFound(const std::string& id) const {
        auto it = itemById.find(id);

        if (it != itemById.end()) {
            return it->second;
        }
        else {
            throw ItemNotFoundException("No item with ID '" + id + "' found.");
        }
    }

};

int main() {
    // create the storage manager class item
    StorageManager manager;

    // create two items to store in the class 
    auto testItem1 = std::make_shared<StoredItem>("ITEM001", "Red/Blue JoyCons", "Aisle 1, shelf 1");
    auto testItem2 = std::make_shared<StoredItem>("ITEM002", "Silver Dualshock 5 Controller", "Aisle 1, shelf 2");

    // add item1 and item2 to the storageManager class
    try {
        manager.addItem(testItem1);
        manager.addItem(testItem2);
    }
    catch (const DuplicateItemException& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    // Attempt to find an item by ID
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~\nRetrieving ITEM002...\n";
    try {
        // Retrieve item by ID and its getters
        std::shared_ptr<StoredItem> foundItem = manager.findById("ITEM002");
        std::cout << "ID: " << foundItem->getId() << '\n';
        std::cout << "Description: " << foundItem->getDescription() << '\n';
        std::cout << "Location: " << foundItem->getLocation() << '\n';
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    // Attempt to remove a non-existent item and catch the exception
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~\nAttempting to remove ITEM025...\n";
    try {
        manager.removeItem("ITEM025"); // ITEM025 does not exist
    }
    catch (const ItemNotFoundException& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    // List items in order of description
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~\nItems in Description Order:\n";
    manager.listItemsByDescription();

    //Test duplicate item addition
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~\nAttempting to add item ITEM001 again:\n";
    manager.testDuplicateAddition(testItem1);

    // Test nonexistant item search
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~\nAttempting to find ITEMXXX:\n";
    try {
        manager.testItemNotFound("ITEMXXX");
    } catch (const ItemNotFoundException& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    // Removing an item and replacing it with another
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~\nAttempting to remove ITEM002\n";
    try {
        manager.removeItem("ITEM002");
    }
    catch (const ItemNotFoundException& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    // List items again to show deletion
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~\nItems in Description Order:\n";
    manager.listItemsByDescription();

    // add a new item to the system
    auto testItem3 = std::make_shared<StoredItem>("ITEM002", "Grey JoyCons", "Aisle 1, shelf 1");
    try {
        manager.addItem(testItem3);
    }
    catch (const DuplicateItemException& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    // Finally, display the list one more time with the new item
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~\nItems in Description Order:\n";
    manager.listItemsByDescription();

    return 0;
}