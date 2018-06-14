//------------------------------------------------------------------------------

#ifndef IMAGE_VIEWER_HPP
#define IMAGE_VIEWER_HPP

//------------------------------------------------------------------------------
//Qt
#include <QMainWindow>
#include <QImage>

//------------------------------------------------------------------------------
// Local
#include "../imager_lib/Transmit_image_dialog.hpp"
#include "../imager_lib/Resize_image_dialog.hpp"

//------------------------------------------------------------------------------

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

//------------------------------------------------------------------------------

namespace Imager {

//------------------------------------------------------------------------------

struct Image_features {
    QSize size = QSize();
    Compression_info info;

    explicit Image_features(QSize sz, int quality = -1,
                            int compression_level = -1)
        : size(sz)
    {
        info = {quality, compression_level};
    }
    Image_features() {}
};

//------------------------------------------------------------------------------

class Image_viewer : public QMainWindow {
    Q_OBJECT
public:
    Image_viewer();

    bool load_file(const QString&);

private slots:
    void open_cb();
    void save_as_cb();
    void save_cb();
    void copy_cb();
    void paste_cb();
    void resize_cb();
    void compress_cb();
    void previous_cb();
    void zoom_in_cb();
    void zoom_out_cb();
    void normal_size_cb();
    void fit_to_window_cb();
    void transmission_cb();
    void about_cb();

private:
    void create_actions();
    void update_actions();
    bool save_file(const QString&);
    void set_image(const QImage&);
    void scale_image(double);
    void adjust_scroll_bar(QScrollBar*, double);

    QImage image;
    QString curr_name_image;
    Image_features prev_img_feat;
    Image_features curr_img_feat;

    QLabel* image_label;
    QScrollArea* scroll_area;
    double scale_factor;

    QAction* save_act;
    QAction* save_as_act;
    QAction* copy_act;
    QAction* zoom_in_act;
    QAction* zoom_out_act;
    QAction* normal_size_act;
    QAction* fit_to_window_act;
    QAction* resize_act;
    QAction* compress_act;
    QAction* previous_state_act;
    QAction* transmission_act;
};

//------------------------------------------------------------------------------

}   // Imager

//------------------------------------------------------------------------------

#endif // IMAGE_VIEWER_HPP

//------------------------------------------------------------------------------
