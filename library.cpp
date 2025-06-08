#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <list>
#include <algorithm>
#include <memory>

// Book class representing individual book
class Book {
public:
    std::string isbn;
    std::string title;
    std::string author;
    std::string genre;
    bool isAvailable;
    int borrowCount;

    Book(const std::string& isbn, const std::string& title, 
         const std::string& author, const std::string& genre)
        : isbn(isbn), title(title), author(author), genre(genre), 
          isAvailable(true), borrowCount(0) {}

    void display() const {
        std::cout << "ISBN: " << isbn << ", Title: " << title 
                  << ", Author: " << author << ", Genre: " << genre 
                  << ", Available: " << (isAvailable ? "Yes" : "No") << std::endl;
    }
};

// BST Node for organizing books by title or ISBN
class BSTNode {
public:
    std::shared_ptr<Book> book;
    std::shared_ptr<BSTNode> left;
    std::shared_ptr<BSTNode> right;

    BSTNode(std::shared_ptr<Book> b) : book(b), left(nullptr), right(nullptr) {}
};

// Binary Search Tree for fast book searching
class BST {
private:
    std::shared_ptr<BSTNode> root;

    std::shared_ptr<BSTNode> insert(std::shared_ptr<BSTNode> node, 
                                   std::shared_ptr<Book> book, 
                                   bool byTitle = true) {
        if (!node) {
            return std::make_shared<BSTNode>(book);
        }

        std::string key = byTitle ? book->title : book->isbn;
        std::string nodeKey = byTitle ? node->book->title : node->book->isbn;

        if (key < nodeKey) {
            node->left = insert(node->left, book, byTitle);
        } else if (key > nodeKey) {
            node->right = insert(node->right, book, byTitle);
        }
        return node;
    }

    std::shared_ptr<Book> search(std::shared_ptr<BSTNode> node, 
                                const std::string& key, 
                                bool byTitle = true) {
        if (!node) return nullptr;

        std::string nodeKey = byTitle ? node->book->title : node->book->isbn;

        if (key == nodeKey) {
            return node->book;
        } else if (key < nodeKey) {
            return search(node->left, key, byTitle);
        } else {
            return search(node->right, key, byTitle);
        }
    }

    void inorderTraversal(std::shared_ptr<BSTNode> node, 
                         std::vector<std::shared_ptr<Book>>& books) {
        if (node) {
            inorderTraversal(node->left, books);
            books.push_back(node->book);
            inorderTraversal(node->right, books);
        }
    }

public:
    BST() : root(nullptr) {}

    void insertBook(std::shared_ptr<Book> book, bool byTitle = true) {
        root = insert(root, book, byTitle);
    }

    std::shared_ptr<Book> searchBook(const std::string& key, bool byTitle = true) {
        return search(root, key, byTitle);
    }

    std::vector<std::shared_ptr<Book>> getAllBooks() {
        std::vector<std::shared_ptr<Book>> books;
        inorderTraversal(root, books);
        return books;
    }
};

// Borrow Request structure
struct BorrowRequest {
    std::string userID;
    std::string bookISBN;
    std::string action; // "BORROW" or "RETURN"
    
    BorrowRequest(const std::string& uid, const std::string& isbn, const std::string& act)
        : userID(uid), bookISBN(isbn), action(act) {}
};

// Graph for book recommendation system using adjacency list
class RecommendationGraph {
private:
    // Adjacency list: book -> set of connected books
    std::vector<std::pair<std::string, std::set<std::string>>> adjacencyList;
    // Genre-based grouping
    std::vector<std::pair<std::string, std::vector<std::string>>> genreGroups;

    int findBookIndex(const std::string& isbn) {
        for (size_t i = 0; i < adjacencyList.size(); i++) {
            if (adjacencyList[i].first == isbn) {
                return i;
            }
        }
        return -1;
    }

    int findGenreIndex(const std::string& genre) {
        for (size_t i = 0; i < genreGroups.size(); i++) {
            if (genreGroups[i].first == genre) {
                return i;
            }
        }
        return -1;
    }

public:
    void addBook(const std::string& isbn, const std::string& genre) {
        // Add to adjacency list
        adjacencyList.push_back({isbn, std::set<std::string>()});
        
        // Add to genre groups
        int genreIndex = findGenreIndex(genre);
        if (genreIndex == -1) {
            genreGroups.push_back({genre, std::vector<std::string>()});
            genreIndex = genreGroups.size() - 1;
        }
        genreGroups[genreIndex].second.push_back(isbn);
    }

    void addConnection(const std::string& book1, const std::string& book2) {
        int index1 = findBookIndex(book1);
        int index2 = findBookIndex(book2);
        
        if (index1 != -1 && index2 != -1) {
            adjacencyList[index1].second.insert(book2);
            adjacencyList[index2].second.insert(book1);
        }
    }

    void buildGenreConnections() {
        // Connect books of the same genre
        for (const auto& genrePair : genreGroups) {
            const auto& books = genrePair.second;
            for (size_t i = 0; i < books.size(); ++i) {
                for (size_t j = i + 1; j < books.size(); ++j) {
                    addConnection(books[i], books[j]);
                }
            }
        }
    }

    std::vector<std::string> getRecommendations(const std::string& isbn, int maxRecs = 5) {
        std::vector<std::string> recommendations;
        int bookIndex = findBookIndex(isbn);
        
        if (bookIndex != -1) {
            for (const auto& connectedBook : adjacencyList[bookIndex].second) {
                recommendations.push_back(connectedBook);
                if (recommendations.size() >= maxRecs) break;
            }
        }
        return recommendations;
    }
};

// Main Library Management System
class LibrarySystem {
private:
    // Data structures used (NO HASH MAP):
    std::list<std::shared_ptr<Book>> bookDatabase; // Linked List for book storage
    BST titleIndex; // BST for searching by title
    BST isbnIndex;  // BST for searching by ISBN
    std::queue<BorrowRequest> borrowQueue; // Queue for FIFO borrow requests
    std::stack<BorrowRequest> actionHistory; // Stack for undo functionality
    RecommendationGraph recommendationSystem; // Graph for recommendations
    
    // Genre-based storage (Tree-like structure using vector)
    std::vector<std::pair<std::string, std::list<std::shared_ptr<Book>>>> genreTree;

    // Helper function to find book by ISBN in linked list
    std::shared_ptr<Book> findBookByISBN(const std::string& isbn) {
        for (const auto& book : bookDatabase) {
            if (book->isbn == isbn) {
                return book;
            }
        }
        return nullptr;
    }

    // Helper function to find or create genre storage
    std::list<std::shared_ptr<Book>>* getGenreList(const std::string& genre) {
        for (auto& pair : genreTree) {
            if (pair.first == genre) {
                return &pair.second;
            }
        }
        // Create new genre storage
        genreTree.push_back({genre, std::list<std::shared_ptr<Book>>()});
        return &genreTree.back().second;
    }

public:
    // Store books per genre
    void addBook(const std::string& isbn, const std::string& title, 
                 const std::string& author, const std::string& genre) {
        auto book = std::make_shared<Book>(isbn, title, author, genre);
        
        // Store in linked list
        bookDatabase.push_back(book);
        
        // Insert into BSTs for fast searching
        titleIndex.insertBook(book, true);  // By title
        isbnIndex.insertBook(book, false);  // By ISBN
        
        // Store in genre tree
        auto genreList = getGenreList(genre);
        genreList->push_back(book);
        
        // Add to recommendation system
        recommendationSystem.addBook(isbn, genre);
        
        std::cout << "Book added successfully!" << std::endl;
    }

    // Organize books by title for fast searching
    std::shared_ptr<Book> searchByTitle(const std::string& title) {
        return titleIndex.searchBook(title, true);
    }

    // Organize books by ISBN for fast searching
    std::shared_ptr<Book> searchByISBN(const std::string& isbn) {
        return isbnIndex.searchBook(isbn, false);
    }

    // Display books by genre
    void displayBooksByGenre(const std::string& genre) {
        std::cout << "\n=== Books in genre: " << genre << " ===" << std::endl;
        
        for (const auto& pair : genreTree) {
            if (pair.first == genre) {
                if (pair.second.empty()) {
                    std::cout << "No books found in this genre." << std::endl;
                    return;
                }
                
                for (const auto& book : pair.second) {
                    book->display();
                }
                return;
            }
        }
        std::cout << "Genre not found." << std::endl;
    }

    // Book borrow requests (FIFO)
    void requestBorrow(const std::string& userID, const std::string& isbn) {
        borrowQueue.push(BorrowRequest(userID, isbn, "BORROW"));
        std::cout << "Borrow request added to queue." << std::endl;
    }

    // Book return requests (FIFO)
    void requestReturn(const std::string& userID, const std::string& isbn) {
        borrowQueue.push(BorrowRequest(userID, isbn, "RETURN"));
        std::cout << "Return request added to queue." << std::endl;
    }

    // Process next request in queue
    void processNextRequest() {
        if (borrowQueue.empty()) {
            std::cout << "No pending requests." << std::endl;
            return;
        }

        BorrowRequest request = borrowQueue.front();
        borrowQueue.pop();

        auto book = findBookByISBN(request.bookISBN);
        if (!book) {
            std::cout << "Book not found!" << std::endl;
            return;
        }

        if (request.action == "BORROW") {
            if (book->isAvailable) {
                book->isAvailable = false;
                book->borrowCount++;
                actionHistory.push(request);
                std::cout << "Book borrowed successfully by " << request.userID << std::endl;
            } else {
                std::cout << "Book is not available for borrowing." << std::endl;
            }
        } else if (request.action == "RETURN") {
            if (!book->isAvailable) {
                book->isAvailable = true;
                actionHistory.push(request);
                std::cout << "Book returned successfully by " << request.userID << std::endl;
            } else {
                std::cout << "Book was not borrowed." << std::endl;
            }
        }
    }

    // Undo last borrow/return actions
    void undoLastAction() {
        if (actionHistory.empty()) {
            std::cout << "No actions to undo." << std::endl;
            return;
        }

        BorrowRequest lastAction = actionHistory.top();
        actionHistory.pop();

        auto book = findBookByISBN(lastAction.bookISBN);
        if (book) {
            if (lastAction.action == "BORROW") {
                book->isAvailable = true;
                book->borrowCount--;
                std::cout << "Undid borrow action for " << lastAction.userID << std::endl;
            } else if (lastAction.action == "RETURN") {
                book->isAvailable = false;
                std::cout << "Undid return action for " << lastAction.userID << std::endl;
            }
        }
    }

    // Connect books by similarity (book recommendation system using genres)
    void buildRecommendations() {
        recommendationSystem.buildGenreConnections();
        std::cout << "Recommendation system built!" << std::endl;
    }

    // Get book recommendations
    void getRecommendations(const std::string& isbn) {
        auto recommendations = recommendationSystem.getRecommendations(isbn);
        
        std::cout << "\n=== Recommendations for ISBN: " << isbn << " ===" << std::endl;
        if (recommendations.empty()) {
            std::cout << "No recommendations available." << std::endl;
            return;
        }

        for (const auto& recISBN : recommendations) {
            auto book = findBookByISBN(recISBN);
            if (book) {
                std::cout << "Recommended: ";
                book->display();
            }
        }
    }

    // Display all books (sorted by title using BST)
    void displayAllBooks() {
        std::cout << "\n=== All Books (Sorted by Title) ===" << std::endl;
        auto books = titleIndex.getAllBooks();
        for (const auto& book : books) {
            book->display();
        }
    }

    // Display pending requests
    void displayPendingRequests() {
        std::cout << "\n=== Pending Requests ===" << std::endl;
        std::cout << "Number of pending requests: " << borrowQueue.size() << std::endl;
    }
};

// Demo function
void runDemo() {
    LibrarySystem library;

    std::cout << "=== Library Management & Recommendation System ===" << std::endl;

    // Store books per genre
    library.addBook("978-0134685991", "Effective Modern C++", "Scott Meyers", "Programming");
    library.addBook("978-0321563842", "The C++ Programming Language", "Bjarne Stroustrup", "Programming");
    library.addBook("978-0596809485", "JavaScript: The Good Parts", "Douglas Crockford", "Programming");
    library.addBook("978-0132350884", "Clean Code", "Robert Martin", "Programming");
    library.addBook("978-0201633610", "Design Patterns", "Gang of Four", "Software Engineering");
    library.addBook("978-1617294136", "Grokking Algorithms", "Aditya Bhargava", "Computer Science");

    // Display books by genre
    library.displayBooksByGenre("Programming");

    // Organize books by title or ISBN for fast searching
    std::cout << "\n=== Fast Search Demo ===" << std::endl;
    auto book = library.searchByTitle("Clean Code");
    if (book) {
        std::cout << "Found by title: ";
        book->display();
    }

    book = library.searchByISBN("978-0134685991");
    if (book) {
        std::cout << "Found by ISBN: ";
        book->display();
    }

    // Book borrow/return requests (FIFO)
    std::cout << "\n=== FIFO Borrow/Return Demo ===" << std::endl;
    library.requestBorrow("user123", "978-0134685991");
    library.requestBorrow("user456", "978-0321563842");
    library.requestReturn("user123", "978-0134685991");

    library.processNextRequest(); // Process first borrow
    library.processNextRequest(); // Process second borrow
    library.processNextRequest(); // Process return

    // Undo last borrow/return actions
    std::cout << "\n=== Undo Last Action Demo ===" << std::endl;
    library.undoLastAction();

    // Connect books by similarity (book recommendation system using genres)
    std::cout << "\n=== Book Recommendation System ===" << std::endl;
    library.buildRecommendations();
    library.getRecommendations("978-0134685991");

    library.displayPendingRequests();
    library.displayAllBooks();
}

int main() {
    runDemo();
    return 0;
}