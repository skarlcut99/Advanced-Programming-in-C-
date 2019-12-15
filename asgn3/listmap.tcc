// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
// listmap::~listmap()
//

template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   while (!empty()) 
      erase(begin());
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}

//
// iterator listmap::insert (const value_type&)
//

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   Less l;
   listmap<Key,Value,Less>::iterator it = begin();
   while(it!=end() && l(pair.first,it->first)) 
      ++it;
   if(it!=end() && !l(it->first,pair.first)){
      it->second = pair.second;
      return it;
   }
   node* temp = new node(it.where, it.where->prev, pair);
   temp->next->prev = temp;
   temp->prev->next = temp;
   DEBUGF ('l', &pair << "->" << pair);
   return iterator(temp);
}

//
// listmap::find(const key_type&)
//

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   listmap<Key,Value,Less>::iterator it = begin();
   while(it!=end() && it->first != that) 
      ++it;
   DEBUGF ('l', that);
   return it;
}

//
// iterator listmap::erase (iterator position)
//

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   listmap<Key,Value,Less>::iterator it = position;
   ++it;
   position.erase();
   DEBUGF ('l', &*position);
   return it;
}

template <typename Key, typename Value, class Less>
bool listmap<Key,Value,Less>::empty() const{
   return anchor_.next == &anchor_;
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::begin() {
  return iterator (anchor_.next);
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::end() {
   return iterator (anchor());
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   DEBUGF ('l', where);
   return where->value;
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->(){
   DEBUGF ('l', where);
   return &(where->value);
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   DEBUGF ('l', where);
   where = where->next;
   return *this;
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   DEBUGF ('l', where);
   where = where->prev;
   return *this;
}

template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::iterator::erase() {
   if (where != nullptr){
      where->prev->next = where->next;
      where->next->prev = where->prev;
      delete where;
  }
}
