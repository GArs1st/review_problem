#include <initializer_list>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
 public:
    HashMap(const Hash& hasher = Hash())
            : hasher(hasher) {
        clear();
    }

    template<typename Iter>
    HashMap(Iter begin, Iter end, const Hash& hasher = Hash())
            : hasher(hasher) {
        clear();

        while (begin != end) {
            insert(*begin);
            ++begin;
        }
    }

    HashMap(const std::initializer_list<std::pair<const KeyType, ValueType>>& items,
            const Hash& hasher = Hash()) {
        HashMap(items.begin(), items.end(), hasher);
    }

    void insert(const std::pair<const KeyType, ValueType>& item) {
        if (static_cast<double>(sz) / data.size() >= LOAD_FACTOR_THRESHOLD) {
            rebuild();
        }

        size_t ind = hasher(item.first) % data.size();

        for (auto it : data[ind]) {
            if (it.first == item.first) return;
        }

        ++sz;
        data[ind].push_back(item);
    }

    void erase(const KeyType& key) {
        size_t ind = hasher(key) % data.size();

        for (auto it = data[ind].begin(); it != data[ind].end(); ++it) {
            if (it->first == key) {
                data[ind].erase(it);
                --sz;
                return;
            }
        }
    }

    void rebuild() {
        auto dataCopy = data;
        size_t prevSize = data.size();
        data.clear();
        data.resize(prevSize * 2);
        sz = 0;

        for (auto lst : dataCopy) {
            for (auto it : lst) {
                insert(it);
            }
        }
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        return size() == 0;
    }

    Hash hash_function() const {
        return hasher;
    }

    template<bool isConst>
    class AnyIterator {
     public:
        using iterator_category = std::forward_iterator_tag;
        using reference = typename std::conditional<isConst, const std::pair<const KeyType,
                ValueType>&, std::pair<const KeyType, ValueType>&>::type;
        using pointer = typename std::conditional<isConst, const std::pair<const KeyType,
                ValueType>*, std::pair<const KeyType, ValueType>*>::type;
        using in_type = typename std::conditional<isConst, const HashMap<KeyType,
                ValueType, Hash>*, HashMap<KeyType, ValueType, Hash>*>::type;
        using list_iterator = typename std::conditional<isConst, typename std::list<std::pair<
                const KeyType, ValueType>>::const_iterator, typename std::list<std::pair<
                const KeyType, ValueType>>::iterator>::type;

        AnyIterator() {}

        AnyIterator(in_type hm, size_t ind, list_iterator now)
                : hashMap(hm)
                , ind(ind)
                , now(now) {}

        AnyIterator(in_type hm, size_t ind)
                : hashMap(hm)
                , ind(ind) {
            if (ind >= hashMap->data.size()) {
                this->ind = hashMap->data.size();
            } else {
                now = hashMap->data[ind].begin();
                if (hashMap->data[ind].empty()) {
                    ++(*this);
                }
            }
        }

        AnyIterator(in_type hm)
                : hashMap(hm) {
            ind = 0;
            now = hashMap->data[ind].begin();
            if (hashMap->data[ind].empty()) {
                ++(*this);
            }
        }

        bool operator == (const AnyIterator& other) const {
            if (hashMap != other.hashMap) return false;  // Exception?
            if (ind >= hashMap->data.size() && other.ind >= other.hashMap->data.size()) return true;
            return ind == other.ind && now == other.now;
        }

        bool operator != (const AnyIterator& other) const {
            return !(*this == other);
        }

        reference operator * () {
            return *now;
        }

        pointer operator -> () {
            return &(operator*());
        }

        AnyIterator& operator++ () {
            if (now != hashMap->data[ind].end()) ++now;

            if (now == hashMap->data[ind].end()) {
                ++ind;
                while (ind < hashMap->data.size() && hashMap->data[ind].empty()) ++ind;
                if (ind >= hashMap->data.size()) return *this;
                now = hashMap->data[ind].begin();
            }

            return *this;
        }

        AnyIterator operator++ (int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        size_t getInd() const {
            return ind;
        }

     private:
        in_type hashMap;
        size_t ind;
        list_iterator now;
    };

    using iterator = AnyIterator<false>;
    using const_iterator = AnyIterator<true>;

    iterator begin() {
        return iterator(this);
    }

    iterator end() {
        return iterator(this, data.size());
    }

    const_iterator begin() const {
        return const_iterator(this);
    }

    const_iterator end() const {
        return const_iterator(this, data.size());
    }

    iterator find(const KeyType& key) {
        size_t ind = hasher(key) % data.size();

        for (auto it = data[ind].begin(); it != data[ind].end(); ++it) {
            if (key == it->first) {
                return iterator(this, ind, it);
            }
        }

        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t ind = hasher(key) % data.size();

        for (auto it = data[ind].begin(); it != data[ind].end(); ++it) {
            if (key == it->first) {
                return const_iterator(this, ind, it);
            }
        }

        return end();
    }

    ValueType& operator[] (const KeyType& key) {
        iterator it = find(key);
        if (it == end()) {
            insert({key, ValueType()});
            it = find(key);
        }

        return it->second;
    }

    const ValueType& at(const KeyType& key) const {
        const_iterator it = find(key);
        if (it == end()) {
            throw std::out_of_range("Such key doesn't exist.");
        }

        return it->second;
    }

    void clear() {
        sz = 0;
        data.clear();
        data.resize(START_SIZE);
    }

    HashMap& operator= (HashMap other) {
        clear();

        for (auto &item : other) {
            insert(item);
        }

        return *this;
    }

 private:
    size_t sz;
    std::vector<std::list<std::pair<const KeyType, ValueType>>> data;
    Hash hasher;
    static constexpr double LOAD_FACTOR_THRESHOLD = 0.75;
    static constexpr int START_SIZE = 1;
};

