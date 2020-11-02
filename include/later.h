#ifndef __LATER_H
#define __LATER_H

class Later {
public:
    template <class callable, class... arguments>
    Later(int after, bool async, callable&& f, arguments&&... args);
};

#endif