//------------------------------------------------------------------------------

#include "Resize_image_widget.hpp"

//------------------------------------------------------------------------------

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPointer>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

//------------------------------------------------------------------------------

namespace Imager {

//------------------------------------------------------------------------------

Compress_image_widget::Compress_image_widget(QWidget* parent)
    : QWidget(parent),
      orig_quality_label(new QLabel()),
      orig_compress_label(new QLabel()),
      quality_spin_box(new QSpinBox()),
      compress_spin_box(new QSpinBox())
{
    setGeometry(0, 0, 363, 125);

    auto main_layout = std::make_unique<QGridLayout>();
    main_layout->addWidget(new QLabel(tr("Quality")), 0, 1);
    main_layout->itemAt(main_layout->count()-1)->setAlignment(align_right_vcenter());

    main_layout->addWidget(new QLabel(tr("Compress")), 0, 3);

    main_layout->addWidget(new QLabel(tr("Current values: ")), 1, 0);
    main_layout->itemAt(main_layout->count()-1)->setAlignment(align_right_vcenter());

    auto interact_flag = Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse;
    orig_quality_label.ref().setTextInteractionFlags(interact_flag);
    orig_quality_label.ref().setAlignment(align_right_vcenter());
    main_layout->addWidget(do_release(orig_quality_label), 1, 1);

    auto label_tb(get_equal_labels(tr("  "), 2));
    main_layout->addWidget(label_tb.at(0),1, 2);
    orig_compress_label.ref().setTextInteractionFlags(interact_flag);
    main_layout->addWidget(do_release(orig_compress_label),1, 3);

    auto new_size_label = std::make_unique<QLabel>(tr("&New values: "));
    new_size_label->setAlignment(align_right_vcenter());
    new_size_label->setBuddy(quality_spin_box.uptr().get());
    main_layout->addWidget(new_size_label.release(), 2, 0);

    prepare_spinbox(quality_spin_box.ref(), make_range(-1, 100));
    main_layout->addWidget(do_release(quality_spin_box), 2, 1);
    main_layout->addWidget(label_tb.at(1), 2, 2);
    prepare_spinbox(compress_spin_box.ref(), make_range(-1, 9));
    main_layout->addWidget(do_release(compress_spin_box), 2, 3);

    setLayout(main_layout.release());
}

//------------------------------------------------------------------------------

Resize_image_widget::Resize_image_widget(QWidget* parent)
    : QWidget(parent),
      orig_width_label(new QLabel()),
      orig_height_label(new QLabel()),
      width_spin_box(new QSpinBox()),
      height_spin_box(new QSpinBox()),
      width_percent_spin_box(new QDoubleSpinBox),
      height_percent_spin_box(new QDoubleSpinBox),
      keep_aspect_check_box(new QCheckBox(tr("Keep aspect ratio")))
{
    setGeometry(0, 0, 333, 165);

    auto main_layout = std::make_unique<QGridLayout>();
    main_layout->addWidget(new QLabel(tr("Width")), 0, 1);
    main_layout->itemAt(main_layout->count()-1)->setAlignment(align_right_vcenter());

    main_layout->addWidget(new QLabel(tr("Height")), 0, 3);
    main_layout->addWidget(new QLabel(tr("Current size: ")), 1, 0);
    main_layout->itemAt(main_layout->count()-1)->setAlignment(align_right_vcenter());

    auto interact_flag = Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse;
    orig_width_label.ref().setTextInteractionFlags(interact_flag);
    orig_width_label.ref().setAlignment(align_right_vcenter());
    main_layout->addWidget(do_release(orig_width_label), 1, 1);

    auto label_tb(get_equal_labels(tr("⨯")));
    main_layout->addWidget(label_tb.at(0),1, 2);
    orig_height_label.ref().setTextInteractionFlags(interact_flag);
    main_layout->addWidget(do_release(orig_height_label),1, 3);

    auto new_size_label = std::make_unique<QLabel>(tr("&New size: "));
    new_size_label->setAlignment(align_right_vcenter());
    new_size_label->setBuddy(width_spin_box.uptr().get());
    main_layout->addWidget(new_size_label.release(), 2, 0);

    prepare_spinbox(width_spin_box.ref(), make_range(1, 99999), " px");
    main_layout->addWidget(do_release(width_spin_box), 2, 1);
    main_layout->addWidget(label_tb.at(1), 2, 2);
    prepare_spinbox(height_spin_box.ref(), make_range(1, 99999), " px");
    main_layout->addWidget(do_release(height_spin_box), 2, 3);

    prepare_spinbox(width_percent_spin_box.ref(), make_range(1, 1000), " %");
    main_layout->addWidget(do_release(width_percent_spin_box), 3, 1);
    main_layout->addWidget(label_tb.at(2), 3, 2);
    prepare_spinbox(height_percent_spin_box.ref(), make_range(1, 1000), " %");
    height_percent_spin_box.ref().setEnabled(false);
    main_layout->addWidget(do_release(height_percent_spin_box), 3, 3);
    keep_aspect_check_box.ref().setChecked(true);
    main_layout->addWidget(do_release(keep_aspect_check_box), 4, 1, 1, 3);
    setLayout(main_layout.release());
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------
