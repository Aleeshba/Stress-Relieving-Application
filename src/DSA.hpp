#include <iostream>
#include <cstring>
using namespace std;

// this file includes all the data structures that are used everywhere in the project
template <class T>
class Node
{
public:
    T data;
    Node *next;
    Node(T value, Node *n = NULL) : data(value), next(n) {}
};

template <class T>
class LinkedList
{
public:
    Node<T> *head;

    LinkedList() : head(NULL) {}

    void insert(T note)
    {
        Node<T> *newNode = new Node<T>(note);
        newNode->next = head;
        head = newNode;
    }

    void moveButton(int buttonIndex)
    {
        Node<T> *current = head;
        int index = 0;
        while (current && index < buttonIndex)
        {
            current = current->next;
            index++;
        }

        if (current)
        {
            current->data += 2;
        }
    }

    void display()
    {
        Node<T> *current = head;
        while (current)
        {
            current = current->next;
        }
    }

    ~LinkedList()
    {
        while (head)
        {
            Node<T> *temp = head;
            head = head->next;
            delete temp;
        }
    }
};

template <class T>
class CircularLinkedList
{
public:
    Node<T> *head;
    Node<T> *tail;
    Node<T> *current;

    CircularLinkedList() : head(NULL),
                           tail(NULL),
                           current(NULL) {}

    ~CircularLinkedList()
    {
        clear();
    }

    void addNode(T texture)
    {
        Node<T> *newNode = new Node<T>(texture);
        if (!head)
        {
            head = newNode;
            tail = newNode;
            newNode->next = head;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
            newNode->next = head;
        }
        current = head;
    }

    void nextNode()
    {
        if (current)
        {
            current = current->next;
        }
    }

    T getCurrentTexture()
    {
        return current ? current->data : NULL;
    }

    void clear()
    {
        if (head == NULL)
        {
            return;
        }
        Node<T> *current = head->next;
        while (current != head)
        {
            Node<T> *nextNode = current->next;
            delete current;
            current = nextNode;
        }

        delete head;
        head = tail = current = NULL;
    }
};
template <class T>
class stack
{
public:
    Node<T> *top;
    stack(Node<T> *t = NULL)
    {
        top = t;
    }
    bool empty()
    {
        return top == NULL;
    }
    void push(T d)
    {
        Node<T> *n = new Node<T>(d, top);
        top = n;
    }
    T peek()
    {
        if (empty())
        {
            cout << "Stack is empty!" << endl;
        }
        return top->data;
    }
    T pop()
    {
        if (empty())
            cout << "Stack is empty!" << endl;
        Node<T> *temp = top;
        T x = top->data;
        top = top->next;
        delete temp;
        return x;
    }
    ~stack()
    {
        Node<T> *ptr = top, *prev = top;
        while (ptr != NULL)
        {
            prev = ptr;
            ptr = ptr->next;
            delete prev;
        }
    }
};

template <class T>
class queue
{
private:
    Node<T> *front;
    Node<T> *rear;
    int elements;

public:
    queue() : front(NULL), rear(NULL), elements(0) {}
    ~queue(){};

    void push(T element)
    {
        Node<T> *newNode = new Node<T>(element);
        if (empty())
        {
            front = rear = newNode;
        }
        else
        {
            rear->next = newNode;
            rear = newNode;
        }
        elements++;
    }

    T pop()
    {
        if (empty())
        {
            cout << "Queue is empty" << endl;
        }

        T value = front->data;
        Node<T> *temp = front;

        if (front == rear)
        {
            front = rear = NULL;
        }
        else
        {
            front = front->next;
        }
        elements--;
        delete temp;
        return value;
    }

    int Size()
    {
        return elements;
    }

    T peek()
    {
        if (empty())
        {
            cout << "Queue is empty!" << endl;
        }
        return front->data;
    }

    void PrintElements()
    {
        Node<T> *ptr = front;
        if (empty())
            cout << "Queue is empty!" << endl;
        else
        {
            while (ptr != NULL)
            {
                cout << ptr->data << "\t";
                ptr = ptr->next;
            }
            cout << endl;
        }
    }

    bool empty()
    {
        return front == NULL;
    }
};

class node
{
public:
    string data;
    string dateTime; // current date and time
    node *next;
    node *prev;

    node(string value) : data(value), next(NULL), prev(NULL)
    {
        // Get current date and time
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char buffer[30];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
        dateTime = buffer;
    }

    string getData()
    {
        return data;
    }

    string getDateTime()
    {
        return dateTime;
    }
};

class DoublyLinkedList
{
public:
    node *head;
    node *tail;
    int s;

    DoublyLinkedList() : head(NULL), tail(NULL), s(0) {}

    ~DoublyLinkedList()
    {
        node *current = head;

        while (current != NULL)
        {
            node *next = current->next;
            delete current;
            current = next;
        }
    }
    // Inside the DoublyLinkedList class
    node *getNodeByIndex(int index)
    {
        node *current = head;
        int count = 1;

        while (current != NULL)
        {
            if (count == index)
            {
                return current;
            }

            current = current->next;
            count++;
        }

        return NULL; // Index out of range
    }

    void addNode(const string &data)
    {
        // Adding a new node at the tail
        node *newNode = new node(data);
        newNode->next = nullptr; // The new node will be the last, so its next is nullptr
        newNode->prev = tail;    // Set the prev pointer to the current tail

        if (tail != nullptr)
        {
            tail->next = newNode; // Update the next pointer of the current tail
        }
        else
        {
            // If the list was empty, set the head to the new node
            head = newNode;
        }

        tail = newNode; // Update the tail to be the new node
        // No need to sort the list when adding at the tail

        s++;
    }

    bool deleteNode(int j)
    {
        if (head == NULL || j < 1)
        {
            cout << "Invalid position or linked list is empty!" << endl;
            return false;
        }

        node *ptr = head;
        int i;

        for (i = 1; i < j && ptr->next != NULL; i++)
        {
            ptr = ptr->next;
        }

        if (j == 1)
        { // Delete first node
            head = (ptr->next != NULL) ? ptr->next : NULL;
            if (head != NULL)
            {
                head->prev = nullptr; // Update the prev pointer of the new head to nullptr
            }
            else
            {
                // The list is empty after deletion, update tail to nullptr
                tail = nullptr;
            }
        }
        else if (i == j)
        { // Delete mid or last node
            if (ptr->next != NULL)
            {
                ptr->next->prev = ptr->prev;
            }

            if (ptr->prev != NULL)
            {
                ptr->prev->next = ptr->next;
            }
            else
            {
                // Updating head when deleting the second node
                head = ptr->next;
                head->prev = nullptr; // Update the prev pointer of the new head to nullptr
            }

            if (ptr == tail)
            {
                tail = ptr->prev; // Update the tail to the previous node
            }
        }
        else
        {
            cout << "Invalid position!" << endl;
            return false;
        }

        cout << "Node at position " << j << " deleted." << endl;
        delete ptr;
        return true;
    }
    string displayAll()
    {
        string result;
        node *current = head;

        while (current != NULL)
        {
            result += "Date/Time: " + current->getDateTime() + "\n Note: " + current->getData() + "\n";
            current = current->next;
        }

        return result;
    }
    int size()
    {
        node *current = head;
        int count = 0;

        while (current != NULL)
        {
            current = current->next;
            count++;
        }

        return count;
    }

private:
    void sortList()
    {
        // Simple bubble sort for demonstration purposes
        if (!head)
            return;

        bool swapped;
        node *ptr1;
        node *lptr = NULL;

        do
        {
            swapped = false;
            ptr1 = head;

            while (ptr1->next != lptr)
            {
                // Compare date and time strings (format YYYY-MM-DD HH:MM:SS)
                if (ptr1->getDateTime() < ptr1->next->getDateTime())
                {
                    swap(ptr1, ptr1->next);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (swapped);
    }

    void swap(node *a, node *b)
    {
        std::swap(a->data, b->data);
        std::swap(a->dateTime, b->dateTime);
    }
};