#pragma once

template<class type>
struct PTRNode {
	type data;
	ListNode* next;

    PTRNode() {
		this->data = type();
		this->next = nullptr;
	}

    PTRNode(type data) {
		this->data = data;
		this->next = nullptr;
	}

    PTRNode(const PTRNode& other) {
		this->data = other.data;
		this->next = nullptr;
	}

    PTRNode& operator=(const PTRNode& other)
	{
		data = other.data;
		next = nullptr;
		return *this;
	}
};

template <class type>
class LinkedList {
public:
	typedef PTRNode<type> Node;
    LinkedList()
    {
        head = nullptr;
        tail = nullptr;
    }

    LinkedList(const LinkedList<type>& other)
    {
        head = nullptr;
        tail = nullptr;
        for (Node* temp = other.head; temp != nullptr; temp = temp->next)
        {
            push_back(temp->data);
        }
    }

    LinkedList<type>& operator=(const LinkedList<type>& other)
    {
        for (Node* temp = head; temp != nullptr; temp = head)
        {
            head = head->next;
            delete temp;
        }
        head = nullptr;
        tail = nullptr;
        for (Node* temp = other.head; temp != nullptr; temp = temp->next)
        {
            push_back(temp->data);
        }
        return *this;
    }

    ~LinkedList()
    {
        for (Node* temp = head; temp != nullptr; temp = head)
        {
            head = head->next;
            delete temp;
        }
    }

    void push_back(type data)
    {
        Node* temp = new Node(data);
        if (head == nullptr)
        {
            head = tail = temp;
        }
        else
        {
            tail->next = temp;
            tail = temp;
        }
    }

    void push_front(type data) {
        Node* temp = new Node(data);
        if (head == nullptr)
        {
            head = tail = temp;
        }
        else
        {
            temp->next = head;
            head = temp;
        }
    }

    void pop_back()
    {
        if (head == nullptr)
        {
            return;
        }
        else if (head == tail)
        {
            delete head;
            head = tail = nullptr;
        }
        else
        {
            Node* temp = head;
            while (temp->next != tail)
            {
                temp = temp->next;
            }
            delete tail;
            tail = temp;
            tail->next = nullptr;
        }
    }

    void pop_front() {
        if (head == nullptr) {
            return;
        }
        else if (head == tail) {
            delete head;
            head = tail = nullptr;
        }
        else {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

	type front() {
		return head->data;
	}

	type back()
	{
		if (tail == nullptr) return type();
		return tail->data;
	}

	bool empty() {
		return head == nullptr;
	}
protected:
	Node *head;
	Node *tail;
};