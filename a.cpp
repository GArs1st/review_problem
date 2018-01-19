#include <iostream>
#include <memory>

template <typename T>
class Node{
 public:
    T val;
    std::shared_ptr<Node<T>> next;

    Node() {}

    Node(T val) : val(val) {}
};

template <typename T>
class List{
 public:
    List() {
        first = std::make_shared<Node<T>>();
        last = first;
        sz = 0;
    }

    void push_back(int val) {
        last->val = val;
        last->next = std::make_shared<Node<T>>();
        last = last->next;

        ++sz;
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        return size() == 0;
    }

    List<T> splitBy(int ind) {
        List<T> rightPart;
        int i = 0;
        for (auto it = begin(); it != end(); ++it, ++i) {
            if (i + 1 == ind) {
                rightPart.first = it.getNode()->next;
                rightPart.last = last;
                rightPart.sz = size() - ind;

                sz = ind;
                last = std::make_shared<Node<T>>();
                it.getNode()->next = last;
                break;
            }
        }

        return rightPart;
    }

    class Iterator{
     public:
        Iterator() {
            now = NULL;
        }

        Iterator(const std::shared_ptr<Node<T>>& ptr) {
            now = ptr;
        }

        std::shared_ptr<Node<T>> getNode() {
            return now;
        }

        bool operator == (const Iterator& other) const {
            return now == other.now;
        }

        bool operator != (const Iterator& other) const {
            return !(*this == other);
        }

        int operator * () const {
            return now->val;
        }

        Iterator& operator++ () {
            now = now->next;
            return *this;
        }

        Iterator operator++ (int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

     private:
        std::shared_ptr<Node<T>> now;
    };

    Iterator begin() const {
        return Iterator(first);
    }

    Iterator end() const {
        return Iterator(last);
    }

 private:
    std::shared_ptr<Node<T>> first;
    std::shared_ptr<Node<T>> last;
    size_t sz;
};

List<int> merge(const List<int>& list1, const List<int>& list2) {
    auto it1 = list1.begin(), it2 = list2.begin();
    List<int> list;

    while (it1 != list1.end() && it2 != list2.end()) {
        if (*it1 < *it2) {
            list.push_back(*it1);
            ++it1;
        } else {
            list.push_back(*it2);
            ++it2;
        }
    }

    for (; it1 != list1.end(); ++it1) {
        list.push_back(*it1);
    }

    for (; it2 != list2.end(); ++it2) {
        list.push_back(*it2);
    }

    return list;
}

void mergeSort(List<int>& list1) {
    if (list1.size() <= 1) return;

    int m = list1.size() / 2;
    List<int> list2 = list1.splitBy(m);
    mergeSort(list1);
    mergeSort(list2);

    list1 = merge(list1, list2);
}

int main() {
    int listSize;
    std::cin >> listSize;

    List<int> list;

    for (int i = 0; i < listSize; ++i) {
        int num;
        std::cin >> num;
        list.push_back(num);
    }

    mergeSort(list);

    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << (*it) << " ";
    }
    std::cout << "\n";

    return 0;
}

