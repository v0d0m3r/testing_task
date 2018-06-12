//------------------------------------------------------------------------------

#include "Resize_image_dialog.hpp"

//------------------------------------------------------------------------------

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

//------------------------------------------------------------------------------

namespace Imager {

//------------------------------------------------------------------------------

Resize_image_dialog::Resize_image_dialog(QWidget* parent)
    : QDialog(parent),
      content(new Resize_image_widget()),
      update_from_ratio(false),
      update_from_size_or_percentage(false)
{
    auto main_layout = std::make_unique<QVBoxLayout>();
    main_layout->setSizeConstraint(QLayout::SetFixedSize);
    main_layout->addWidget(do_release(content));

    auto button_box = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok
                                                         | QDialogButtonBox::Cancel);
    auto ok_button = button_box->button(QDialogButtonBox::Ok);
    ok_button->setDefault(true);
    ok_button->setText("Resize");
    ok_button->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(button_box.get(), &QDialogButtonBox::accepted, this, &Resize_image_dialog::accept);
    connect(button_box.get(), &QDialogButtonBox::rejected, this, &Resize_image_dialog::reject);
    main_layout->addWidget(button_box.release());

    content.ref().layout()->setMargin(0);

    setWindowTitle(content.ref().windowTitle());
    content.ref().width_spin_box.ref().setFocus();

    connect(&content.ref().width_spin_box.ref(),
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &Resize_image_dialog::width_changed_cb);
    connect(&content.ref().height_spin_box.ref(),
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &Resize_image_dialog::height_changed_cb);
    connect(&content.ref().width_percent_spin_box.ref(),
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &Resize_image_dialog::width_percent_changed_cb);
    connect(&content.ref().height_percent_spin_box.ref(),
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &Resize_image_dialog::height_percent_changed_cb);
    connect(&content.ref().keep_aspect_check_box.ref(), &QCheckBox::toggled,
            this, &Resize_image_dialog::keep_aspect_changed_cb);
    setLayout(main_layout.release());
}

//------------------------------------------------------------------------------

void Resize_image_dialog::set_original_size(const QSize& size)
{
    orig_size = size;
    content.ref().orig_width_label.ref().setText(QString::number(size.width())
                                                 + QStringLiteral(" px"));
    content.ref().orig_height_label.ref().setText(QString::number(size.height())
                                                  + QStringLiteral(" px"));
    content.ref().width_spin_box.ref().setValue(size.width());
    content.ref().height_spin_box.ref().setValue(size.height());
}

//------------------------------------------------------------------------------

QSize Resize_image_dialog::size() const
{
    return QSize(
               content.ref().width_spin_box.ref().value(),
               content.ref().height_spin_box.ref().value()
           );
}

//------------------------------------------------------------------------------

void Resize_image_dialog::width_changed_cb(int width)
{
    // Update width percentage to match width, only if this was a manual adjustment
    if (!update_from_size_or_percentage && !update_from_ratio) {
        update_from_size_or_percentage = true;
        content.ref().width_percent_spin_box.ref().setValue((double(width) / orig_size.width()) * 100);
        update_from_size_or_percentage = false;
    }

    if (!content.ref().keep_aspect_check_box.ref().isChecked() || update_from_ratio)
        return;

    // Update height to keep ratio, only if ratio locked and this was a manual adjustment
    update_from_ratio = true;
    content.ref().height_spin_box.ref().setValue(orig_size.height() * width / orig_size.width());
    update_from_ratio = false;
}

//------------------------------------------------------------------------------

void Resize_image_dialog::height_changed_cb(int height)
{
    // Update height percentage to match height, only if this was a manual adjustment
    if (!update_from_size_or_percentage && !update_from_ratio) {
        update_from_size_or_percentage = true;
        content.ref().height_percent_spin_box.ref().setValue((double(height) / orig_size.height()) * 100);
        update_from_size_or_percentage = false;
    }

    if (!content.ref().keep_aspect_check_box.ref().isChecked() || update_from_ratio)
        return;

    // Update width to keep ratio, only if ratio locked and this was a manual adjustment
    update_from_ratio = true;
    content.ref().width_spin_box.ref().setValue(orig_size.width() * height / orig_size.height());
    update_from_ratio = false;
}

//------------------------------------------------------------------------------

void Resize_image_dialog::width_percent_changed_cb(double width_percent)
{
    // Update width to match width percentage, only if this was a manual adjustment
    if (!update_from_size_or_percentage && !update_from_ratio) {
        update_from_size_or_percentage = true;
        content.ref().width_spin_box.ref().setValue((width_percent / 100) * orig_size.width());
        update_from_size_or_percentage = false;
    }

    if (!content.ref().keep_aspect_check_box.ref().isChecked() || update_from_ratio) {
        return;
    }

    // Keep height percentage in sync with width percentage, only if ratio locked and this was a manual adjustment
    update_from_ratio = true;
    content.ref().height_percent_spin_box.ref().setValue(content.ref().width_percent_spin_box.ref().value());
    update_from_ratio = false;
}

//------------------------------------------------------------------------------

void Resize_image_dialog::height_percent_changed_cb(double height_percent)
{
    // Update height to match height percentage, only if this was a manual adjustment
    if (!update_from_size_or_percentage && !update_from_ratio) {
        update_from_size_or_percentage = true;
        content.ref().height_spin_box.ref().setValue((height_percent / 100) * orig_size.height());
        update_from_size_or_percentage = false;
    }

    if (!content.ref().keep_aspect_check_box.ref().isChecked() || update_from_ratio)
        return;

    // Keep height percentage in sync with width percentage, only if ratio locked and this was a manual adjustment
    update_from_ratio = true;
    content.ref().width_percent_spin_box.ref().setValue(content.ref().height_percent_spin_box.ref().value());
    update_from_ratio = false;
}

//------------------------------------------------------------------------------

void Resize_image_dialog::keep_aspect_changed_cb(bool value)
{
    if (value) {
        update_from_size_or_percentage = true;
        width_changed_cb(content.ref().width_spin_box.ref().value());
        width_percent_changed_cb(content.ref().width_percent_spin_box.ref().value());
        update_from_size_or_percentage = false;
    }
}

//------------------------------------------------------------------------------

Compress_image_dialog::Compress_image_dialog(QWidget* parent)
    : QDialog(parent),
      content(new Compress_image_widget())
{
    auto main_layout = std::make_unique<QVBoxLayout>();
    main_layout->setSizeConstraint(QLayout::SetFixedSize);
    main_layout->addWidget(do_release(content));

    auto button_box = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok
                                                         | QDialogButtonBox::Cancel);
    auto ok_button = button_box->button(QDialogButtonBox::Ok);
    ok_button->setDefault(true);
    ok_button->setText("Compress");
    ok_button->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(button_box.get(), &QDialogButtonBox::accepted, this, &Resize_image_dialog::accept);
    connect(button_box.get(), &QDialogButtonBox::rejected, this, &Resize_image_dialog::reject);
    main_layout->addWidget(button_box.release());

    content.ref().layout()->setMargin(0);

    setWindowTitle(content.ref().windowTitle());
    content.ref().quality_spin_box.ref().setFocus();
    setLayout(main_layout.release());
}

//------------------------------------------------------------------------------

void Compress_image_dialog::set_original_info(const Compression_info& info)
{
    orig_info = info;
    content.ref().orig_quality_label.ref().setText(QString::number(info.quality));
    content.ref().orig_compress_label.ref().setText(QString::number(info.compression_level));
    content.ref().quality_spin_box.ref().setValue(info.quality);
    content.ref().compress_spin_box.ref().setValue(info.compression_level);
}

//------------------------------------------------------------------------------

Compression_info Compress_image_dialog::compression_info() const
{
    Compression_info ci = {
        content.ref().quality_spin_box.ref().value(),
        content.ref().compress_spin_box.ref().value()
    };
    return ci;
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------
