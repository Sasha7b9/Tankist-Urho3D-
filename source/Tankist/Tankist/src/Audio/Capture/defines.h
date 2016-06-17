#pragma once


typedef short int16;

#define alignas(n) __declspec(align(n))

template <class type> class Storage
{
private:

    alignas(16) char	storage[sizeof(type)];

public:

    operator type *(void)
    {
        return (reinterpret_cast<type *>(storage));
    }

    operator const type *(void) const
    {
        return (reinterpret_cast<const type *>(storage));
    }

    type *operator ->(void)
    {
        return (reinterpret_cast<type *>(storage));
    }

    const type *operator ->(void) const
    {
        return (reinterpret_cast<const type *>(storage));
    }
};
