//------------------------------------------------------------------------------

#ifndef RESIZE_IMAGE_WIDGET_HPP
#define RESIZE_IMAGE_WIDGET_HPP

//------------------------------------------------------------------------------

#include "../../common_lib/Common_facilities.hpp"

//------------------------------------------------------------------------------

namespace Imager {

//------------------------------------------------------------------------------

template<class T1, class T2>
using Range = std::pair<T1, T2>;

template<class T1, class T2>
auto make_range(const T1& t1, const T2& t2)
{
    return std::make_pair(t1, t2);
}

//------------------------------------------------------------------------------

inline auto align_right_vcenter()
{
    static auto align = Qt::AlignRight | Qt::AlignVCenter;
    return align;
}

//------------------------------------------------------------------------------

inline auto align_left_vcenter()
{
    static auto align = Qt::AlignLeft | Qt::AlignVCenter;
    return align;
}

//------------------------------------------------------------------------------

struct Compress_image_widget : public QWidget {
    Ref_unique_ptr<QLabel> orig_quality_label;
    Ref_unique_ptr<QLabel> orig_compress_label;
    Ref_unique_ptr<QSpinBox> quality_spin_box;
    Ref_unique_ptr<QSpinBox> compress_spin_box;

    explicit Compress_image_widget(QWidget* parent = nullptr);
};

//------------------------------------------------------------------------------

struct Resize_image_widget : public QWidget {
    Ref_unique_ptr<QLabel> orig_width_label;
    Ref_unique_ptr<QLabel> orig_height_label;
    Ref_unique_ptr<QSpinBox> width_spin_box;
    Ref_unique_ptr<QSpinBox> height_spin_box;
    Ref_unique_ptr<QDoubleSpinBox> width_percent_spin_box;
    Ref_unique_ptr<QDoubleSpinBox> height_percent_spin_box;
    Ref_unique_ptr<QCheckBox> keep_aspect_check_box;

    explicit Resize_image_widget(QWidget* parent = nullptr);
};

//------------------------------------------------------------------------------

inline void prepare_spinbox(QSpinBox& spbx, Range<int, int> range,
                            const QString& suffix = "",
                            Qt::Alignment flag = align_right_vcenter())
{
    spbx.setRange(range.first, range.second);
    spbx.setSuffix(suffix);
    spbx.setAlignment(flag);
}

//------------------------------------------------------------------------------

inline void prepare_spinbox(QDoubleSpinBox& spbx, Range<double, double> range,
                            const QString& suffix = "",
                            Qt::Alignment flag = align_right_vcenter())
{
    spbx.setRange(range.first, range.second);
    spbx.setDecimals(0);
    spbx.setSuffix(suffix);
    spbx.setAlignment(flag);
}

//------------------------------------------------------------------------------

inline auto get_equal_labels(const QString& lab,
                             int number_of_labels = 3)
{
    QVector<QLabel*> vl(number_of_labels);
    for (auto& e : vl) {
        e = new QLabel(lab);
        e->setAlignment(Qt::AlignCenter);
        e->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    }
    return vl;
}

//------------------------------------------------------------------------------

}   // Imager

//------------------------------------------------------------------------------

#endif // RESIZE_IMAGE_WIDGET_HPP

//------------------------------------------------------------------------------
