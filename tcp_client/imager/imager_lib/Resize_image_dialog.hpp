//------------------------------------------------------------------------------

#ifndef RESIZE_IMAGE_DIALOG_HPP
#define RESIZE_IMAGE_DIALOG_HPP

#include <QDialog>

//------------------------------------------------------------------------------

#include "Resize_image_widget.hpp"

//------------------------------------------------------------------------------

namespace Imager {

//------------------------------------------------------------------------------

class Resize_image_dialog : public QDialog {
    Q_OBJECT
public:
    Resize_image_dialog(QWidget* parent = nullptr);

    void set_original_size(const QSize&);
    QSize size() const;

private slots:
    void width_changed_cb(int);
    void height_changed_cb(int);
    void width_percent_changed_cb(double);
    void height_percent_changed_cb(double);
    void keep_aspect_changed_cb(bool);

private:
    Ref_unique_ptr<Resize_image_widget> content;
    bool update_from_ratio;
    bool update_from_size_or_percentage;
    QSize orig_size;
};

//------------------------------------------------------------------------------

struct Compression_info {
    int quality = 0;
    int compression_level = 0;

    bool operator ==(const Compression_info& ci) const noexcept
    {
        return      this->quality == ci.quality
                &&  this->compression_level == ci.compression_level;
    }

    bool operator !=(const Compression_info& ci) const noexcept
    { return !(*this==ci); }
};

//------------------------------------------------------------------------------

class Compress_image_dialog : public QDialog {
    Q_OBJECT
public:
    Compress_image_dialog(QWidget* parent = nullptr);

    void set_original_info(const Compression_info&);
    Compression_info compression_info() const;

private:
    Ref_unique_ptr<Compress_image_widget> content;
    Compression_info orig_info;
};

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // RESIZE_IMAGE_DIALOG_HPP

//------------------------------------------------------------------------------
