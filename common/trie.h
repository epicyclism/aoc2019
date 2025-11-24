//
// Copyright (c) 2021 Paul Ranson, paul@epicyclism.com
//
#pragma once

#include <algorithm>
#include <vector>
#include <optional>
#include <string_view>

namespace TRIE
{
    template<typename V> struct node_t
    {
        static_assert(std::is_integral_v<V>, "trie value type must be integral");
        static constexpr V invalid_val = static_cast<V>(-1);
        using val_type = V;

        char key_;
        std::vector<node_t> children_;
        val_type val_ ;
        node_t() : key_{0}, val_ {invalid_val}
        {}
        node_t(char k) : key_{k}, val_ {invalid_val}
        {}
        bool valid() const
        {
            return val_ != invalid_val;
        }
    };

    template<typename V> std::optional<V> find(node_t<V> const* node, std::string_view key)
    {
        for(auto k : key)
        {
            auto nn = std::find_if(node->children_.begin(), node->children_.end(), [k](auto const& nn){ return nn.key_ == k;});
            if( nn == node->children_.end())
                return std::nullopt;
            node = &*nn;
        }
        return node->valid() ? std::optional<V>{node->val_} : std::nullopt;
    }
     template<typename V> std::size_t find_depth(node_t<V> const* node, std::string_view key)
    {
        std::size_t d{ 0 };
        std::size_t c{ 0 };
        for (auto k : key)
        {
            ++c;
            auto nn = std::find_if(node->children_.begin(), node->children_.end(), [k](auto const& nn) { return nn.key_ == k; });
            if (nn == node->children_.end())
                break;
            node = &*nn;
            if (node->valid())
                d = c;
        }
        return d;
    }
    template<typename V> void insert(node_t<V>* node, std::string_view key, V val)
    {
        for(auto k : key)
        {
            auto nn = std::find_if(node->children_.begin(), node->children_.end(), [k](auto const& nn){ return nn.key_ == k;});
            node = (nn == node->children_.end()) ? &node->children_.emplace_back(k) : &*nn;
        }
        node->val_ = val;
    }
} // namespace TRIE

template<typename V> class trie_t
{
    private:
        TRIE::node_t<V> root_;
    public:
        using val_type = V;

        void insert(std::string_view key, V val)
        {
            TRIE::insert(&root_, key, val);
        }
        [[nodiscard]] std::optional<V> find(std::string_view key) const
        {
            return TRIE::find(&root_, key);
        }
        [[nodiscard]] std::size_t find_depth(std::string_view key) const
        {
            return TRIE::find_depth(&root_, key);
        }
        [[nodiscard]] bool empty() const
        {
            return root_.children_.empty();
        }
};