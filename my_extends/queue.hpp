//
// Created by Gen on 16/1/6.
//

#ifndef GODOT_MAIN_QUEUE_HPP
#define GODOT_MAIN_QUEUE_HPP

#include "../../core/os/memory.h"

namespace MyTools {

    template <class T>
    class Queue {
        int _limit;
        int _size;
        int _offset;
        T *mem;

    public:

        void alloc(int p_size) {
            if (mem) {
                memdelete_arr(mem);
            }
            mem = p_size > 0 ? memnew_arr(T, p_size) : NULL;
            _size = 0;
            _offset = 0;
            _limit = p_size;
        }

        _FORCE_INLINE_ int limit() {return _limit;}

        const T &get(int index) const {
            return operator[](index);
        }

        T &get(int index) {
            return operator[](index);
        }

        void set(int index, T value) {
            if (index < _size) {
                operator[](index);
            }
        }

        _FORCE_INLINE_ T &operator [](int p_index) {
            if (p_index >= _size){
                T& aux=*((T*)0);
                ERR_FAIL_COND_V(p_index >= _size, aux);
                return aux;
            }
            else
                return mem[(_offset+p_index)%_limit];
        }

        int size() const {
            return _size;
        }

        T* push(const T &p_value) {
            int off = (_offset+_size)%_limit;
            mem[off] = p_value;
            _size ++;
            if (_size > _limit) {
                int m = _size-_limit;
                _size = _limit;
                _offset = (_offset+m)%_limit;
            }
            return &mem[off];
        }

        T pop() {
            if (_size > 0) {
                T res = mem[_offset++];
                _size--;
                return res;
            }else {
                return T();
            }
        }

        void clear() {
            _size = 0;
        }

        Queue() {
            mem = NULL;
            _limit = 0;
            _size = 0;
            _offset = 0;
        }
        ~Queue() {
            if (mem) {
                memdelete_arr(mem);
            }
        }
    };
}

#endif //GODOT_MAIN_QUEUE_HPP
