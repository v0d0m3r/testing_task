//------------------------------------------------------------------------------

#ifndef COMMON_FACILITIES_HPP
#define COMMON_FACILITIES_HPP

//------------------------------------------------------------------------------
// Qt
#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>

//------------------------------------------------------------------------------
// std
#include <memory>

//------------------------------------------------------------------------------

using Byte = unsigned char;

//------------------------------------------------------------------------------

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T,std::function<void(T*)>>;

//------------------------------------------------------------------------------
// Ref_unique_ptr представляет собой пару
// 1) ссылку на объект
// 2) интелектуальный указатель
// Это необходимо для ситуации, когда освободили ресурс из указателя
// (например для владения к QVBoxLayout),
// но нужно обращаться к ресурсу по ссылке
template<typename T>
class Ref_unique_ptr {
public:
    explicit Ref_unique_ptr(T* tt)
        : p{std::pair<T&, std::unique_ptr<T>>{*tt, std::unique_ptr<T>{tt}}} {}

    const T& ref() const { return p.first; }
    T& ref() { return p.first; }

    const std::unique_ptr<T>& uptr() const { return p.second; }
    std::unique_ptr<T>& uptr() { return p.second; }
private:
    std::pair<T&, std::unique_ptr<T>> p;
};

//------------------------------------------------------------------------------

template<typename T>
T* do_release(Ref_unique_ptr<T>& r)
{
    return r.uptr().release();
}

//------------------------------------------------------------------------------

#endif // COMMON_FACILITIES_HPP

//------------------------------------------------------------------------------
