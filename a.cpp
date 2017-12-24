#include <iostream>
#include <memory>

class Node{
 public:
    int val;
    std::shared_ptr<Node> next;

    Node(int val) : val(val) {}
};

class List{
 public:
    List() {
        first = last = NULL;
        sz = 0;
    }

    void push_back(int val) {
        if (empty()) {
            first = last = std::make_shared<Node>(val);
        } else {
            last->next = std::make_shared<Node>(val);
            last = last->next;
        }

        ++sz;
    }

    size_t size() const {
        return sz;
    }

    bool empty() {
        return (size() == 0);
    }

    List splitBy(int ind) {
        List list1, list2;
        int i = 0;
        for (auto it = begin(); it != end(); ++it, ++i) {
            if (i < ind) {
                list1.push_back(*it);
            } else {
                list2.push_back(*it);
            }
        }

        *this = list1;
        return list2;
    }

    class Iterator{
     public:
        Iterator() {
            now = NULL;
        }

        Iterator(const std::shared_ptr<Node>& ptr) {
            now = ptr;
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
        std::shared_ptr<Node> now;
    };

    Iterator begin() const {
        return Iterator(first);
    }

    Iterator end() const {
        return Iterator();
    }

 private:
    std::shared_ptr<Node> first, last;
    size_t sz;
};

List merge(const List& list1, const List& list2) {
    auto it1 = list1.begin(), it2 = list2.begin();
    List list;

    while (it1 != list1.end() || it2 != list2.end()) {
        if (it2 == list2.end() || (it1 != list1.end() && *it1 < *it2)) {
            list.push_back(*it1);
            ++it1;
        } else {
            list.push_back(*it2);
            ++it2;
        }
    }

    return list;
}

void mergeSort(List& list) {
    if (list.size() <= 1) return;

    int m = list.size() / 2;
    List list2 = list.splitBy(m);
    mergeSort(list);
    mergeSort(list2);

    list = merge(list, list2);
}

int main() {
    int n;
    std::cin >> n;

    List a;

    for (int i = 0; i < n; ++i) {
        int tmp;
        std::cin >> tmp;
        a.push_back(tmp);
    }

    mergeSort(a);

    for (auto it = a.begin(); it != a.end(); ++it) {
        std::cout << (*it) << " ";
    }
    std::cout << "\n";

    return 0;
}

